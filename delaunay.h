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

typedef double real;

typedef struct {
	real	x, y;
} del_point2d_t;

typedef struct {
	/** input points count */
	unsigned int	num_points;

	/** the input points */
	del_point2d_t*	points;

	/** number of returned faces */
	unsigned int	num_faces;

	/** the faces are given as a sequence: num verts, verts indices, num verts, verts indices...
	 * the first face is the external face */
	unsigned int*	faces;
} delaunay2d_t;

/*
 * build the 2D Delaunay triangulation given a set of points of at least 3 points
 *
 * @points: point set given as a sequence of tuple x0, y0, x1, y1, ....
 * @num_points: number of given point
 * @preds: the incircle predicate
 * @faces: the triangles given as a sequence: num verts, verts indices, num verts, verts indices
 *		first face is the external face
 * @return: the created topology
 */
delaunay2d_t*			delaunay2d_from(del_point2d_t *points, unsigned int num_points);

/*
 * release a delaunay2d object
 */
void				delaunay2d_release(delaunay2d_t* del);

#ifdef __cplusplus
}
#endif

#endif // DELAUNAY_H
