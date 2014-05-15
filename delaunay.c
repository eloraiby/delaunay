/*
**  delaunay.c : compute 2D delaunay triangulation in the plane.
**  Copyright (C) 2005  Wael El Oraiby <wael.eloraiby@gmail.com>
**
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#define ON_RIGHT	1
#define ON_SEG		0
#define ON_LEFT		-1

#define OUTSIDE		-1
#define	ON_CIRCLE	0
#define INSIDE		1

struct	point2d_s;
struct	face_s;
struct	halfedge_s;
struct	delaunay_s;

#ifdef USE_DOUBLE
typedef double real;
#define REAL_ZERO	0.0
#define REAL_ONE	1.0
#define REAL_TWO	2.0
#define REAL_FOUR	4.0
#else
typedef float real;
#define REAL_ZERO	0.0f
#define REAL_ONE	1.0f
#define REAL_TWO	2.0f
#define REAL_FOUR	4.0f
#endif

typedef struct point2d_s	point2d_t;
typedef struct face_s		face_t;
typedef struct halfedge_s	halfedge_t;
typedef struct delaunay_s	delaunay_t;
typedef real mat3_t[3][3];

struct point2d_s
{
	int			idx;			/* point index in input buffer */
	real			x, y;			/* point coordinates */
	halfedge_t*		he;			/* point halfedge */
};

struct face_s
{
/*	real			radius;
	real			cx, cy;
	point2d_t*		p[3];
*/
	halfedge_t*		he;			/* a pointing half edge */
	int			num_verts;		/* number of vertices on this face */
};

struct halfedge_s
{
	point2d_t*		vertex;			/* vertex */
	halfedge_t*		alpha;			/* alpha */
	halfedge_t*		sigma;			/* sigma */
	halfedge_t*		amgis;			/* sigma^-1 */
	face_t*			face;			/* halfedge face */
};

struct delaunay_s
{
	halfedge_t*		rightmost_he;		/* right most halfedge */
	halfedge_t*		leftmost_he;		/* left most halfedge */
	point2d_t**		points;			/* pointer to points */
	face_t*			faces;			/* faces of delaunay */
	int			num_faces;		/* face count */
	int			start_point;		/* start point index */
	int			end_point;		/* end point index */
};

/*
* 3x3 matrix determinant
*/
static real det3( mat3_t *m )
{
	real		res;

	res		= ((*m)[0][0]) * (((*m)[1][1]) * ((*m)[2][2]) - ((*m)[1][2]) * ((*m)[2][1]));
	res		-= ((*m)[0][1]) * (((*m)[1][0]) * ((*m)[2][2]) - ((*m)[1][2]) * ((*m)[2][0]));
	res		+= ((*m)[0][2]) * (((*m)[1][0]) * ((*m)[2][1]) - ((*m)[1][1]) * ((*m)[2][0]));

	return res;
}

/*
* allocate a point
*/
static point2d_t* point_alloc()
{
	point2d_t*	p;

	p	= (point2d_t*)malloc(sizeof(point2d_t));
	assert( p != NULL );
	memset(p, 0, sizeof(point2d_t));

	return p;
}

/*
* free a point
*/
static void point_free( point2d_t* p )
{
	assert( p != NULL );
	free(p);
}

/*
* allocate a halfedge
*/
static halfedge_t* halfedge_alloc()
{
	halfedge_t*		d;

	d	= (halfedge_t*)malloc(sizeof(halfedge_t));
	assert( d != NULL );
	memset(d, 0, sizeof(halfedge_t));

	return d;
}

/*
* free a halfedge
*/
static void halfedge_free( halfedge_t* d )
{
	assert( d != NULL );
	free(d);
}

/*
* free all delaunay halfedges
*/
void del_free_halfedges( delaunay_t *del )
{
	int			i;
	halfedge_t		*d, *sig;

	/* if there is nothing to do */
	if( del->points == NULL )
		return;

	for( i = 0; i <= (del->end_point - del->start_point); i++ )
	{
		/* free all the halfedges around the point */
		d	= del->points[i]->he;
		if( d != NULL )
		{
			do {
				sig	= d->sigma;
				halfedge_free( d );
				d	= sig;
			} while( d != del->points[i]->he );
			del->points[i]->he	= NULL;
		}
	}
}

