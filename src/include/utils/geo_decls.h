/*-------------------------------------------------------------------------
 *
 * geo_decls.h - Declarations for various 2D constructs.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/geo_decls.h
 *
 * NOTE
 *	  These routines do *not* use the float types from adt/.
 *
 *	  XXX These routines were not written by a numerical analyst.
 *
 *	  XXX I have made some attempt to flesh out the operators
 *		and data types. There are still some more to do. - tgl 97/04/19
 *
 *-------------------------------------------------------------------------
 */
#ifndef GEO_DECLS_H
#define GEO_DECLS_H

#include <math.h>

#include "fmgr.h"

/*--------------------------------------------------------------------
 * Useful floating point utilities and constants.
 *-------------------------------------------------------------------*/


#define EPSILON					1.0E-06

#ifdef EPSILON
#define FPzero(A)				(fabs(A) <= EPSILON)
#define FPeq(A,B)				(fabs((A) - (B)) <= EPSILON)
#define FPne(A,B)				(fabs((A) - (B)) > EPSILON)
#define FPlt(A,B)				((B) - (A) > EPSILON)
#define FPle(A,B)				((A) - (B) <= EPSILON)
#define FPgt(A,B)				((A) - (B) > EPSILON)
#define FPge(A,B)				((B) - (A) <= EPSILON)
#else
#define FPzero(A)				((A) == 0)
#define FPeq(A,B)				((A) == (B))
#define FPne(A,B)				((A) != (B))
#define FPlt(A,B)				((A) < (B))
#define FPle(A,B)				((A) <= (B))
#define FPgt(A,B)				((A) > (B))
#define FPge(A,B)				((A) >= (B))
#endif

#define HYPOT(A, B)				mdb_hypot(A, B)

/*---------------------------------------------------------------------
 * Point - (x,y)
 *-------------------------------------------------------------------*/
typedef struct
{
	double		x,
				y;
} Point;


/*---------------------------------------------------------------------
 * LSEG - A straight line, specified by endpoints.
 *-------------------------------------------------------------------*/
typedef struct
{
	Point		p[2];
} LSEG;


/*---------------------------------------------------------------------
 * PATH - Specified by vertex points.
 *-------------------------------------------------------------------*/
typedef struct
{
	int32		vl_len_;		/* varlena header (do not touch directly!) */
	int32		npts;
	int32		closed;			/* is this a closed polygon? */
	int32		dummy;			/* padding to make it double align */
	Point		p[FLEXIBLE_ARRAY_MEMBER];
} PATH;


/*---------------------------------------------------------------------
 * LINE - Specified by its general equation (Ax+By+C=0).
 *-------------------------------------------------------------------*/
typedef struct
{
	double		A,
				B,
				C;
} LINE;


/*---------------------------------------------------------------------
 * BOX	- Specified by two corner points, which are
 *		 sorted to save calculation time later.
 *-------------------------------------------------------------------*/
typedef struct
{
	Point		high,
				low;			/* corner POINTs */
} BOX;

/*---------------------------------------------------------------------
 * POLYGON - Specified by an array of doubles defining the points,
 *		keeping the number of points and the bounding box for
 *		speed purposes.
 *-------------------------------------------------------------------*/
typedef struct
{
	int32		vl_len_;		/* varlena header (do not touch directly!) */
	int32		npts;
	BOX			boundbox;
	Point		p[FLEXIBLE_ARRAY_MEMBER];
} POLYGON;

/*---------------------------------------------------------------------
 * CIRCLE - Specified by a center point and radius.
 *-------------------------------------------------------------------*/
typedef struct
{
	Point		center;
	double		radius;
} CIRCLE;

/*
 * fmgr interface macros
 *
 * Path and Polygon are toastable varlena types, the others are just
 * fixed-size pass-by-reference types.
 */

#define DatumGetPointP(X)	 ((Point *) DatumGetPointer(X))
#define PointPGetDatum(X)	 PointerGetDatum(X)
#define MDB_GETARG_POINT_P(n) DatumGetPointP(MDB_GETARG_DATUM(n))
#define MDB_RETURN_POINT_P(x) return PointPGetDatum(x)

