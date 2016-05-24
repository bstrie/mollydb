/* -------------------------------------------------------------------------
 *
 * contrib/semdb/uavc.c
 *
 * Implementation of userspace access vector cache; that enables to cache
 * access control decisions recently used, and reduce number of kernel
 * invocations to avoid unnecessary performance hit.
 *
 * Copyright (c) 2011-2016, MollyDB Global Development Group
 *
 * -------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "access/hash.h"
#include "catalog/mdb_proc.h"
#include "commands/seclabel.h"
#include "storage/ipc.h"
#include "utils/guc.h"
#include "utils/memutils.h"

#include "semdb.h"

/*
 * avc_cache
 *
 * It enables to cache access control decision (and behavior on execution of
 * trusted procedure, db_procedure class only) for a particular pair of
 * security labels and object class in userspace.
 */
typedef struct
{
	uint32		hash;			/* hash value of this cache entry */
	char	   *scontext;		/* security context of the subject */
	char	   *tcontext;		/* security context of the target */
	uint16		tclass;			/* object class of the target */

	uint32		allowed;		/* permissions to be allowed */
	uint32		auditallow;		/* permissions to be audited on allowed */
	uint32		auditdeny;		/* permissions to be audited on denied */

	bool		permissive;		/* true, if permissive rule */
	bool		hot_cache;		/* true, if recently referenced */
	bool		tcontext_is_valid;
	/* true, if tcontext is valid */
	char	   *ncontext;		/* temporary scontext on execution of trusted
								 * procedure, or NULL elsewhere */
}	avc_cache;

/*
 * Declaration of static variables
 */
#define AVC_NUM_SLOTS		512
#define AVC_NUM_RECLAIM		16
#define AVC_DEF_THRESHOLD	384

static MemoryContext avc_mem_cxt;
static List *avc_slots[AVC_NUM_SLOTS];	/* avc's hash buckets */
static int	avc_num_caches;		/* number of caches currently used */
static int	avc_lru_hint;		/* index of the buckets to be reclaimed next */
static int	avc_threshold;		/* threshold to launch cache-reclaiming  */
static char *avc_unlabeled;		/* system 'unlabeled' label */

/*
 * Hash function
 */
static uint32
semdb_avc_hash(const char *scontext, const char *tcontext, uint16 tclass)
{
	return hash_any((const unsigned char *) scontext, strlen(scontext))
		^ hash_any((const unsigned char *) tcontext, strlen(tcontext))
		^ tclass;
}

/*
 * Reset all the avc caches
 */
static void
semdb_avc_reset(void)
{
	MemoryContextReset(avc_mem_cxt);

	memset(avc_slots, 0, sizeof(List *) * AVC_NUM_SLOTS);
	avc_num_caches = 0;
	avc_lru_hint = 0;
	avc_unlabeled = NULL;
}

/*
 * Reclaim caches recently unreferenced
 */
static void
semdb_avc_reclaim(void)
{
	ListCell   *cell;
	ListCell   *next;
	ListCell   *prev;
	int			index;

	while (avc_num_caches >= avc_threshold - AVC_NUM_RECLAIM)
	{
		index = avc_lru_hint;

		prev = NULL;
		for (cell = list_head(avc_slots[index]); cell; cell = next)
		{
			avc_cache  *cache = lfirst(cell);

			next = lnext(cell);
			if (!cache->hot_cache)
			{
				avc_slots[index]
					= list_delete_cell(avc_slots[index], cell, prev);

				pfree(cache->scontext);
				pfree(cache->tcontext);
				if (cache->ncontext)
					pfree(cache->ncontext);
				pfree(cache);

				avc_num_caches--;
			}
			else
			{
				cache->hot_cache = false;
				prev = cell;
			}
		}
		avc_lru_hint = (avc_lru_hint + 1) % AVC_NUM_SLOTS;
	}
}