/*
 * allocate memory for a face
 */
static face_t* face_alloc()
{
	face_t	*f = (face_t*)malloc(sizeof(face_t));
	assert( f != NULL );
	memset(f, 0, sizeof(face_t));
	return f;
}

/*
 * free a face
 */
static void face_free(face_t *f)
{
	assert( f != NULL );
	free(f);
}

/*
* classify a point relative to a segment
*/
static int classify_point_seg( point2d_t *s, point2d_t *e, point2d_t *pt )
{
	point2d_t		se, spt;
	real		res;

	se.x	= e->x - s->x;
	se.y	= e->y - s->y;

	spt.x	= pt->x - s->x;
	spt.y	= pt->y - s->y;

	res	= (( se.x * spt.y ) - ( se.y * spt.x ));
	if( res < REAL_ZERO )
		return ON_RIGHT;
	else if( res > REAL_ZERO )
		return ON_LEFT;

	return ON_SEG;
}

/*
* classify a point relative to a halfedge, -1 is left, 0 is on, 1 is right
*/
static int del_classify_point( halfedge_t *d, point2d_t *pt )
{
	point2d_t		*s, *e;

	s		= d->vertex;
	e		= d->alpha->vertex;

	return classify_point_seg(s, e, pt);
}

/*
* return the absolute value
*/
static real dabs( real a )
{
	if( a < REAL_ZERO )
		return (-a);
	return a;
}

/*
* compute the circle given 3 points
*/
static void compute_circle( point2d_t *pt0, point2d_t *pt1, point2d_t *pt2, real *cx, real *cy, real *radius )
{
	mat3_t	ma, mbx, mby, mc;
	real	x0y0, x1y1, x2y2;
	real	a, bx, by, c;

	/* calculate x0y0, .... */
	x0y0		= pt0->x * pt0->x + pt0->y * pt0->y;
	x1y1		= pt1->x * pt1->x + pt1->y * pt1->y;
	x2y2		= pt2->x * pt2->x + pt2->y * pt2->y;

	/* setup A matrix */
	ma[0][0]	= pt0->x;
	ma[0][1]	= pt0->y;
	ma[1][0]	= pt1->x;
	ma[1][1]	= pt1->y;
	ma[2][0]	= pt2->x;
	ma[2][1]	= pt2->y;
	ma[0][2]	= ma[1][2] = ma[2][2] = REAL_ONE;

	/* setup Bx matrix */
	mbx[0][0]	= x0y0;
	mbx[1][0]	= x1y1;
	mbx[2][0]	= x2y2;
	mbx[0][1]	= pt0->y;
	mbx[1][1]	= pt1->y;
	mbx[2][1]	= pt2->y;
	mbx[0][2]	= mbx[1][2] = mbx[2][2] = REAL_ONE;

	/* setup By matrix */
	mby[0][0]	= x0y0;
	mby[1][0]	= x1y1;
	mby[2][0]	= x2y2;
	mby[0][1]	= pt0->x;
	mby[1][1]	= pt1->x;
	mby[2][1]	= pt2->x;
	mby[0][2]	= mby[1][2] = mby[2][2] = REAL_ONE;

	/* setup C matrix */
	mc[0][0]	= x0y0;
	mc[1][0]	= x1y1;
	mc[2][0]	= x2y2;
	mc[0][1]	= pt0->x;
	mc[1][1]	= pt1->x;
	mc[2][1]	= pt2->x;
	mc[0][2]	= pt0->y;
	mc[1][2]	= pt1->y;
	mc[2][2]	= pt2->y;

	/* compute a, bx, by and c */
	a	= det3(&ma);
	bx	= det3(&mbx);
	by	= -det3(&mby);
	c	= -det3(&mc);

	*cx	= bx / (REAL_TWO * a);
	*cy	= by / (REAL_TWO * a);
	*radius	= sqrt(bx * bx + by * by - REAL_FOUR * a * c) / (REAL_TWO * dabs(a));
}

