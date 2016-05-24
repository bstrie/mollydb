/*-------------------------------------------------------------------------
 *
 * acl.h
 *	  Definition of (and support for) access control list data structures.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/acl.h
 *
 * NOTES
 *	  An ACL array is simply an array of AclItems, representing the union
 *	  of the privileges represented by the individual items.  A zero-length
 *	  array represents "no privileges".  There are no assumptions about the
 *	  ordering of the items, but we do expect that there are no two entries
 *	  in the array with the same grantor and grantee.
 *
 *	  For backward-compatibility purposes we have to allow null ACL entries
 *	  in system catalogs.  A null ACL will be treated as meaning "default
 *	  protection" (i.e., whatever acldefault() returns).
 *-------------------------------------------------------------------------
 */
#ifndef ACL_H
#define ACL_H

#include "access/htup.h"
#include "nodes/parsenodes.h"
#include "utils/array.h"
#include "utils/snapshot.h"


/*
 * typedef AclMode is declared in parsenodes.h, also the individual privilege
 * bit meanings are defined there
 */

#define ACL_ID_PUBLIC	0		/* placeholder for id in a PUBLIC acl item */

/*
 * AclItem
 *
 * Note: must be same size on all platforms, because the size is hardcoded
 * in the mdb_type.h entry for aclitem.
 */
typedef struct AclItem
{
	Oid			ai_grantee;		/* ID that this item grants privs to */
	Oid			ai_grantor;		/* grantor of privs */
	AclMode		ai_privs;		/* privilege bits */
} AclItem;

/*
 * The upper 16 bits of the ai_privs field of an AclItem are the grant option
 * bits, and the lower 16 bits are the actual privileges.  We use "rights"
 * to mean the combined grant option and privilege bits fields.
 */
#define ACLITEM_GET_PRIVS(item)    ((item).ai_privs & 0xFFFF)
#define ACLITEM_GET_GOPTIONS(item) (((item).ai_privs >> 16) & 0xFFFF)
#define ACLITEM_GET_RIGHTS(item)   ((item).ai_privs)

#define ACL_GRANT_OPTION_FOR(privs) (((AclMode) (privs) & 0xFFFF) << 16)
#define ACL_OPTION_TO_PRIVS(privs)	(((AclMode) (privs) >> 16) & 0xFFFF)

#define ACLITEM_SET_PRIVS(item,privs) \
  ((item).ai_privs = ((item).ai_privs & ~((AclMode) 0xFFFF)) | \
					 ((AclMode) (privs) & 0xFFFF))
#define ACLITEM_SET_GOPTIONS(item,goptions) \
  ((item).ai_privs = ((item).ai_privs & ~(((AclMode) 0xFFFF) << 16)) | \
					 (((AclMode) (goptions) & 0xFFFF) << 16))
#define ACLITEM_SET_RIGHTS(item,rights) \
  ((item).ai_privs = (AclMode) (rights))

#define ACLITEM_SET_PRIVS_GOPTIONS(item,privs,goptions) \
  ((item).ai_privs = ((AclMode) (privs) & 0xFFFF) | \
					 (((AclMode) (goptions) & 0xFFFF) << 16))


#define ACLITEM_ALL_PRIV_BITS		((AclMode) 0xFFFF)
#define ACLITEM_ALL_GOPTION_BITS	((AclMode) 0xFFFF << 16)

/*
 * Definitions for convenient access to Acl (array of AclItem).
 * These are standard MollyDB arrays, but are restricted to have one
 * dimension and no nulls.  We also ignore the lower bound when reading,
 * and set it to one when writing.
 *
 * CAUTION: as of MollyDB 7.1, these arrays are toastable (just like all
 * other array types).  Therefore, be careful to detoast them with the
 * macros provided, unless you know for certain that a particular array
 * can't have been toasted.
 */


/*
 * Acl			a one-dimensional array of AclItem
 */
typedef ArrayType Acl;

#define ACL_NUM(ACL)			(ARR_DIMS(ACL)[0])
#define ACL_DAT(ACL)			((AclItem *) ARR_DATA_PTR(ACL))
#define ACL_N_SIZE(N)			(ARR_OVERHEAD_NONULLS(1) + ((N) * sizeof(AclItem)))
#define ACL_SIZE(ACL)			ARR_SIZE(ACL)

/*
 * fmgr macros for these types
 */
#define DatumGetAclItemP(X)		   ((AclItem *) DatumGetPointer(X))
#define MDB_GETARG_ACLITEM_P(n)	   DatumGetAclItemP(MDB_GETARG_DATUM(n))
#define MDB_RETURN_ACLITEM_P(x)	   MDB_RETURN_POINTER(x)

