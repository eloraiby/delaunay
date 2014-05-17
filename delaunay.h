#ifndef DELAUNAY_H
#define DELAUNAY_H

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

#define DEL_CIRCLE



#ifdef __cplusplus
extern "C" {
#endif

/* define the floating point type, comment to use float - Be careful "float" type will assert more often */
#define USE_DOUBLE

#define FAST_PREDICATE	1	/* fast but floating point errors are more likely to occur */
#define LOOSE_PREDICATE	2	/* loose with epsilon defined in the delaunay file - errors will happen but less frequently */
#define EXACT_PREDICATE	3	/* use exact arithmetic - slower, but shouldn't produce any floating point error */

#define PREDICATE	LOOSE_PREDICATE

#if PREDICATE == EXACT_PREDICATE && !defined(USE_DOUBLE)
#	define USE_DOUBLE
#endif

#ifdef USE_DOUBLE
typedef double real;
#else
typedef float	real;
#endif
int			delaunay2d(real *points, int num_points, int **faces);

#ifdef __cplusplus
}
#endif

#endif // DELAUNAY_H
