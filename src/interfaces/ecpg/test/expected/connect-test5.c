/* Processed by ecpg (regression mode) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */
#define ECPGdebug(X,Y) ECPGdebug((X)+100,(Y))

#line 1 "test5.pgc"
/*
 * this file tests all sorts of connecting to one single database.
 */

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* do not include regression.h */

int
main(void)
{
/* exec sql begin declare section */
	 
	 
	 

#line 16 "test5.pgc"
 char db [ 200 ] ;
 
#line 17 "test5.pgc"
 char id [ 200 ] ;
 
#line 18 "test5.pgc"
 char * user = "connectuser" ;
/* exec sql end declare section */
#line 19 "test5.pgc"


	ECPGdebug(1, stderr);

	{ ECPGconnect(__LINE__, 0, "connectdb" , NULL, NULL , "main", 0); }
#line 23 "test5.pgc"

	{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "alter user connectdb encrypted password 'insecure'", ECPGt_EOIT, ECPGt_EORT);}
#line 24 "test5.pgc"

	{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "alter user connectuser encrypted password 'connectpw'", ECPGt_EOIT, ECPGt_EORT);}
#line 25 "test5.pgc"

	{ ECPGtrans(__LINE__, NULL, "commit");}
#line 26 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "CURRENT");}
#line 27 "test5.pgc"
  /* <-- "main" not specified */

	strcpy(db, "connectdb");
	strcpy(id, "main");
	{ ECPGconnect(__LINE__, 0, db , NULL, NULL , id, 0); }
#line 31 "test5.pgc"

	{ ECPGdisconnect(__LINE__, id);}
#line 32 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "connectdb" , NULL, NULL , "main", 0); }
#line 34 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 35 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "connectdb" , NULL, NULL , "main", 0); }
#line 37 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 38 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "connectdb" , NULL, NULL , "main", 0); }
#line 40 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 41 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "" , "connectdb" , "insecure" , "main", 0); }
#line 43 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 44 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "connectdb" , "connectuser" , "connectpw" , "main", 0); }
#line 46 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 47 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "unix:mollydb://localhost/connectdb" , "connectuser" , "connectpw" , "main", 0); }
#line 49 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 50 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "unix:mollydb://localhost/connectdb" , "connectuser" , "connectpw" , "main", 0); }
#line 52 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 53 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "unix:mollydb://localhost/connectdb" , user , "connectpw" , "main", 0); }
#line 55 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 56 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "unix:mollydb://localhost/connectdb?connect_timeout=14 & client_encoding=latin1" , "connectuser" , "connectpw" , "main", 0); }
#line 58 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 59 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "unix:mollydb://200.46.204.71/connectdb" , "connectuser" , "connectpw" , "main", 0); }
#line 61 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 62 "test5.pgc"


	{ ECPGconnect(__LINE__, 0, "unix:mollydb://localhost/" , "connectdb" , "insecure" , "main", 0); }
#line 64 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 65 "test5.pgc"


	/* connect twice */
	{ ECPGconnect(__LINE__, 0, "connectdb" , NULL, NULL , "main", 0); }
#line 68 "test5.pgc"

	{ ECPGconnect(__LINE__, 0, "connectdb" , NULL, NULL , "main", 0); }
#line 69 "test5.pgc"

	{ ECPGdisconnect(__LINE__, "main");}
#line 70 "test5.pgc"


	/* not connected */
	{ ECPGdisconnect(__LINE__, "nonexistant");}
#line 73 "test5.pgc"


	return (0);
}