#define DatumGetAclP(X)			   ((Acl *) MDB_DETOAST_DATUM(X))
#define DatumGetAclPCopy(X)		   ((Acl *) MDB_DETOAST_DATUM_COPY(X))
#define MDB_GETARG_ACL_P(n)		   DatumGetAclP(MDB_GETARG_DATUM(n))
#define MDB_GETARG_ACL_P_COPY(n)    DatumGetAclPCopy(MDB_GETARG_DATUM(n))
#define MDB_RETURN_ACL_P(x)		   MDB_RETURN_POINTER(x)

/*
 * ACL modification opcodes for aclupdate
 */
#define ACL_MODECHG_ADD			1
#define ACL_MODECHG_DEL			2
#define ACL_MODECHG_EQL			3

/*
 * External representations of the privilege bits --- aclitemin/aclitemout
 * represent each possible privilege bit with a distinct 1-character code
 */
#define ACL_INSERT_CHR			'a'		/* formerly known as "append" */
#define ACL_SELECT_CHR			'r'		/* formerly known as "read" */
#define ACL_UPDATE_CHR			'w'		/* formerly known as "write" */
#define ACL_DELETE_CHR			'd'
#define ACL_TRUNCATE_CHR		'D'		/* super-delete, as it were */
#define ACL_REFERENCES_CHR		'x'
#define ACL_TRIGGER_CHR			't'
#define ACL_EXECUTE_CHR			'X'
#define ACL_USAGE_CHR			'U'
#define ACL_CREATE_CHR			'C'
#define ACL_CREATE_TEMP_CHR		'T'
#define ACL_CONNECT_CHR			'c'

/* string holding all privilege code chars, in order by bitmask position */
#define ACL_ALL_RIGHTS_STR	"arwdDxtXUCTc"

/*
 * Bitmasks defining "all rights" for each supported object type
 */
#define ACL_ALL_RIGHTS_COLUMN		(ACL_INSERT|ACL_SELECT|ACL_UPDATE|ACL_REFERENCES)
#define ACL_ALL_RIGHTS_RELATION		(ACL_INSERT|ACL_SELECT|ACL_UPDATE|ACL_DELETE|ACL_TRUNCATE|ACL_REFERENCES|ACL_TRIGGER)
#define ACL_ALL_RIGHTS_SEQUENCE		(ACL_USAGE|ACL_SELECT|ACL_UPDATE)
#define ACL_ALL_RIGHTS_DATABASE		(ACL_CREATE|ACL_CREATE_TEMP|ACL_CONNECT)
#define ACL_ALL_RIGHTS_FDW			(ACL_USAGE)
#define ACL_ALL_RIGHTS_FOREIGN_SERVER (ACL_USAGE)
#define ACL_ALL_RIGHTS_FUNCTION		(ACL_EXECUTE)
#define ACL_ALL_RIGHTS_LANGUAGE		(ACL_USAGE)
#define ACL_ALL_RIGHTS_LARGEOBJECT	(ACL_SELECT|ACL_UPDATE)
#define ACL_ALL_RIGHTS_NAMESPACE	(ACL_USAGE|ACL_CREATE)
#define ACL_ALL_RIGHTS_TABLESPACE	(ACL_CREATE)
#define ACL_ALL_RIGHTS_TYPE			(ACL_USAGE)

/* operation codes for mdb_*_aclmask */
typedef enum
{
	ACLMASK_ALL,				/* normal case: compute all bits */
	ACLMASK_ANY					/* return when result is known nonzero */
} AclMaskHow;

/* result codes for mdb_*_aclcheck */
typedef enum
{
	ACLCHECK_OK = 0,
	ACLCHECK_NO_PRIV,
	ACLCHECK_NOT_OWNER
} AclResult;

