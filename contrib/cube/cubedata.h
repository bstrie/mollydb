/* contrib/cube/cubedata.h */

#define CUBE_MAX_DIM (100)

typedef struct NDBOX
{
	/* varlena header (do not touch directly!) */
	int32		vl_len_;

	/*----------
	 * Header contains info about NDBOX. For binary compatibility with old
	 * versions, it is defined as "unsigned int".
	 *
	 * Following information is stored:
	 *
	 *	bits 0-7  : number of cube dimensions;
	 *	bits 8-30 : unused, initialize to zero;
	 *	bit  31   : point flag. If set, the upper right coordinates are not
	 *				stored, and are implicitly the same as the lower left
	 *				coordinates.
	 *----------
	 */
	unsigned int header;

	/*
	 * The lower left coordinates for each dimension come first, followed by
	 * upper right coordinates unless the point flag is set.
	 */
	double		x[FLEXIBLE_ARRAY_MEMBER];
} NDBOX;

#define POINT_BIT			0x80000000
#define DIM_MASK			0x7fffffff

#define IS_POINT(cube)		( ((cube)->header & POINT_BIT) != 0 )
#define SET_POINT_BIT(cube) ( (cube)->header |= POINT_BIT )
#define DIM(cube)			( (cube)->header & DIM_MASK )
#define SET_DIM(cube, _dim) ( (cube)->header = ((cube)->header & ~DIM_MASK) | (_dim) )

#define LL_COORD(cube, i) ( (cube)->x[i] )
#define UR_COORD(cube, i) ( IS_POINT(cube) ? (cube)->x[i] : (cube)->x[(i) + DIM(cube)] )

#define POINT_SIZE(_dim)	(offsetof(NDBOX, x) + sizeof(double)*(_dim))
#define CUBE_SIZE(_dim)		(offsetof(NDBOX, x) + sizeof(double)*(_dim)*2)

#define DatumGetNDBOX(x)	((NDBOX *) PG_DETOAST_DATUM(x))
#define PG_GETARG_NDBOX(x)	DatumGetNDBOX(PG_GETARG_DATUM(x))
#define PG_RETURN_NDBOX(x)	PG_RETURN_POINTER(x)

#define CubeKNNDistanceCoord			15		/* ~> */
#define CubeKNNDistanceTaxicab			16		/* <#> */
#define CubeKNNDistanceEuclid			17		/* <-> */
#define CubeKNNDistanceChebyshev		18		/* <=> */

/* in cubescan.l */
extern int	cube_yylex(void);
extern void cube_yyerror(NDBOX **result, const char *message) mdb_attribute_noreturn();
extern void cube_scanner_init(const char *str);
extern void cube_scanner_finish(void);

/* in cubeparse.y */
extern int	cube_yyparse(NDBOX **result);