/*
* test if a point is inside a circle given by 3 points, 1 if inside, 0 if outside
*/
static int in_circle( point2d_t *pt0, point2d_t *pt1, point2d_t *pt2, point2d_t *p )
{
	mat3_t	ma, mbx, mby, mc;
	real	x0y0, x1y1, x2y2;
	real	a, bx, by, c, res;

	/* calculate x0y0, .... */
	x0y0		= pt0->x * pt0->x + pt0->y * pt0->y;
	x1y1		= pt1->x * pt1->x + pt1->y * pt1->y;
	x2y2		= pt2->x * pt2->x + pt2->y * pt2->y;

	/* setup A matrix */
	ma[0][0]	= pt0->x;
	ma[0][1]	= pt0->y;
	ma[1][0]	= pt1->x;
	ma[1][1]	= pt1->y;
	ma[2][0]	= pt2->x;
	ma[2][1]	= pt2->y;
	ma[0][2]	= ma[1][2] = ma[2][2] = REAL_ONE;

	/* setup Bx matrix */
	mbx[0][0]	= x0y0;
	mbx[1][0]	= x1y1;
	mbx[2][0]	= x2y2;
	mbx[0][1]	= pt0->y;
	mbx[1][1]	= pt1->y;
	mbx[2][1]	= pt2->y;
	mbx[0][2]	= mbx[1][2] = mbx[2][2] = REAL_ONE;

	/* setup By matrix */
	mby[0][0]	= x0y0;
	mby[1][0]	= x1y1;
	mby[2][0]	= x2y2;
	mby[0][1]	= pt0->x;
	mby[1][1]	= pt1->x;
	mby[2][1]	= pt2->x;
	mby[0][2]	= mby[1][2] = mby[2][2] = REAL_ONE;

	/* setup C matrix */
	mc[0][0]	= x0y0;
	mc[1][0]	= x1y1;
	mc[2][0]	= x2y2;
	mc[0][1]	= pt0->x;
	mc[1][1]	= pt1->x;
	mc[2][1]	= pt2->x;
	mc[0][2]	= pt0->y;
	mc[1][2]	= pt1->y;
	mc[2][2]	= pt2->y;

	/* compute a, bx, by and c */
	a	= det3(&ma);
	bx	= det3(&mbx);
	by	= -det3(&mby);
	c	= -det3(&mc);

	res	= a * (p->x * p->x + p->y * p->y ) - bx * p->x - by * p->y + c;


	if( res < REAL_ZERO )
		return INSIDE;
	else if( res > REAL_ZERO )
		return OUTSIDE;

	return ON_CIRCLE;
}

/*
* initialize delaunay segment
*/
static int del_init_seg( delaunay_t *del, int start )
{
	halfedge_t		*d0, *d1;
	point2d_t		*pt0, *pt1;

	/* init delaunay */
	del->start_point	= start;
	del->end_point		= start + 1;

	/* setup pt0 and pt1 */
	pt0			= del->points[start];
	pt1			= del->points[start + 1];

	/* allocate the halfedges and setup them */
	d0	= halfedge_alloc();
	d1	= halfedge_alloc();

	d0->vertex	= pt0;
	d1->vertex	= pt1;

	d0->sigma	= d0->amgis	= d0;
	d1->sigma	= d1->amgis	= d1;

	d0->alpha	= d1;
	d1->alpha	= d0;

	pt0->he	= d0;
	pt1->he	= d1;

	del->rightmost_he	= d1;
	del->leftmost_he	= d0;


	return 0;
}