/* -------------------------------------------------------------------------
 *
 * semdb_avc_check_valid
 *
 * This function checks whether the cached entries are still valid.  If
 * the security policy has been reloaded (or any other events that requires
 * resetting userspace caches has occurred) since the last reference to
 * the access vector cache, we must flush the cache.
 *
 * Access control decisions must be atomic, but multiple system calls may
 * be required to make a decision; thus, when referencing the access vector
 * cache, we must loop until we complete without an intervening cache flush
 * event.  In practice, looping even once should be very rare.  Callers should
 * do something like this:
 *
 *	 semdb_avc_check_valid();
 *	 do {
 *			 :
 *		 <reference to uavc>
 *			 :
 *	 } while (!semdb_avc_check_valid())
 *
 * -------------------------------------------------------------------------
 */
static bool
semdb_avc_check_valid(void)
{
	if (selinux_status_updated() > 0)
	{
		semdb_avc_reset();

		return false;
	}
	return true;
}

/*
 * semdb_avc_unlabeled
 *
 * Returns an alternative label to be applied when no label or an invalid
 * label would otherwise be assigned.
 */
static char *
semdb_avc_unlabeled(void)
{
	if (!avc_unlabeled)
	{
		security_context_t unlabeled;

		if (security_get_initial_context_raw("unlabeled", &unlabeled) < 0)
			ereport(ERROR,
					(errcode(ERRCODE_INTERNAL_ERROR),
			   errmsg("SELinux: failed to get initial security label: %m")));
		MDB_TRY();
		{
			avc_unlabeled = MemoryContextStrdup(avc_mem_cxt, unlabeled);
		}
		MDB_CATCH();
		{
			freecon(unlabeled);
			MDB_RE_THROW();
		}
		MDB_END_TRY();

		freecon(unlabeled);
	}
	return avc_unlabeled;
}

/*
 * semdb_avc_compute
 *
 * A fallback path, when cache mishit. It asks SELinux its access control
 * decision for the supplied pair of security context and object class.
 */
static avc_cache *
semdb_avc_compute(const char *scontext, const char *tcontext, uint16 tclass)
{
	char	   *ucontext = NULL;
	char	   *ncontext = NULL;
	MemoryContext oldctx;
	avc_cache  *cache;
	uint32		hash;
	int			index;
	struct av_decision avd;

	hash = semdb_avc_hash(scontext, tcontext, tclass);
	index = hash % AVC_NUM_SLOTS;

	/*
	 * Validation check of the supplied security context. Because it always
	 * invoke system-call, frequent check should be avoided. Unless security
	 * policy is reloaded, validation status shall be kept, so we also cache
	 * whether the supplied security context was valid, or not.
	 */
	if (security_check_context_raw((security_context_t) tcontext) != 0)
		ucontext = semdb_avc_unlabeled();

	/*
	 * Ask SELinux its access control decision
	 */
	if (!ucontext)
		semdb_compute_avd(scontext, tcontext, tclass, &avd);
	else
		semdb_compute_avd(scontext, ucontext, tclass, &avd);

	/*
	 * It also caches a security label to be switched when a client labeled as
	 * 'scontext' executes a procedure labeled as 'tcontext', not only access
	 * control decision on the procedure. The security label to be switched
	 * shall be computed uniquely on a pair of 'scontext' and 'tcontext',
	 * thus, it is reasonable to cache the new label on avc, and enables to
	 * reduce unnecessary system calls. It shall be referenced at
	 * semdb_needs_fmgr_hook to check whether the supplied function is a
	 * trusted procedure, or not.
	 */
	if (tclass == SEMDB_CLASS_DB_PROCEDURE)
	{
		if (!ucontext)
			ncontext = semdb_compute_create(scontext, tcontext,
											  SEMDB_CLASS_PROCESS, NULL);
		else
			ncontext = semdb_compute_create(scontext, ucontext,
											  SEMDB_CLASS_PROCESS, NULL);
		if (strcmp(scontext, ncontext) == 0)
		{
			pfree(ncontext);
			ncontext = NULL;
		}
	}

	/*
	 * Set up an avc_cache object
	 */
	oldctx = MemoryContextSwitchTo(avc_mem_cxt);

	cache = palloc0(sizeof(avc_cache));

	cache->hash = hash;
	cache->scontext = pstrdup(scontext);
	cache->tcontext = pstrdup(tcontext);
	cache->tclass = tclass;

	cache->allowed = avd.allowed;
	cache->auditallow = avd.auditallow;
	cache->auditdeny = avd.auditdeny;
	cache->hot_cache = true;
	if (avd.flags & SELINUX_AVD_FLAGS_PERMISSIVE)
		cache->permissive = true;
	if (!ucontext)
		cache->tcontext_is_valid = true;
	if (ncontext)
		cache->ncontext = pstrdup(ncontext);

	avc_num_caches++;

	if (avc_num_caches > avc_threshold)
		semdb_avc_reclaim();

	avc_slots[index] = lcons(cache, avc_slots[index]);

	MemoryContextSwitchTo(oldctx);

	return cache;
}