#define DatumGetLsegP(X)	((LSEG *) DatumGetPointer(X))
#define LsegPGetDatum(X)	PointerGetDatum(X)
#define MDB_GETARG_LSEG_P(n) DatumGetLsegP(MDB_GETARG_DATUM(n))
#define MDB_RETURN_LSEG_P(x) return LsegPGetDatum(x)

#define DatumGetPathP(X)		 ((PATH *) MDB_DETOAST_DATUM(X))
#define DatumGetPathPCopy(X)	 ((PATH *) MDB_DETOAST_DATUM_COPY(X))
#define PathPGetDatum(X)		 PointerGetDatum(X)
#define MDB_GETARG_PATH_P(n)		 DatumGetPathP(MDB_GETARG_DATUM(n))
#define MDB_GETARG_PATH_P_COPY(n) DatumGetPathPCopy(MDB_GETARG_DATUM(n))
#define MDB_RETURN_PATH_P(x)		 return PathPGetDatum(x)

#define DatumGetLineP(X)	((LINE *) DatumGetPointer(X))
#define LinePGetDatum(X)	PointerGetDatum(X)
#define MDB_GETARG_LINE_P(n) DatumGetLineP(MDB_GETARG_DATUM(n))
#define MDB_RETURN_LINE_P(x) return LinePGetDatum(x)

#define DatumGetBoxP(X)    ((BOX *) DatumGetPointer(X))
#define BoxPGetDatum(X)    PointerGetDatum(X)
#define MDB_GETARG_BOX_P(n) DatumGetBoxP(MDB_GETARG_DATUM(n))
#define MDB_RETURN_BOX_P(x) return BoxPGetDatum(x)

#define DatumGetPolygonP(X)			((POLYGON *) MDB_DETOAST_DATUM(X))
#define DatumGetPolygonPCopy(X)		((POLYGON *) MDB_DETOAST_DATUM_COPY(X))
#define PolygonPGetDatum(X)			PointerGetDatum(X)
#define MDB_GETARG_POLYGON_P(n)		DatumGetPolygonP(MDB_GETARG_DATUM(n))
#define MDB_GETARG_POLYGON_P_COPY(n) DatumGetPolygonPCopy(MDB_GETARG_DATUM(n))
#define MDB_RETURN_POLYGON_P(x)		return PolygonPGetDatum(x)

#define DatumGetCircleP(X)	  ((CIRCLE *) DatumGetPointer(X))
#define CirclePGetDatum(X)	  PointerGetDatum(X)
#define MDB_GETARG_CIRCLE_P(n) DatumGetCircleP(MDB_GETARG_DATUM(n))
#define MDB_RETURN_CIRCLE_P(x) return CirclePGetDatum(x)


/*
 * in geo_ops.h
 */

/* public point routines */
extern Datum point_in(MDB_FUNCTION_ARGS);
extern Datum point_out(MDB_FUNCTION_ARGS);
extern Datum point_recv(MDB_FUNCTION_ARGS);
extern Datum point_send(MDB_FUNCTION_ARGS);
extern Datum construct_point(MDB_FUNCTION_ARGS);
extern Datum point_left(MDB_FUNCTION_ARGS);
extern Datum point_right(MDB_FUNCTION_ARGS);
extern Datum point_above(MDB_FUNCTION_ARGS);
extern Datum point_below(MDB_FUNCTION_ARGS);
extern Datum point_vert(MDB_FUNCTION_ARGS);
extern Datum point_horiz(MDB_FUNCTION_ARGS);
extern Datum point_eq(MDB_FUNCTION_ARGS);
extern Datum point_ne(MDB_FUNCTION_ARGS);
extern Datum point_distance(MDB_FUNCTION_ARGS);
extern Datum point_slope(MDB_FUNCTION_ARGS);
extern Datum point_add(MDB_FUNCTION_ARGS);
extern Datum point_sub(MDB_FUNCTION_ARGS);
extern Datum point_mul(MDB_FUNCTION_ARGS);
extern Datum point_div(MDB_FUNCTION_ARGS);