/*
* initialize delaunay triangle
*/
static int del_init_tri( delaunay_t *del, int start )
{
	halfedge_t		*d0, *d1, *d2, *d3, *d4, *d5;
	point2d_t		*pt0, *pt1, *pt2;

	/* initiate delaunay */
	del->start_point	= start;
	del->end_point		= start + 2;

	/* setup the points */
	pt0					= del->points[start];
	pt1					= del->points[start + 1];
	pt2					= del->points[start + 2];

	/* allocate the 6 halfedges */
	d0	= halfedge_alloc();
	d1	= halfedge_alloc();
	d2	= halfedge_alloc();
	d3	= halfedge_alloc();
	d4	= halfedge_alloc();
	d5	= halfedge_alloc();

	if( classify_point_seg(pt0, pt2, pt1) == ON_LEFT )	/* first case */
	{
		/* set halfedges points */
		d0->vertex	= pt0;
		d1->vertex	= pt2;
		d2->vertex	= pt1;

		d3->vertex	= pt2;
		d4->vertex	= pt1;
		d5->vertex	= pt0;

		/* set points halfedges */
		pt0->he	= d0;
		pt1->he	= d2;
		pt2->he	= d1;

		/* sigma and sigma -1 setup */
		d0->sigma	= d5;
		d0->amgis	= d5;

		d1->sigma	= d3;
		d1->amgis	= d3;

		d2->sigma	= d4;
		d2->amgis	= d4;

		d3->sigma	= d1;
		d3->amgis	= d1;

		d4->sigma	= d2;
		d4->amgis	= d2;

		d5->sigma	= d0;
		d5->amgis	= d0;

		/* set halfedges alpha */
		d0->alpha	= d3;
		d3->alpha	= d0;

		d1->alpha	= d4;
		d4->alpha	= d1;

		d2->alpha	= d5;
		d5->alpha	= d2;

		del->rightmost_he	= d1;
		del->leftmost_he		= d0;

	} else /* 2nd case */
	{
		/* set halfedges points */
		d0->vertex	= pt0;
		d1->vertex	= pt1;
		d2->vertex	= pt2;

		d3->vertex	= pt1;
		d4->vertex	= pt2;
		d5->vertex	= pt0;

		/* set points halfedges */
		pt0->he	= d0;
		pt1->he	= d1;
		pt2->he	= d2;

		/* sigma and sigma -1 setup */
		d0->sigma	= d5;
		d0->amgis	= d5;

		d1->sigma	= d3;
		d1->amgis	= d3;

		d2->sigma	= d4;
		d2->amgis	= d4;

		d3->sigma	= d1;
		d3->amgis	= d1;

		d4->sigma	= d2;
		d4->amgis	= d2;

		d5->sigma	= d0;
		d5->amgis	= d0;

		/* set halfedges alpha */
		d0->alpha	= d3;
		d3->alpha	= d0;

		d1->alpha	= d4;
		d4->alpha	= d1;

		d2->alpha	= d5;
		d5->alpha	= d2;

		del->rightmost_he	= d2;
		del->leftmost_he		= d0;
	}

	/* TODO: remove when finished */
/*	del_show(del);
*/
	return 0;
}

/*
* remove a single halfedge
*/
static void del_remove_single_halfedge( halfedge_t *d )
{
	halfedge_t	*sigma, *amgis, *alpha;

	sigma	= d->sigma;
	amgis	= d->amgis;
	alpha	= d->alpha;

	assert(sigma != NULL);
	assert(amgis != NULL);

	sigma->amgis	= amgis;
	amgis->sigma	= sigma;

	/* check to see if we have already removed alpha */
	if( alpha )
		alpha->alpha	= NULL;

	/* check to see if the vertex points to this halfedge */
	if( d->vertex->he == d )
		d->vertex->he	= sigma;

	/* finally free the halfedge */
	halfedge_free(d);
}

/*
* remove an edge given a halfedge
*/
static void del_remove_halfedge( halfedge_t *d )
{
	halfedge_t	*alpha;

	alpha	= d->alpha;

	del_remove_single_halfedge(d);
	del_remove_single_halfedge(alpha);

}

