/*-------------------------------------------------------------------------
 *
 * hba.h
 *	  Interface to hba.c
 *
 *
 * src/include/libpq/hba.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef HBA_H
#define HBA_H

#include "libpq/pqcomm.h"	/* pgrminclude ignore */	/* needed for NetBSD */
#include "nodes/mdb_list.h"
#include "regex/regex.h"


typedef enum UserAuth
{
	uaReject,
	uaImplicitReject,
	uaTrust,
	uaIdent,
	uaPassword,
	uaMD5,
	uaGSS,
	uaSSPI,
	uaPAM,
	uaBSD,
	uaLDAP,
	uaCert,
	uaRADIUS,
	uaPeer
} UserAuth;

typedef enum IPCompareMethod
{
	ipCmpMask,
	ipCmpSameHost,
	ipCmpSameNet,
	ipCmpAll
} IPCompareMethod;

typedef enum ConnType
{
	ctLocal,
	ctHost,
	ctHostSSL,
	ctHostNoSSL
} ConnType;

typedef struct HbaLine
{
	int			linenumber;
	char	   *rawline;
	ConnType	conntype;
	List	   *databases;
	List	   *roles;
	struct sockaddr_storage addr;
	struct sockaddr_storage mask;
	IPCompareMethod ip_cmp_method;
	char	   *hostname;
	UserAuth	auth_method;

	char	   *usermap;
	char	   *pamservice;
	bool		pam_use_hostname;
	bool		ldaptls;
	char	   *ldapserver;
	int			ldapport;
	char	   *ldapbinddn;
	char	   *ldapbindpasswd;
	char	   *ldapsearchattribute;
	char	   *ldapbasedn;
	int			ldapscope;
	char	   *ldapprefix;
	char	   *ldapsuffix;
	bool		clientcert;
	char	   *krb_realm;
	bool		include_realm;
	bool		compat_realm;
	bool		upn_username;
	char	   *radiusserver;
	char	   *radiussecret;
	char	   *radiusidentifier;
	int			radiusport;
} HbaLine;

typedef struct IdentLine
{
	int			linenumber;

	char	   *usermap;
	char	   *ident_user;
	char	   *mdb_role;
	regex_t		re;
} IdentLine;

/* kluge to avoid including libpq/libpq-be.h here */
typedef struct Port hbaPort;

extern bool load_hba(void);
extern bool load_ident(void);
extern void hba_getauthmethod(hbaPort *port);
extern int check_usermap(const char *usermap_name,
			  const char *mdb_role, const char *auth_user,
			  bool case_sensitive);
extern bool mdb_isblank(const char c);

#endif   /* HBA_H */
