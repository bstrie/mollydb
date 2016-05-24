/* src/interfaces/ecpg/include/ecpgerrno.h */

#ifndef _ECMDB_ERRNO_H
#define _ECMDB_ERRNO_H

#include <errno.h>

/* This is a list of all error codes the embedded SQL program can return */
#define ECMDB_NO_ERROR		0
#define ECMDB_NOT_FOUND		100

/* system error codes returned by ecpglib get the correct number,
 * but are made negative
 */
#define ECMDB_OUT_OF_MEMORY	-ENOMEM

/* first we have a set of ecpg messages, they start at 200 */
#define ECMDB_UNSUPPORTED		-200
#define ECMDB_TOO_MANY_ARGUMENTS		-201
#define ECMDB_TOO_FEW_ARGUMENTS		-202
#define ECMDB_TOO_MANY_MATCHES		-203
#define ECMDB_INT_FORMAT			-204
#define ECMDB_UINT_FORMAT		-205
#define ECMDB_FLOAT_FORMAT		-206
#define ECMDB_NUMERIC_FORMAT		-207
#define ECMDB_INTERVAL_FORMAT		-208
#define ECMDB_DATE_FORMAT		-209
#define ECMDB_TIMESTAMP_FORMAT		-210
#define ECMDB_CONVERT_BOOL		-211
#define ECMDB_EMPTY			-212
#define ECMDB_MISSING_INDICATOR		-213
#define ECMDB_NO_ARRAY			-214
#define ECMDB_DATA_NOT_ARRAY		-215
#define ECMDB_ARRAY_INSERT		-216

#define ECMDB_NO_CONN			-220
#define ECMDB_NOT_CONN			-221

#define ECMDB_INVALID_STMT		-230

/* dynamic SQL related */
#define ECMDB_UNKNOWN_DESCRIPTOR		-240
#define ECMDB_INVALID_DESCRIPTOR_INDEX	-241
#define ECMDB_UNKNOWN_DESCRIPTOR_ITEM	-242
#define ECMDB_VAR_NOT_NUMERIC		-243
#define ECMDB_VAR_NOT_CHAR		-244

/* finally the backend error messages, they start at 400 */
#define ECMDB_PGSQL			-400
#define ECMDB_TRANS			-401
#define ECMDB_CONNECT			-402
#define ECMDB_DUPLICATE_KEY		-403
#define ECMDB_SUBSELECT_NOT_ONE		-404

/* for compatibility we define some different error codes for the same error
 * if adding a new one make sure to not double define it */
#define ECMDB_INFORMIX_DUPLICATE_KEY -239
#define ECMDB_INFORMIX_SUBSELECT_NOT_ONE -284

/* backend WARNINGs, starting at 600 */
#define ECMDB_WARNING_UNRECOGNIZED	   -600
 /* WARNING:  (transaction aborted): queries ignored until END */

 /*
  * WARNING:  current transaction is aborted, queries ignored until end of
  * transaction block
  */
#define ECMDB_WARNING_QUERY_IGNORED	   -601
 /* WARNING:  PerformPortalClose: portal "*" not found */
#define ECMDB_WARNING_UNKNOWN_PORTAL    -602
 /* WARNING:  BEGIN: already a transaction in progress */
#define ECMDB_WARNING_IN_TRANSACTION    -603
 /* WARNING:  AbortTransaction and not in in-progress state */
 /* WARNING:  COMMIT: no transaction in progress */
#define ECMDB_WARNING_NO_TRANSACTION    -604
 /* WARNING:  BlankPortalAssignName: portal * already exists */
#define ECMDB_WARNING_PORTAL_EXISTS	   -605

#endif   /* !_ECMDB_ERRNO_H */