/*
* pass through all the halfedges on the left side and validate them
*/
static halfedge_t* del_valid_left( halfedge_t* b )
{
	point2d_t		*g, *d, *u, *v;
	halfedge_t		*c, *du, *dg;

	g	= b->vertex;				/* base halfedge point */
	dg	= b;

	d	= b->alpha->vertex;			/* alpha(halfedge) point */
	b	= b->sigma;

	u	= b->alpha->vertex;			/* sigma(alpha(halfedge)) point */
	du	= b->alpha;

	v	= b->sigma->alpha->vertex;	/* alpha(sigma(sigma(halfedge)) point */

	if( classify_point_seg(g, d, u) == ON_LEFT )
	{
		/* 3 points aren't colinear */
		/* as long as the 4 points belong to the same circle, do the cleaning */
		while( v != d && in_circle(g, d, u, v) == INSIDE	)
		{
			c	= b->sigma;
			du	= b->sigma->alpha;
			del_remove_halfedge(b);
			b	= c;
			u	= du->vertex;
			v	= b->sigma->alpha->vertex;
		}
		if( v != d && in_circle(g, d, u, v) == ON_CIRCLE )
		{
			du	= du->amgis;
			del_remove_halfedge(b);
		}
	} else	/* treat the case where the 3 points are colinear */
		du		= dg;

	return du;
}

/*
* pass through all the halfedges on the right side and validate them
*/
static halfedge_t* del_valid_right( halfedge_t *b )
{
	point2d_t		*g, *d, *u, *v;
	halfedge_t		*c, *dd, *du;

	b	= b->alpha;
	d	= b->vertex;
	dd	= b;
	g	= b->alpha->vertex;
	b	= b->amgis;
	u	= b->alpha->vertex;
	du	= b->alpha;

	v	= b->amgis->alpha->vertex;

	if( classify_point_seg(g, d, u) == ON_LEFT )
	{
		while( v != g && in_circle(g, d, u, v) == INSIDE )
		{
			c	= b->amgis;
			du	= c->alpha;
			del_remove_halfedge(b);
			b	= c;
			u	= du->vertex;
			v	= b->amgis->alpha->vertex;
		}
		if( v != g && in_circle(g, d, u, v) == ON_CIRCLE )

		{
			du	= du->sigma;
			del_remove_halfedge(b);
		}
	} else
		du	= dd;

	return du;
}


/*
* validate a link
*/
static halfedge_t* del_valid_link( halfedge_t *b )
{
	point2d_t	*g, *g_p, *d, *d_p;
	halfedge_t	*gd, *dd, *new_gd, *new_dd;
	int		a;

	g	= b->vertex;
	gd	= del_valid_left(b);
	g_p	= gd->vertex;


	d	= b->alpha->vertex;
	dd	= del_valid_right(b);
	d_p	= dd->vertex;

	if( g != g_p && d != d_p )
	{
		a	= in_circle(g, d, g_p, d_p);

		if( a != ON_CIRCLE )
		{
			if( a == INSIDE )
			{
				g_p	= g;
				gd	= b;
			}
			else
			{
				d_p = d;
				dd	= b->alpha;
			}
		}
	}

	/* create the 2 halfedges */
	new_gd	= halfedge_alloc();
	new_dd	= halfedge_alloc();

	/* setup new_gd and new_dd */

	new_gd->vertex	= gd->vertex;
	new_gd->alpha	= new_dd;
	new_gd->amgis	= gd;
	new_gd->sigma	= gd->sigma;
	gd->sigma->amgis	= new_gd;
	gd->sigma		= new_gd;

	new_dd->vertex	= dd->vertex;
	new_dd->alpha	= new_gd;
	new_dd->amgis	= dd->amgis;
	dd->amgis->sigma	= new_dd;
	new_dd->sigma	= dd;
	dd->amgis		= new_dd;

	return new_gd;
}