/* private routines */
extern double point_dt(Point *pt1, Point *pt2);
extern double point_sl(Point *pt1, Point *pt2);
extern double mdb_hypot(double x, double y);

/* public lseg routines */
extern Datum lseg_in(MDB_FUNCTION_ARGS);
extern Datum lseg_out(MDB_FUNCTION_ARGS);
extern Datum lseg_recv(MDB_FUNCTION_ARGS);
extern Datum lseg_send(MDB_FUNCTION_ARGS);
extern Datum lseg_intersect(MDB_FUNCTION_ARGS);
extern Datum lseg_parallel(MDB_FUNCTION_ARGS);
extern Datum lseg_perp(MDB_FUNCTION_ARGS);
extern Datum lseg_vertical(MDB_FUNCTION_ARGS);
extern Datum lseg_horizontal(MDB_FUNCTION_ARGS);
extern Datum lseg_eq(MDB_FUNCTION_ARGS);
extern Datum lseg_ne(MDB_FUNCTION_ARGS);
extern Datum lseg_lt(MDB_FUNCTION_ARGS);
extern Datum lseg_le(MDB_FUNCTION_ARGS);
extern Datum lseg_gt(MDB_FUNCTION_ARGS);
extern Datum lseg_ge(MDB_FUNCTION_ARGS);
extern Datum lseg_construct(MDB_FUNCTION_ARGS);
extern Datum lseg_length(MDB_FUNCTION_ARGS);
extern Datum lseg_distance(MDB_FUNCTION_ARGS);
extern Datum lseg_center(MDB_FUNCTION_ARGS);
extern Datum lseg_interpt(MDB_FUNCTION_ARGS);
extern Datum dist_pl(MDB_FUNCTION_ARGS);
extern Datum dist_ps(MDB_FUNCTION_ARGS);
extern Datum dist_ppath(MDB_FUNCTION_ARGS);
extern Datum dist_pb(MDB_FUNCTION_ARGS);
extern Datum dist_sl(MDB_FUNCTION_ARGS);
extern Datum dist_sb(MDB_FUNCTION_ARGS);
extern Datum dist_lb(MDB_FUNCTION_ARGS);
extern Datum close_lseg(MDB_FUNCTION_ARGS);
extern Datum close_pl(MDB_FUNCTION_ARGS);
extern Datum close_ps(MDB_FUNCTION_ARGS);
extern Datum close_pb(MDB_FUNCTION_ARGS);
extern Datum close_sl(MDB_FUNCTION_ARGS);
extern Datum close_sb(MDB_FUNCTION_ARGS);
extern Datum close_ls(MDB_FUNCTION_ARGS);
extern Datum close_lb(MDB_FUNCTION_ARGS);
extern Datum on_pl(MDB_FUNCTION_ARGS);
extern Datum on_ps(MDB_FUNCTION_ARGS);
extern Datum on_pb(MDB_FUNCTION_ARGS);
extern Datum on_ppath(MDB_FUNCTION_ARGS);
extern Datum on_sl(MDB_FUNCTION_ARGS);
extern Datum on_sb(MDB_FUNCTION_ARGS);
extern Datum inter_sl(MDB_FUNCTION_ARGS);
extern Datum inter_sb(MDB_FUNCTION_ARGS);
extern Datum inter_lb(MDB_FUNCTION_ARGS);

/* public line routines */
extern Datum line_in(MDB_FUNCTION_ARGS);
extern Datum line_out(MDB_FUNCTION_ARGS);
extern Datum line_recv(MDB_FUNCTION_ARGS);
extern Datum line_send(MDB_FUNCTION_ARGS);
extern Datum line_interpt(MDB_FUNCTION_ARGS);
extern Datum line_distance(MDB_FUNCTION_ARGS);
extern Datum line_construct_pp(MDB_FUNCTION_ARGS);
extern Datum line_intersect(MDB_FUNCTION_ARGS);
extern Datum line_parallel(MDB_FUNCTION_ARGS);
extern Datum line_perp(MDB_FUNCTION_ARGS);
extern Datum line_vertical(MDB_FUNCTION_ARGS);
extern Datum line_horizontal(MDB_FUNCTION_ARGS);
extern Datum line_eq(MDB_FUNCTION_ARGS);