/*
 * semdb_avc_lookup
 *
 * Look up a cache entry that matches the supplied security contexts and
 * object class.  If not found, create a new cache entry.
 */
static avc_cache *
semdb_avc_lookup(const char *scontext, const char *tcontext, uint16 tclass)
{
	avc_cache  *cache;
	ListCell   *cell;
	uint32		hash;
	int			index;

	hash = semdb_avc_hash(scontext, tcontext, tclass);
	index = hash % AVC_NUM_SLOTS;

	foreach(cell, avc_slots[index])
	{
		cache = lfirst(cell);

		if (cache->hash == hash &&
			cache->tclass == tclass &&
			strcmp(cache->tcontext, tcontext) == 0 &&
			strcmp(cache->scontext, scontext) == 0)
		{
			cache->hot_cache = true;
			return cache;
		}
	}
	/* not found, so insert a new cache */
	return semdb_avc_compute(scontext, tcontext, tclass);
}

/*
 * semdb_avc_check_perms(_label)
 *
 * It returns 'true', if the security policy suggested to allow the required
 * permissions. Otherwise, it returns 'false' or raises an error according
 * to the 'abort_on_violation' argument.
 * The 'tobject' and 'tclass' identify the target object being referenced,
 * and 'required' is a bitmask of permissions (SEMDB_*__*) defined for each
 * object classes.
 * The 'audit_name' is the object name (optional). If SEPGSQL_AVC_NOAUDIT
 * was supplied, it means to skip all the audit messages.
 */
bool
semdb_avc_check_perms_label(const char *tcontext,
							  uint16 tclass, uint32 required,
							  const char *audit_name,
							  bool abort_on_violation)
{
	char	   *scontext = semdb_get_client_label();
	avc_cache  *cache;
	uint32		denied;
	uint32		audited;
	bool		result;

	semdb_avc_check_valid();
	do
	{
		result = true;

		/*
		 * If the target object is unlabeled, we perform the check using the
		 * label supplied by semdb_avc_unlabeled().
		 */
		if (tcontext)
			cache = semdb_avc_lookup(scontext, tcontext, tclass);
		else
			cache = semdb_avc_lookup(scontext,
									   semdb_avc_unlabeled(), tclass);

		denied = required & ~cache->allowed;

		/*
		 * Compute permissions to be audited
		 */
		if (semdb_get_debug_audit())
			audited = (denied ? (denied & ~0) : (required & ~0));
		else
			audited = denied ? (denied & cache->auditdeny)
				: (required & cache->auditallow);

		if (denied)
		{
			/*
			 * In permissive mode or permissive domain, violated permissions
			 * shall be audited to the log files at once, and then implicitly
			 * allowed to avoid a flood of access denied logs, because the
			 * purpose of permissive mode/domain is to collect a violation log
			 * that will make it possible to fix up the security policy.
			 */
			if (!semdb_getenforce() || cache->permissive)
				cache->allowed |= required;
			else
				result = false;
		}
	} while (!semdb_avc_check_valid());

	/*
	 * In the case when we have something auditable actions here,
	 * semdb_audit_log shall be called with text representation of security
	 * labels for both of subject and object. It records this access
	 * violation, so DBA will be able to find out unexpected security problems
	 * later.
	 */
	if (audited != 0 &&
		audit_name != SEPGSQL_AVC_NOAUDIT &&
		semdb_get_mode() != SEPGSQL_MODE_INTERNAL)
	{
		semdb_audit_log(denied != 0,
						  cache->scontext,
						  cache->tcontext_is_valid ?
						  cache->tcontext : semdb_avc_unlabeled(),
						  cache->tclass,
						  audited,
						  audit_name);
	}

	if (abort_on_violation && !result)
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
				 errmsg("SELinux: security policy violation")));

	return result;
}