/* this enum covers all object types that can have privilege errors */
/* currently it's only used to tell aclcheck_error what to say */
typedef enum AclObjectKind
{
	ACL_KIND_COLUMN,			/* mdb_attribute */
	ACL_KIND_CLASS,				/* mdb_class */
	ACL_KIND_SEQUENCE,			/* mdb_sequence */
	ACL_KIND_DATABASE,			/* mdb_database */
	ACL_KIND_PROC,				/* mdb_proc */
	ACL_KIND_OPER,				/* mdb_operator */
	ACL_KIND_TYPE,				/* mdb_type */
	ACL_KIND_LANGUAGE,			/* mdb_language */
	ACL_KIND_LARGEOBJECT,		/* mdb_largeobject */
	ACL_KIND_NAMESPACE,			/* mdb_namespace */
	ACL_KIND_OPCLASS,			/* mdb_opclass */
	ACL_KIND_OPFAMILY,			/* mdb_opfamily */
	ACL_KIND_COLLATION,			/* mdb_collation */
	ACL_KIND_CONVERSION,		/* mdb_conversion */
	ACL_KIND_TABLESPACE,		/* mdb_tablespace */
	ACL_KIND_TSDICTIONARY,		/* mdb_ts_dict */
	ACL_KIND_TSCONFIGURATION,	/* mdb_ts_config */
	ACL_KIND_FDW,				/* mdb_foreign_data_wrapper */
	ACL_KIND_FOREIGN_SERVER,	/* mdb_foreign_server */
	ACL_KIND_EVENT_TRIGGER,		/* mdb_event_trigger */
	ACL_KIND_EXTENSION,			/* mdb_extension */
	MAX_ACL_KIND				/* MUST BE LAST */
} AclObjectKind;


/*
 * routines used internally
 */
extern Acl *acldefault(GrantObjectType objtype, Oid ownerId);
extern Acl *get_user_default_acl(GrantObjectType objtype, Oid ownerId,
					 Oid nsp_oid);

extern Acl *aclupdate(const Acl *old_acl, const AclItem *mod_aip,
		  int modechg, Oid ownerId, DropBehavior behavior);
extern Acl *aclnewowner(const Acl *old_acl, Oid oldOwnerId, Oid newOwnerId);
extern Acl *make_empty_acl(void);
extern Acl *aclcopy(const Acl *orig_acl);
extern Acl *aclconcat(const Acl *left_acl, const Acl *right_acl);
extern Acl *aclmerge(const Acl *left_acl, const Acl *right_acl, Oid ownerId);
extern void aclitemsort(Acl *acl);
extern bool aclequal(const Acl *left_acl, const Acl *right_acl);

extern AclMode aclmask(const Acl *acl, Oid roleid, Oid ownerId,
		AclMode mask, AclMaskHow how);
extern int	aclmembers(const Acl *acl, Oid **roleids);

extern bool has_privs_of_role(Oid member, Oid role);
extern bool is_member_of_role(Oid member, Oid role);
extern bool is_member_of_role_nosuper(Oid member, Oid role);
extern bool is_admin_of_role(Oid member, Oid role);
extern void check_is_member_of_role(Oid member, Oid role);
extern Oid	get_role_oid(const char *rolename, bool missing_ok);
extern Oid	get_role_oid_or_public(const char *rolename);
extern Oid	get_rolespec_oid(const Node *node, bool missing_ok);
extern void	check_rolespec_name(const Node *node, const char *detail_msg);
extern HeapTuple get_rolespec_tuple(const Node *node);
extern char *get_rolespec_name(const Node *node);

extern void select_best_grantor(Oid roleId, AclMode privileges,
					const Acl *acl, Oid ownerId,
					Oid *grantorId, AclMode *grantOptions);

extern void initialize_acl(void);

/*
 * SQL functions (from acl.c)
 */
extern Datum aclitemin(MDB_FUNCTION_ARGS);
extern Datum aclitemout(MDB_FUNCTION_ARGS);
extern Datum aclinsert(MDB_FUNCTION_ARGS);
extern Datum aclremove(MDB_FUNCTION_ARGS);
extern Datum aclcontains(MDB_FUNCTION_ARGS);
extern Datum makeaclitem(MDB_FUNCTION_ARGS);
extern Datum aclitem_eq(MDB_FUNCTION_ARGS);
extern Datum hash_aclitem(MDB_FUNCTION_ARGS);
extern Datum acldefault_sql(MDB_FUNCTION_ARGS);
extern Datum aclexplode(MDB_FUNCTION_ARGS);

/*
 * prototypes for functions in aclchk.c
 */
extern void ExecuteGrantStmt(GrantStmt *stmt);
extern void ExecAlterDefaultPrivilegesStmt(AlterDefaultPrivilegesStmt *stmt);

extern void RemoveRoleFromObjectACL(Oid roleid, Oid classid, Oid objid);
extern void RemoveDefaultACLById(Oid defaclOid);

extern AclMode mdb_attribute_aclmask(Oid table_oid, AttrNumber attnum,
					 Oid roleid, AclMode mask, AclMaskHow how);
extern AclMode mdb_class_aclmask(Oid table_oid, Oid roleid,
				 AclMode mask, AclMaskHow how);
extern AclMode mdb_database_aclmask(Oid db_oid, Oid roleid,
					AclMode mask, AclMaskHow how);
extern AclMode mdb_proc_aclmask(Oid proc_oid, Oid roleid,
				AclMode mask, AclMaskHow how);