/* public box routines */
extern Datum box_in(MDB_FUNCTION_ARGS);
extern Datum box_out(MDB_FUNCTION_ARGS);
extern Datum box_recv(MDB_FUNCTION_ARGS);
extern Datum box_send(MDB_FUNCTION_ARGS);
extern Datum box_same(MDB_FUNCTION_ARGS);
extern Datum box_overlap(MDB_FUNCTION_ARGS);
extern Datum box_left(MDB_FUNCTION_ARGS);
extern Datum box_overleft(MDB_FUNCTION_ARGS);
extern Datum box_right(MDB_FUNCTION_ARGS);
extern Datum box_overright(MDB_FUNCTION_ARGS);
extern Datum box_below(MDB_FUNCTION_ARGS);
extern Datum box_overbelow(MDB_FUNCTION_ARGS);
extern Datum box_above(MDB_FUNCTION_ARGS);
extern Datum box_overabove(MDB_FUNCTION_ARGS);
extern Datum box_contained(MDB_FUNCTION_ARGS);
extern Datum box_contain(MDB_FUNCTION_ARGS);
extern Datum box_contain_pt(MDB_FUNCTION_ARGS);
extern Datum box_below_eq(MDB_FUNCTION_ARGS);
extern Datum box_above_eq(MDB_FUNCTION_ARGS);
extern Datum box_lt(MDB_FUNCTION_ARGS);
extern Datum box_gt(MDB_FUNCTION_ARGS);
extern Datum box_eq(MDB_FUNCTION_ARGS);
extern Datum box_le(MDB_FUNCTION_ARGS);
extern Datum box_ge(MDB_FUNCTION_ARGS);
extern Datum box_area(MDB_FUNCTION_ARGS);
extern Datum box_width(MDB_FUNCTION_ARGS);
extern Datum box_height(MDB_FUNCTION_ARGS);
extern Datum box_distance(MDB_FUNCTION_ARGS);
extern Datum box_center(MDB_FUNCTION_ARGS);
extern Datum box_intersect(MDB_FUNCTION_ARGS);
extern Datum box_diagonal(MDB_FUNCTION_ARGS);
extern Datum points_box(MDB_FUNCTION_ARGS);
extern Datum box_add(MDB_FUNCTION_ARGS);
extern Datum box_sub(MDB_FUNCTION_ARGS);
extern Datum box_mul(MDB_FUNCTION_ARGS);
extern Datum box_div(MDB_FUNCTION_ARGS);
extern Datum point_box(MDB_FUNCTION_ARGS);
extern Datum boxes_bound_box(MDB_FUNCTION_ARGS);

/* public path routines */
extern Datum path_area(MDB_FUNCTION_ARGS);
extern Datum path_in(MDB_FUNCTION_ARGS);
extern Datum path_out(MDB_FUNCTION_ARGS);
extern Datum path_recv(MDB_FUNCTION_ARGS);
extern Datum path_send(MDB_FUNCTION_ARGS);
extern Datum path_n_lt(MDB_FUNCTION_ARGS);
extern Datum path_n_gt(MDB_FUNCTION_ARGS);
extern Datum path_n_eq(MDB_FUNCTION_ARGS);
extern Datum path_n_le(MDB_FUNCTION_ARGS);
extern Datum path_n_ge(MDB_FUNCTION_ARGS);
extern Datum path_inter(MDB_FUNCTION_ARGS);
extern Datum path_distance(MDB_FUNCTION_ARGS);
extern Datum path_length(MDB_FUNCTION_ARGS);