/*
* find the lower supportant between 2 delaunay
*/
static halfedge_t* del_get_lower_supportant( delaunay_t *left, delaunay_t *right )
{
	point2d_t	*pl, *pr;
	halfedge_t	*right_d, *left_d, *new_ld, *new_rd;
	int		sl, sr;

	left_d	= left->rightmost_he;
	right_d	= right->leftmost_he;

	do {
		pl		= left_d->amgis->alpha->vertex;
		pr		= right_d->alpha->vertex;

		if( (sl = classify_point_seg(left_d->vertex, right_d->vertex, pl)) == ON_RIGHT )
		{
			left_d	= left_d->amgis->alpha;
		}

		if( (sr = classify_point_seg(left_d->vertex, right_d->vertex, pr)) == ON_RIGHT )
		{
			right_d	= right_d->alpha->sigma;
		}

	} while( sl == ON_RIGHT || sr == ON_RIGHT );

	/* create the 2 halfedges */
	new_ld	= halfedge_alloc();
	new_rd	= halfedge_alloc();

	/* setup new_gd and new_dd */
	new_ld->vertex	= left_d->vertex;
	new_ld->alpha	= new_rd;
	new_ld->amgis	= left_d->amgis;
	left_d->amgis->sigma	= new_ld;
	new_ld->sigma	= left_d;
	left_d->amgis	= new_ld;

	new_rd->vertex	= right_d->vertex;
	new_rd->alpha	= new_ld;
	new_rd->amgis	= right_d->amgis;
	right_d->amgis->sigma	= new_rd;
	new_rd->sigma	= right_d;
	right_d->amgis	= new_rd;

	return new_ld;
}

/*
* link the 2 delaunay together
*/
static void del_link( delaunay_t *result, delaunay_t *left, delaunay_t *right )
{
	point2d_t		*u, *v, *ml, *mr;
	halfedge_t		*b;

	assert( left->points == right->points );

	/* save the most right point and the most left point */
	ml		= left->leftmost_he->vertex;
	mr		= right->rightmost_he->vertex;

	b		= del_get_lower_supportant(left, right);

	u		= b->sigma->alpha->vertex;
	v		= b->alpha->amgis->alpha->vertex;

	while( del_classify_point(b, u) == ON_LEFT ||
		del_classify_point(b, v) == ON_LEFT )
	{
		b	= del_valid_link(b);
		u	= b->sigma->alpha->vertex;
		v	= b->alpha->amgis->alpha->vertex;
	}

	right->rightmost_he	= mr->he;
	left->leftmost_he	= ml->he;

	/* TODO: this part is not needed, and can be optimized */
	while( del_classify_point( right->rightmost_he, right->rightmost_he->amgis->alpha->vertex ) == ON_RIGHT )
		right->rightmost_he	= right->rightmost_he->amgis;

	while( del_classify_point( left->leftmost_he, left->leftmost_he->amgis->alpha->vertex ) == ON_RIGHT )
		left->leftmost_he	= left->leftmost_he->amgis;

	result->leftmost_he	= left->leftmost_he;
	result->rightmost_he	= right->rightmost_he;
	result->points			= left->points;
	result->start_point		= left->start_point;
	result->end_point		= right->end_point;
}

/*
* divide and conquer delaunay
*/
void del_divide_and_conquer( delaunay_t *del, int start, int end )
{
	delaunay_t	left, right;
	int			i, n;

	n		= (end - start + 1);

	if( n > 3 )
	{
		i		= (n / 2) + (n & 1);
		left.points		= del->points;
		right.points	= del->points;
		del_divide_and_conquer( &left, start, start + i - 1 );
		del_divide_and_conquer( &right, start + i, end );
		del_link( del, &left, &right );
	} else
		if( n == 3 )
			del_init_tri( del, start );
		else
			if( n == 2 )
				del_init_seg( del, start );
}