bool
semdb_avc_check_perms(const ObjectAddress *tobject,
						uint16 tclass, uint32 required,
						const char *audit_name,
						bool abort_on_violation)
{
	char	   *tcontext = GetSecurityLabel(tobject, SEPGSQL_LABEL_TAG);
	bool		rc;

	rc = semdb_avc_check_perms_label(tcontext,
									   tclass, required,
									   audit_name, abort_on_violation);
	if (tcontext)
		pfree(tcontext);

	return rc;
}

/*
 * semdb_avc_trusted_proc
 *
 * If the supplied function OID is configured as a trusted procedure, this
 * function will return a security label to be used during the execution of
 * that function.  Otherwise, it returns NULL.
 */
char *
semdb_avc_trusted_proc(Oid functionId)
{
	char	   *scontext = semdb_get_client_label();
	char	   *tcontext;
	ObjectAddress tobject;
	avc_cache  *cache;

	tobject.classId = ProcedureRelationId;
	tobject.objectId = functionId;
	tobject.objectSubId = 0;
	tcontext = GetSecurityLabel(&tobject, SEPGSQL_LABEL_TAG);

	semdb_avc_check_valid();
	do
	{
		if (tcontext)
			cache = semdb_avc_lookup(scontext, tcontext,
									   SEMDB_CLASS_DB_PROCEDURE);
		else
			cache = semdb_avc_lookup(scontext, semdb_avc_unlabeled(),
									   SEMDB_CLASS_DB_PROCEDURE);
	} while (!semdb_avc_check_valid());

	return cache->ncontext;
}

/*
 * semdb_avc_exit
 *
 * Clean up userspace AVC on process exit.
 */
static void
semdb_avc_exit(int code, Datum arg)
{
	selinux_status_close();
}

/*
 * semdb_avc_init
 *
 * Initialize the userspace AVC.  This should be called from _MDB_init.
 */
void
semdb_avc_init(void)
{
	int			rc;

	/*
	 * All the avc stuff shall be allocated on avc_mem_cxt
	 */
	avc_mem_cxt = AllocSetContextCreate(TopMemoryContext,
										"userspace access vector cache",
										ALLOCSET_DEFAULT_MINSIZE,
										ALLOCSET_DEFAULT_INITSIZE,
										ALLOCSET_DEFAULT_MAXSIZE);
	memset(avc_slots, 0, sizeof(avc_slots));
	avc_num_caches = 0;
	avc_lru_hint = 0;
	avc_threshold = AVC_DEF_THRESHOLD;

	/*
	 * SELinux allows to mmap(2) its kernel status page in read-only mode to
	 * inform userspace applications its status updating (such as policy
	 * reloading) without system-call invocations. This feature is only
	 * supported in Linux-2.6.38 or later, however, libselinux provides a
	 * fallback mode to know its status using netlink sockets.
	 */
	rc = selinux_status_open(1);
	if (rc < 0)
		ereport(ERROR,
				(errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("SELinux: could not open selinux status : %m")));
	else if (rc > 0)
		ereport(LOG,
				(errmsg("SELinux: kernel status page uses fallback mode")));

	/* Arrange to close selinux status page on process exit. */
	on_proc_exit(semdb_avc_exit, 0);
}