extern Datum path_isclosed(MDB_FUNCTION_ARGS);
extern Datum path_isopen(MDB_FUNCTION_ARGS);
extern Datum path_npoints(MDB_FUNCTION_ARGS);

extern Datum path_close(MDB_FUNCTION_ARGS);
extern Datum path_open(MDB_FUNCTION_ARGS);
extern Datum path_add(MDB_FUNCTION_ARGS);
extern Datum path_add_pt(MDB_FUNCTION_ARGS);
extern Datum path_sub_pt(MDB_FUNCTION_ARGS);
extern Datum path_mul_pt(MDB_FUNCTION_ARGS);
extern Datum path_div_pt(MDB_FUNCTION_ARGS);

extern Datum path_center(MDB_FUNCTION_ARGS);
extern Datum path_poly(MDB_FUNCTION_ARGS);

/* public polygon routines */
extern Datum poly_in(MDB_FUNCTION_ARGS);
extern Datum poly_out(MDB_FUNCTION_ARGS);
extern Datum poly_recv(MDB_FUNCTION_ARGS);
extern Datum poly_send(MDB_FUNCTION_ARGS);
extern Datum poly_left(MDB_FUNCTION_ARGS);
extern Datum poly_overleft(MDB_FUNCTION_ARGS);
extern Datum poly_right(MDB_FUNCTION_ARGS);
extern Datum poly_overright(MDB_FUNCTION_ARGS);
extern Datum poly_below(MDB_FUNCTION_ARGS);
extern Datum poly_overbelow(MDB_FUNCTION_ARGS);
extern Datum poly_above(MDB_FUNCTION_ARGS);
extern Datum poly_overabove(MDB_FUNCTION_ARGS);
extern Datum poly_same(MDB_FUNCTION_ARGS);
extern Datum poly_overlap(MDB_FUNCTION_ARGS);
extern Datum poly_contain(MDB_FUNCTION_ARGS);
extern Datum poly_contained(MDB_FUNCTION_ARGS);
extern Datum poly_contain_pt(MDB_FUNCTION_ARGS);
extern Datum pt_contained_poly(MDB_FUNCTION_ARGS);
extern Datum poly_distance(MDB_FUNCTION_ARGS);
extern Datum poly_npoints(MDB_FUNCTION_ARGS);
extern Datum poly_center(MDB_FUNCTION_ARGS);
extern Datum poly_box(MDB_FUNCTION_ARGS);
extern Datum poly_path(MDB_FUNCTION_ARGS);
extern Datum box_poly(MDB_FUNCTION_ARGS);