static void build_halfedge_face( delaunay_t *del, halfedge_t *d )
{
	halfedge_t	*curr;

	/* test if the halfedge has already a pointing face */
	if( d->face != NULL )
		return;

	del->faces = (face_t*)realloc(del->faces, (del->num_faces + 1) * sizeof(face_t));

	face_t	*f	= &(del->faces[del->num_faces]);
	curr	= d;
	f->he	= d;
	f->num_verts	= 0;
	do {
		curr->face	= f;
		(f->num_verts)++;
		curr	= curr->alpha->amgis;
	} while( curr != d );

	(del->num_faces)++;

/*	if( d->face.radius < 0.0 )
	{
		d->face.p[0]	= d->vertex;
		d->face.p[1]	= d->alpha->vertex;
		d->face.p[2]	= d->alpha->amgis->alpha->vertex;

		if( classify_point_seg( d->face.p[0], d->face.p[1], d->face.p[2] ) == ON_LEFT )
		{
			compute_circle(d->face.p[0], d->face.p[1], d->face.p[2], &(d->face.cx), &(d->face.cy), &(d->face.radius));
		}
	}
*/
}

/*
* build the faces for all the halfedge
*/
void del_build_faces( delaunay_t *del )
{
	int		i;
	halfedge_t	*curr;

	del->num_faces	= 0;
	del->faces		= NULL;

	/* build external face first */
	build_halfedge_face(del, del->rightmost_he->alpha);

	for( i = del->start_point; i <= del->end_point; i++ )
	{
		curr	= del->points[i]->he;

		do {
			build_halfedge_face( del, curr );
			curr	= curr->sigma;
		} while( curr != del->points[i]->he );
	}
}

/*
* compare 2 points when sorting
*/
static int cmp_points( const void *_pt0, const void *_pt1 )
{
	point2d_t		*pt0, *pt1;

	pt0	= (point2d_t*)(*((point2d_t**)_pt0));
	pt1	= (point2d_t*)(*((point2d_t**)_pt1));

	if( pt0->x < pt1->x )
		return -1;
	else if( pt0->x > pt1->x )
		return 1;
	else if( pt0->y < pt1->y )
		return -1;
	else if( pt0->y > pt1->y )
		return 1;
	return 0;
}

/*
 * build the 2D Delaunay triangulation given a set of points of at least 3 points
 *
 * @points: point set given as a sequence of tuple x0, y0, x1, y1, ....
 * @num_points: number of given point
 * @faces: the triangles given as a sequence: num verts, verts indices, num verts, verts indices
 *		first face is the external face
 * @return: the number of created faces
 */
int delaunay2d(real *points, int num_points, int **faces)
{
	delaunay_t	del;
	int			i, j, fbuff_size = 0;

	/* allocate the points */
	del.points	= (point2d_t**)malloc(num_points * sizeof(point2d_t*));
	assert( del.points != NULL );
	memset(del.points, 0, num_points * sizeof(point2d_t*));
	
	/* copy the points */
	for( i = 0; i < num_points; i++ )
	{
		del.points[i]		= point_alloc();
		del.points[i]->idx	= i;
		del.points[i]->x	= points[i * 2];
		del.points[i]->y	= points[i * 2 + 1];
	}

	qsort(del.points, num_points, sizeof(point2d_t*), cmp_points);

	if( num_points >= 3 )
	{
		del_divide_and_conquer( &del, 0, num_points - 1 );

		del_build_faces( &del );

		fbuff_size	= 0;
		for( i = 0; i < del.num_faces; i++ )
			fbuff_size	+= del.faces[i].num_verts + 1;

		(*faces) = (int*)malloc(sizeof(int) * fbuff_size);

		j = 0;
		for( i = 0; i < del.num_faces; i++ )
		{
			halfedge_t	*curr;

			(*faces)[j]	= del.faces[i].num_verts;
			j++;

			curr	= del.faces[i].he;
			do {
				(*faces)[j]	= curr->vertex->idx;
				j++;
				curr	= curr->alpha->amgis;
			} while( curr != del.faces[i].he );
		}

		del_free_halfedges( &del );

		free(del.faces);
		for( i = 0; i < num_points; i++ )
			point_free(del.points[i]);

		free(del.points);
	}

	return del.num_faces;
}