extern AclMode mdb_language_aclmask(Oid lang_oid, Oid roleid,
					AclMode mask, AclMaskHow how);
extern AclMode mdb_largeobject_aclmask_snapshot(Oid lobj_oid, Oid roleid,
							AclMode mask, AclMaskHow how, Snapshot snapshot);
extern AclMode mdb_namespace_aclmask(Oid nsp_oid, Oid roleid,
					 AclMode mask, AclMaskHow how);
extern AclMode mdb_tablespace_aclmask(Oid spc_oid, Oid roleid,
					  AclMode mask, AclMaskHow how);
extern AclMode mdb_foreign_data_wrapper_aclmask(Oid fdw_oid, Oid roleid,
								AclMode mask, AclMaskHow how);
extern AclMode mdb_foreign_server_aclmask(Oid srv_oid, Oid roleid,
						  AclMode mask, AclMaskHow how);
extern AclMode mdb_type_aclmask(Oid type_oid, Oid roleid,
				AclMode mask, AclMaskHow how);

extern AclResult mdb_attribute_aclcheck(Oid table_oid, AttrNumber attnum,
					  Oid roleid, AclMode mode);
extern AclResult mdb_attribute_aclcheck_all(Oid table_oid, Oid roleid,
						  AclMode mode, AclMaskHow how);
extern AclResult mdb_class_aclcheck(Oid table_oid, Oid roleid, AclMode mode);
extern AclResult mdb_database_aclcheck(Oid db_oid, Oid roleid, AclMode mode);
extern AclResult mdb_proc_aclcheck(Oid proc_oid, Oid roleid, AclMode mode);
extern AclResult mdb_language_aclcheck(Oid lang_oid, Oid roleid, AclMode mode);
extern AclResult mdb_largeobject_aclcheck_snapshot(Oid lang_oid, Oid roleid,
								 AclMode mode, Snapshot snapshot);
extern AclResult mdb_namespace_aclcheck(Oid nsp_oid, Oid roleid, AclMode mode);
extern AclResult mdb_tablespace_aclcheck(Oid spc_oid, Oid roleid, AclMode mode);
extern AclResult mdb_foreign_data_wrapper_aclcheck(Oid fdw_oid, Oid roleid, AclMode mode);
extern AclResult mdb_foreign_server_aclcheck(Oid srv_oid, Oid roleid, AclMode mode);
extern AclResult mdb_type_aclcheck(Oid type_oid, Oid roleid, AclMode mode);

extern void aclcheck_error(AclResult aclerr, AclObjectKind objectkind,
			   const char *objectname);

extern void aclcheck_error_col(AclResult aclerr, AclObjectKind objectkind,
				   const char *objectname, const char *colname);

extern void aclcheck_error_type(AclResult aclerr, Oid typeOid);

/* ownercheck routines just return true (owner) or false (not) */
extern bool mdb_class_ownercheck(Oid class_oid, Oid roleid);
extern bool mdb_type_ownercheck(Oid type_oid, Oid roleid);
extern bool mdb_oper_ownercheck(Oid oper_oid, Oid roleid);
extern bool mdb_proc_ownercheck(Oid proc_oid, Oid roleid);
extern bool mdb_language_ownercheck(Oid lan_oid, Oid roleid);
extern bool mdb_largeobject_ownercheck(Oid lobj_oid, Oid roleid);
extern bool mdb_namespace_ownercheck(Oid nsp_oid, Oid roleid);
extern bool mdb_tablespace_ownercheck(Oid spc_oid, Oid roleid);
extern bool mdb_opclass_ownercheck(Oid opc_oid, Oid roleid);
extern bool mdb_opfamily_ownercheck(Oid opf_oid, Oid roleid);
extern bool mdb_database_ownercheck(Oid db_oid, Oid roleid);
extern bool mdb_collation_ownercheck(Oid coll_oid, Oid roleid);
extern bool mdb_conversion_ownercheck(Oid conv_oid, Oid roleid);
extern bool mdb_ts_dict_ownercheck(Oid dict_oid, Oid roleid);
extern bool mdb_ts_config_ownercheck(Oid cfg_oid, Oid roleid);
extern bool mdb_foreign_data_wrapper_ownercheck(Oid srv_oid, Oid roleid);
extern bool mdb_foreign_server_ownercheck(Oid srv_oid, Oid roleid);
extern bool mdb_event_trigger_ownercheck(Oid et_oid, Oid roleid);
extern bool mdb_extension_ownercheck(Oid ext_oid, Oid roleid);
extern bool has_createrole_privilege(Oid roleid);
extern bool has_bypassrls_privilege(Oid roleid);

#endif   /* ACL_H */