/* public circle routines */
extern Datum circle_in(MDB_FUNCTION_ARGS);
extern Datum circle_out(MDB_FUNCTION_ARGS);
extern Datum circle_recv(MDB_FUNCTION_ARGS);
extern Datum circle_send(MDB_FUNCTION_ARGS);
extern Datum circle_same(MDB_FUNCTION_ARGS);
extern Datum circle_overlap(MDB_FUNCTION_ARGS);
extern Datum circle_overleft(MDB_FUNCTION_ARGS);
extern Datum circle_left(MDB_FUNCTION_ARGS);
extern Datum circle_right(MDB_FUNCTION_ARGS);
extern Datum circle_overright(MDB_FUNCTION_ARGS);
extern Datum circle_contained(MDB_FUNCTION_ARGS);
extern Datum circle_contain(MDB_FUNCTION_ARGS);
extern Datum circle_below(MDB_FUNCTION_ARGS);
extern Datum circle_above(MDB_FUNCTION_ARGS);
extern Datum circle_overbelow(MDB_FUNCTION_ARGS);
extern Datum circle_overabove(MDB_FUNCTION_ARGS);
extern Datum circle_eq(MDB_FUNCTION_ARGS);
extern Datum circle_ne(MDB_FUNCTION_ARGS);
extern Datum circle_lt(MDB_FUNCTION_ARGS);
extern Datum circle_gt(MDB_FUNCTION_ARGS);
extern Datum circle_le(MDB_FUNCTION_ARGS);
extern Datum circle_ge(MDB_FUNCTION_ARGS);
extern Datum circle_contain_pt(MDB_FUNCTION_ARGS);
extern Datum pt_contained_circle(MDB_FUNCTION_ARGS);
extern Datum circle_add_pt(MDB_FUNCTION_ARGS);
extern Datum circle_sub_pt(MDB_FUNCTION_ARGS);
extern Datum circle_mul_pt(MDB_FUNCTION_ARGS);
extern Datum circle_div_pt(MDB_FUNCTION_ARGS);
extern Datum circle_diameter(MDB_FUNCTION_ARGS);
extern Datum circle_radius(MDB_FUNCTION_ARGS);
extern Datum circle_distance(MDB_FUNCTION_ARGS);
extern Datum dist_pc(MDB_FUNCTION_ARGS);
extern Datum dist_cpoint(MDB_FUNCTION_ARGS);
extern Datum dist_cpoly(MDB_FUNCTION_ARGS);
extern Datum dist_ppoly(MDB_FUNCTION_ARGS);
extern Datum dist_polyp(MDB_FUNCTION_ARGS);
extern Datum circle_center(MDB_FUNCTION_ARGS);
extern Datum cr_circle(MDB_FUNCTION_ARGS);
extern Datum box_circle(MDB_FUNCTION_ARGS);
extern Datum circle_box(MDB_FUNCTION_ARGS);
extern Datum poly_circle(MDB_FUNCTION_ARGS);
extern Datum circle_poly(MDB_FUNCTION_ARGS);
extern Datum circle_area(MDB_FUNCTION_ARGS);

/* support routines for the GiST access method (access/gist/gistproc.c) */
extern Datum gist_box_compress(MDB_FUNCTION_ARGS);
extern Datum gist_box_decompress(MDB_FUNCTION_ARGS);
extern Datum gist_box_union(MDB_FUNCTION_ARGS);
extern Datum gist_box_picksplit(MDB_FUNCTION_ARGS);
extern Datum gist_box_consistent(MDB_FUNCTION_ARGS);
extern Datum gist_box_penalty(MDB_FUNCTION_ARGS);
extern Datum gist_box_same(MDB_FUNCTION_ARGS);
extern Datum gist_box_fetch(MDB_FUNCTION_ARGS);
extern Datum gist_poly_compress(MDB_FUNCTION_ARGS);
extern Datum gist_poly_consistent(MDB_FUNCTION_ARGS);
extern Datum gist_poly_distance(MDB_FUNCTION_ARGS);
extern Datum gist_circle_compress(MDB_FUNCTION_ARGS);
extern Datum gist_circle_consistent(MDB_FUNCTION_ARGS);
extern Datum gist_circle_distance(MDB_FUNCTION_ARGS);
extern Datum gist_point_compress(MDB_FUNCTION_ARGS);
extern Datum gist_point_consistent(MDB_FUNCTION_ARGS);
extern Datum gist_point_distance(MDB_FUNCTION_ARGS);
extern Datum gist_point_fetch(MDB_FUNCTION_ARGS);

/* utils/adt/geo_spgist.c */
Datum smdb_box_quad_config(MDB_FUNCTION_ARGS);
Datum smdb_box_quad_choose(MDB_FUNCTION_ARGS);
Datum smdb_box_quad_picksplit(MDB_FUNCTION_ARGS);
Datum smdb_box_quad_inner_consistent(MDB_FUNCTION_ARGS);
Datum smdb_box_quad_leaf_consistent(MDB_FUNCTION_ARGS);

/* geo_selfuncs.c */
extern Datum areasel(MDB_FUNCTION_ARGS);
extern Datum areajoinsel(MDB_FUNCTION_ARGS);
extern Datum positionsel(MDB_FUNCTION_ARGS);
extern Datum positionjoinsel(MDB_FUNCTION_ARGS);
extern Datum contsel(MDB_FUNCTION_ARGS);
extern Datum contjoinsel(MDB_FUNCTION_ARGS);

#endif   /* GEO_DECLS_H */
