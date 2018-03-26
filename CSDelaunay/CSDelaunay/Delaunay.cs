/*
**  JavaDelaunay
**  Copyright (C) 2018 David V. Makray, Wael El Oraiby <wael.eloraiby@gmail.com>
**
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU Affero General Public License as
**  published by the Free Software Foundation, either version 3 of the
**  License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Diagnostics;

namespace Delaunay {

public class Delaunay {

	const int ON_RIGHT = 1;
	const int ON_SEG = 0;
	const int ON_LEFT = -1;

	const int OUTSIDE = -1;
	const int ON_CIRCLE = 0;
	const int INSIDE = 1;

	const double REAL_ZERO = 0.0;
	const double REAL_ONE = 1.0;
	const double REAL_TWO = 2.0;
	const double REAL_FOUR = 4.0;

	tri_delaunay2d triangles = null;
	int triangle2d_delaunay_position = 0;

	private class mat3 {
		public double[,] matrix = new double[3,3];
	}

	private class Point2d : IComparable<Point2d> {
		public double x, y;		/* point coordinates */
		public Halfedge he;		/* point halfedge */
		public int idx;			/* point index in input buffer */

		public int CompareTo(Point2d that)
		{
			if (that == null)
				throw new Exception("Can't compare Point2d against a null.");

			if (this.x < that.x)
				return -1;
			if (this.x > that.x)
				return 1;

			if (this.y < that.y)
				return -1;
			if (this.y > that.y)
				return 1;

			return 0;
		}
	}

	private class Face {
		public Halfedge he;		/* a pointing half edge */
		public int num_verts;		/* number of vertices on this face */
	}

	private class Halfedge {
		public Point2d vertex;	/* vertex */
		public Halfedge pair;		/* pair */
		public Halfedge next;		/* next */
		public Halfedge prev;		/* next^-1 */
		public Face face;		/* halfedge face */
	}

	private class DelaunaySegment {
		public Halfedge rightmost_he;	/* right most halfedge */
		public Halfedge leftmost_he;	/* left most halfedge */
		public Point2d[] points;		/* pointer to points */
		public Face[] faces;			/* faces of delaunay */
		public int num_faces;			/* face count */
		public int start_point;		/* start point index */
		public int end_point;			/* end point index */
	}

	/*
	 * 3x3 matrix determinant
	 */
	private double det3(mat3 m) {
		double res = m.matrix[0,0] * (m.matrix[1,1] * m.matrix[2,2] - m.matrix[1,2] * m.matrix[2,1])
			   - m.matrix[0,1] * (m.matrix[1,0] * m.matrix[2,2] - m.matrix[1,2] * m.matrix[2,0])
			   + m.matrix[0,2] * (m.matrix[1,0] * m.matrix[2,1] - m.matrix[1,1] * m.matrix[2,0]);

		return res;
	}

	private class Delaunay2d {
		/**
		 * input points count
		 */
		public int num_points;

		/**
		 * the input points
		 */
		public Del_Point2d[] points;

		/**
		 * number of returned faces
		 */
		public int num_faces;

		/**
		 * the faces are given as a sequence: num verts, verts indices, num
		 * verts, verts indices... the first face is the external face
		 */
		public int[] faces;
	}

	private class tri_delaunay2d {

		/**
		 * input points count
		 */
		public int num_points;

		/**
		 * input points
		 */
		public Del_Point2d[] points;

		/**
		 * number of triangles
		 */
		public int num_triangles;

		/**
		 * the triangles indices v0,v1,v2, v0,v1,v2 ....
		 */
		public int[] tris;
	}
	
	private class Del_Point2d : IComparable<Del_Point2d>, ICloneable {
		public double x, y;

		public object Clone() {
			Del_Point2d result = new Del_Point2d();
			result.x = x;
			result.y = y;
			return result;
		}

		public int CompareTo(Del_Point2d that)
		{
			if (that == null)
				throw new Exception("Can't compare Point2d against a null.");

			if (this.x < that.x)
				return -1;
			if (this.x > that.x)
				return 1;

			if (this.y < that.y)
				return -1;
			if (this.y > that.y)
				return 1;

			return 0;
		}
	}

	public Delaunay(List<Point> points) {
		Point[] point_array = new Point[points.Count];
		point_array = points.ToArray();
		DelaunaySetup(point_array);
	}

	public Delaunay(Point[] points) {
		DelaunaySetup(points);
	}

	private void DelaunaySetup(Point[] points) {
		triangles = null;
		triangle2d_delaunay_position = 0;
		Del_Point2d[] del_points = new Del_Point2d[points.Length];

		for (int loop = 0; loop < points.Length; loop++) {
			del_points[loop] = new Del_Point2d();
			Point pt = points[loop];

			del_points[loop].x = (double) (pt.X);
			del_points[loop].y = (double) (pt.Y);
		}

		//Sort points and drop matching coordinates
		Array.Sort(del_points);
		int num_points = points.Length;
		int data_shift = 0;
		int deduplicate_loop;
		for (deduplicate_loop = 0; deduplicate_loop < num_points - 1 - data_shift; deduplicate_loop++) {
			if (data_shift > 0)
				del_points[deduplicate_loop] = del_points[deduplicate_loop + data_shift];

			//Use while loop to check for multiple point copies.
			while (deduplicate_loop < num_points - 1 - data_shift) {
				if (del_points[deduplicate_loop].CompareTo(del_points[deduplicate_loop + 1 + data_shift]) == 0)
					data_shift++;
				else
					break;
			}
			if (deduplicate_loop >= num_points - 1 - data_shift)	//Preserve variable
				break;
		}

		if (data_shift > 0) {
			del_points[deduplicate_loop] = del_points[deduplicate_loop + data_shift];
			del_points[deduplicate_loop + 1] = null;
		}
		num_points -= data_shift;

		Delaunay2d delaunay2d = delaunay2d_from(del_points, num_points);
		if (num_points >= 3) {
			triangles = tri_delaunay2d_from(delaunay2d);
		}
	}

	/*
	 * classify a point relative to a segment
	 */
	private int classify_point_seg(Point2d s, Point2d e, Point2d pt) {
		double se_x, se_y, spt_x, spt_y;
		double res;

		se_x = (double) (e.x - s.x);
		se_y = (double) (e.y - s.y);

		spt_x = (double) (pt.x - s.x);
		spt_y = (double) (pt.y - s.y);

		res = ((se_x * spt_y) - (se_y * spt_x));
		if (res < REAL_ZERO) {
			return ON_RIGHT;
		} else if (res > REAL_ZERO) {
			return ON_LEFT;
		}

		return ON_SEG;
	}

	/*
	 * classify a point relative to a halfedge, -1 is left, 0 is on, 1 is right
	 */
	private int del_classify_point(Halfedge d, Point2d pt) {
		Point2d s;
		Point2d e;

		s = d.vertex;
		e = d.pair.vertex;

		return classify_point_seg(s, e, pt);
	}

	/*
	 * test if a point is inside a circle given by 3 points, 1 if inside, 0 if outside
	 */
	private int in_circle(Point2d pt0, Point2d pt1, Point2d pt2, Point2d p) {
		// reduce the computational complexity by substracting the last row of the matrix
		// ref: https://www.cs.cmu.edu/~quake/robust.html
		double p0p_x, p0p_y, p1p_x, p1p_y, p2p_x, p2p_y, p0p, p1p, p2p, res;
		mat3 m = new mat3();

		p0p_x = pt0.x - p.x;
		p0p_y = pt0.y - p.y;

		p1p_x = pt1.x - p.x;
		p1p_y = pt1.y - p.y;

		p2p_x = pt2.x - p.x;
		p2p_y = pt2.y - p.y;

		p0p = p0p_x * p0p_x + p0p_y * p0p_y;
		p1p = p1p_x * p1p_x + p1p_y * p1p_y;
		p2p = p2p_x * p2p_x + p2p_y * p2p_y;

		m.matrix[0,0] = p0p_x;
		m.matrix[0,1] = p0p_y;
		m.matrix[0,2] = p0p;

		m.matrix[1,0] = p1p_x;
		m.matrix[1,1] = p1p_y;
		m.matrix[1,2] = p1p;

		m.matrix[2,0] = p2p_x;
		m.matrix[2,1] = p2p_y;
		m.matrix[2,2] = p2p;

		res = -det3(m);

		if (res < REAL_ZERO) {
			return INSIDE;
		} else if (res > REAL_ZERO) {
			return OUTSIDE;
		}

		return ON_CIRCLE;
	}

	/*
	 * initialize delaunay segment
	 */
	private int del_init_seg(DelaunaySegment del, int start) {
		Halfedge d0;
		Halfedge d1;
		Point2d pt0;
		Point2d pt1;

		/* init delaunay */
		del.start_point = start;
		del.end_point = start + 1;

		/* setup pt0 and pt1 */
		pt0 = del.points[start];
		pt1 = del.points[start + 1];

		/* allocate the halfedges and setup them */
		d0 = new Halfedge();
		d1 = new Halfedge();

		d0.vertex = pt0;
		d1.vertex = pt1;

		d0.next = d0.prev = d0;
		d1.next = d1.prev = d1;

		d0.pair = d1;
		d1.pair = d0;

		pt0.he = d0;
		pt1.he = d1;

		del.rightmost_he = d1;
		del.leftmost_he = d0;

		return 0;
	}

	/*
	 * initialize delaunay triangle
	 */
	private int del_init_tri(DelaunaySegment del, int start) {
		Halfedge d0;
		Halfedge d1;
		Halfedge d2, d3, d4, d5;
		Point2d pt0;
		Point2d pt1, pt2;

		/* initiate delaunay */
		del.start_point = start;
		del.end_point = start + 2;

		/* setup the points */
		pt0 = del.points[start];
		pt1 = del.points[start + 1];
		pt2 = del.points[start + 2];

		/* allocate the 6 halfedges */
		d0 = new Halfedge();
		d1 = new Halfedge();
		d2 = new Halfedge();
		d3 = new Halfedge();
		d4 = new Halfedge();
		d5 = new Halfedge();

		if (classify_point_seg(pt0, pt2, pt1) == ON_LEFT) /* first case */ {
			/* set halfedges points */
			d0.vertex = pt0;
			d1.vertex = pt2;
			d2.vertex = pt1;

			d3.vertex = pt2;
			d4.vertex = pt1;
			d5.vertex = pt0;

			/* set points halfedges */
			pt0.he = d0;
			pt1.he = d2;
			pt2.he = d1;

			/* next and next -1 setup */
			d0.next = d5;
			d0.prev = d5;

			d1.next = d3;
			d1.prev = d3;

			d2.next = d4;
			d2.prev = d4;

			d3.next = d1;
			d3.prev = d1;

			d4.next = d2;
			d4.prev = d2;

			d5.next = d0;
			d5.prev = d0;

			/* set halfedges pair */
			d0.pair = d3;
			d3.pair = d0;

			d1.pair = d4;
			d4.pair = d1;

			d2.pair = d5;
			d5.pair = d2;

			del.rightmost_he = d1;
			del.leftmost_he = d0;

		} else /* 2nd case */ {
			/* set halfedges points */
			d0.vertex = pt0;
			d1.vertex = pt1;
			d2.vertex = pt2;

			d3.vertex = pt1;
			d4.vertex = pt2;
			d5.vertex = pt0;

			/* set points halfedges */
			pt0.he = d0;
			pt1.he = d1;
			pt2.he = d2;

			/* next and next -1 setup */
			d0.next = d5;
			d0.prev = d5;

			d1.next = d3;
			d1.prev = d3;

			d2.next = d4;
			d2.prev = d4;

			d3.next = d1;
			d3.prev = d1;

			d4.next = d2;
			d4.prev = d2;

			d5.next = d0;
			d5.prev = d0;

			/* set halfedges pair */
			d0.pair = d3;
			d3.pair = d0;

			d1.pair = d4;
			d4.pair = d1;

			d2.pair = d5;
			d5.pair = d2;

			del.rightmost_he = d2;
			del.leftmost_he = d0;
		}
		return 0;
	}

	/*
	 * remove an edge given a halfedge
	 */
	private void del_remove_edge(Halfedge d) {
		Halfedge next;
		Halfedge prev;
		Halfedge pair, orig_pair;

		orig_pair = d.pair;

		next = d.next;
		prev = d.prev;
		pair = d.pair;

		Debug.Assert (next != null);
		Debug.Assert (prev != null);

		next.prev = prev;
		prev.next = next;


		/* check to see if we have already removed pair */
		if (pair != null) {
			pair.pair = null;
		}

		/* check to see if the vertex points to this halfedge */
		if (d.vertex.he == d) {
			d.vertex.he = next;
		}

		d.vertex = null;
		d.next = null;
		d.prev = null;
		d.pair = null;

		next = orig_pair.next;
		prev = orig_pair.prev;
		pair = orig_pair.pair;

		Debug.Assert (next != null);
		Debug.Assert (prev != null);

		next.prev = prev;
		prev.next = next;


		/* check to see if we have already removed pair */
		if (pair != null) {
			pair.pair = null;
		}

		/* check to see if the vertex points to this halfedge */
		if (orig_pair.vertex.he == orig_pair) {
			orig_pair.vertex.he = next;
		}

		orig_pair.vertex = null;
		orig_pair.next = null;
		orig_pair.prev = null;
		orig_pair.pair = null;
	}

	/*
	 * pass through all the halfedges on the left side and validate them
	 */
	private Halfedge del_valid_left(Halfedge b) {
		Point2d g;
		Point2d d, u, v;
		Halfedge c;
		Halfedge du;
		Halfedge dg;

		g = b.vertex;			/* base halfedge point */
		dg = b;
		
		d = b.pair.vertex;		/* pair(halfedge) point */
		b = b.next;

		u = b.pair.vertex;		/* next(pair(halfedge)) point */
		du = b.pair;

		v = b.next.pair.vertex;	/* pair(next(next(halfedge)) point */

		if (classify_point_seg(g, d, u) == ON_LEFT) {
			/* 3 points aren't colinear */
			/* as long as the 4 points belong to the same circle, do the cleaning */
			Debug.Assert (v != u); //1: floating point precision error
			while (v != d && v != g && in_circle(g, d, u, v) == INSIDE) {
				c = b.next;
				du = b.next.pair;
				del_remove_edge(b);
				b = c;
				u = du.vertex;
				v = b.next.pair.vertex;
			}

			Debug.Assert (v != u); //2: floating point precision error
			if (v != d && v != g && in_circle(g, d, u, v) == ON_CIRCLE) {
				du = du.prev;
				del_remove_edge(b);
			}
		} else /* treat the case where the 3 points are colinear */ {
			du = dg;
		}

		Debug.Assert (du.pair != null);
		return du;
	}

	/*
	 * pass through all the halfedges on the right side and validate them
	 */
	private Halfedge del_valid_right(Halfedge b) {
		Point2d rv;
		Point2d lv, u, v;
		Halfedge c;
		Halfedge dd;
		Halfedge du;

		b = b.pair;
		rv = b.vertex;
		dd = b;
		lv = b.pair.vertex;
		b = b.prev;
		u = b.pair.vertex;
		du = b.pair;

		v = b.prev.pair.vertex;

		if (classify_point_seg(lv, rv, u) == ON_LEFT) {
			Debug.Assert (v != u); //1: floating point precision error
			while (v != lv && v != rv && in_circle(lv, rv, u, v) == INSIDE) {
				c = b.prev;
				du = c.pair;
				del_remove_edge(b);
				b = c;
				u = du.vertex;
				v = b.prev.pair.vertex;
			}

			Debug.Assert (v != u); //1: floating point precision error
			if (v != lv && v != rv && in_circle(lv, rv, u, v) == ON_CIRCLE) {
				du = du.next;
				del_remove_edge(b);
			}
		} else {
			du = dd;
		}

		Debug.Assert (du.pair != null);
		return du;
	}

	/*
	 * validate a link
	 */
	private Halfedge del_valid_link(Halfedge b) {
		Point2d g;
		Point2d g_p, d, d_p;
		Halfedge gd;
		Halfedge dd;
		Halfedge new_gd, new_dd;
		int a;

		g = b.vertex;
		gd = del_valid_left(b);
		g_p = gd.vertex;

		Debug.Assert (b.pair != null);
		d = b.pair.vertex;
		dd = del_valid_right(b);
		d_p = dd.vertex;
		Debug.Assert (b.pair != null);

		if (g != g_p && d != d_p) {
			a = in_circle(g, d, g_p, d_p);

			if (a != ON_CIRCLE) {
				if (a == INSIDE) {
					gd = b;
				} else {
					dd = b.pair;
				}
			}
		}

		/* create the 2 halfedges */
		new_gd = new Halfedge();
		new_dd = new Halfedge();

		/* setup new_gd and new_dd */
		new_gd.vertex = gd.vertex;
		new_gd.pair = new_dd;
		new_gd.prev = gd;
		new_gd.next = gd.next;
		gd.next.prev = new_gd;
		gd.next = new_gd;

		new_dd.vertex = dd.vertex;
		new_dd.pair = new_gd;
		new_dd.prev = dd.prev;
		dd.prev.next = new_dd;
		new_dd.next = dd;
		dd.prev = new_dd;

		return new_gd;
	}

	/*
     * find the lower tangent between the two delaunay, going from left to right (returns the left half edge)
	 */
	private Halfedge del_get_lower_tangent(DelaunaySegment left, DelaunaySegment right) {
		Point2d pl;
		Point2d pr;
		Halfedge right_d;
		Halfedge left_d;
		Halfedge new_ld, new_rd;
		int sl, sr;

		left_d = left.rightmost_he;
		right_d = right.leftmost_he;

		do {
			pl = left_d.prev.pair.vertex;
			pr = right_d.pair.vertex;

			if ((sl = classify_point_seg(left_d.vertex, right_d.vertex, pl)) == ON_RIGHT) {
				left_d = left_d.prev.pair;
			}

			if ((sr = classify_point_seg(left_d.vertex, right_d.vertex, pr)) == ON_RIGHT) {
				right_d = right_d.pair.next;
			}

		} while (sl == ON_RIGHT || sr == ON_RIGHT);

		/* create the 2 halfedges */
		new_ld = new Halfedge();
		new_rd = new Halfedge();

		/* setup new_gd and new_dd */
		new_ld.vertex = left_d.vertex;
		new_ld.pair = new_rd;
		new_ld.prev = left_d.prev;
		left_d.prev.next = new_ld;
		new_ld.next = left_d;
		left_d.prev = new_ld;

		new_rd.vertex = right_d.vertex;
		new_rd.pair = new_ld;
		new_rd.prev = right_d.prev;
		right_d.prev.next = new_rd;
		new_rd.next = right_d;
		right_d.prev = new_rd;

		return new_ld;
	}

	/*
	 * link the 2 delaunay together
	 */
	private void del_link(DelaunaySegment result, DelaunaySegment left, DelaunaySegment right) {
		Point2d u;
		Point2d v, ml, mr;
		Halfedge basis;

		Debug.Assert (left.points == right.points);

		/* save the most right point and the most left point */
		ml = left.leftmost_he.vertex;
		mr = right.rightmost_he.vertex;

		basis = del_get_lower_tangent(left, right);

		u = basis.next.pair.vertex;
		v = basis.pair.prev.pair.vertex;

		while (del_classify_point(basis, u) == ON_LEFT
			   || del_classify_point(basis, v) == ON_LEFT) {
			basis = del_valid_link(basis);
			u = basis.next.pair.vertex;
			v = basis.pair.prev.pair.vertex;
		}

		right.rightmost_he = mr.he;
		left.leftmost_he = ml.he;

		/* TODO: this part is not needed, and can be optimized */
		while (del_classify_point(right.rightmost_he, right.rightmost_he.prev.pair.vertex) == ON_RIGHT) {
			right.rightmost_he = right.rightmost_he.prev;
		}

		while (del_classify_point(left.leftmost_he, left.leftmost_he.prev.pair.vertex) == ON_RIGHT) {
			left.leftmost_he = left.leftmost_he.prev;
		}

		result.leftmost_he = left.leftmost_he;
		result.rightmost_he = right.rightmost_he;
		result.points = left.points;
		result.start_point = left.start_point;
		result.end_point = right.end_point;
	}

	/*
	 * divide and conquer delaunay
	 */
	private void del_divide_and_conquer(DelaunaySegment del, int start, int end) {
		DelaunaySegment left = new DelaunaySegment();
		DelaunaySegment right = new DelaunaySegment();
		int i, n;

		n = (end - start + 1);

		if (n > 3) {
			i = (n / 2) + (n & 1);
			left.points = del.points;
			right.points = del.points;
			del_divide_and_conquer(left, start, start + i - 1);
			del_divide_and_conquer(right, start + i, end);
			del_link(del, left, right);
		} else if (n == 3) {
			del_init_tri(del, start);
		} else if (n == 2) {
			del_init_seg(del, start);
		}
	}

	private void build_halfedge_face(DelaunaySegment del, Halfedge d) {
		Halfedge curr;

		/* test if the halfedge has already a pointing face */
		if (d.face != null) {
			return;
		}

		/* TODO: optimize this */
		/* copy the points */
		Face[] new_faces = new Face[del.num_faces + 1];
		Debug.Assert (null != new_faces && (del.num_faces == 0 || null != del.faces));
		for (int i = 0; i < del.num_faces; i++) {
			new_faces[i] = del.faces[i];
		}

		new_faces[del.num_faces] = new Face();
		del.faces = new_faces;

		Face f = del.faces[del.num_faces];
		curr = d;
		f.he = d;
		f.num_verts = 0;
		do {
			curr.face = f;
			(f.num_verts)++;
			curr = curr.pair.prev;
		} while (curr != d);

		(del.num_faces)++;
	}

	/*
	 * build the faces for all the halfedge
	 */
	private void del_build_faces(DelaunaySegment del) {
		int i;
		Halfedge curr;

		del.num_faces = 0;
		del.faces = null;

		/* build external face first */
		build_halfedge_face(del, del.rightmost_he.pair);

		for (i = del.start_point; i <= del.end_point; i++) {
			curr = del.points[i].he;

			do {
				build_halfedge_face(del, curr);
				curr = curr.next;
			} while (curr != del.points[i].he);
		}
	}

	/*
	 */
	private Delaunay2d delaunay2d_from(Del_Point2d[] points, int num_points) {
		Delaunay2d res = null;
		DelaunaySegment del = new DelaunaySegment();
		int i, j, fbuff_size = 0;
		int[] faces = null;

		/* allocate the points */
		del.points = new Point2d[num_points];
		Debug.Assert (null != del.points);

		/* copy the points */
		for (i = 0; i < num_points; i++) {
			del.points[i] = new Point2d();
			del.points[i].idx = i;
			del.points[i].x = points[i].x;
			del.points[i].y = points[i].y;
		}

		Array.Sort(del.points);
		if (num_points >= 3) {
			del_divide_and_conquer(del, 0, num_points - 1);

			del_build_faces(del);

			fbuff_size = 0;
			for (i = 0; i < del.num_faces; i++) {
				fbuff_size += del.faces[i].num_verts + 1;
			}

			faces = new int[fbuff_size];
			Debug.Assert (null != faces);

			j = 0;
			for (i = 0; i < del.num_faces; i++) {
				Halfedge curr;

				faces[j] = del.faces[i].num_verts;
				j++;

				curr = del.faces[i].he;
				do {
					faces[j] = curr.vertex.idx;
					j++;
					curr = curr.pair.prev;
				} while (curr != del.faces[i].he);
			}
			del.faces = null;
			del.points = null;
		}

		res = new Delaunay2d();
		Debug.Assert (null != res);

		res.num_points = num_points;
		res.points = new Del_Point2d[num_points];
		Debug.Assert (null != res.points);

		for (int loop = 0; loop < num_points; loop++) {
			res.points[loop] = (Del_Point2d) points[loop].Clone();
		}

		res.num_faces = del.num_faces;
		res.faces = faces;

		return res;
	}

	private tri_delaunay2d tri_delaunay2d_from(Delaunay2d del) {
		int v_offset = del.faces[0] + 1;	/* ignore external face */
		int dst_offset = 0;
		int i;

		tri_delaunay2d tdel = new tri_delaunay2d();
		tdel.num_triangles = 0;

		/* count the number of triangles */
		if (1 == del.num_faces) {
			/* degenerate case: only external face exists */
			int nv = del.faces[0];
			tdel.num_triangles += nv - 2;
		} else {
			for (i = 1; i < del.num_faces; ++i) {
				int nv = del.faces[v_offset];
				tdel.num_triangles += nv - 2;
				v_offset += nv + 1;
			}
		}

		/* copy points */
		tdel.num_points = del.num_points;
		tdel.points = new Del_Point2d[del.num_points];
		Debug.Assert (null != tdel.points);

		for (int loop = 0; loop < del.num_points; loop++) {
			tdel.points[loop] = (Del_Point2d) del.points[loop].Clone();
		}

		/* build the triangles */
		tdel.tris = new int[3 * tdel.num_triangles];
		Debug.Assert (null != tdel.tris);

		v_offset = del.faces[0] + 1;		/* ignore external face */

		if (1 == del.num_faces) {
			/* handle the degenerated case where only the external face exists */
			int nv = del.faces[0];
			int j = 0;
			v_offset = 1;
			for (; j < nv - 2; ++j) {
				tdel.tris[dst_offset] = del.faces[v_offset + j];
				tdel.tris[dst_offset + 1] = del.faces[(v_offset + j + 1) % nv];
				tdel.tris[dst_offset + 2] = del.faces[v_offset + j];
				dst_offset += 3;
			}
		} else {
			for (i = 1; i < del.num_faces; ++i) {
				int nv = del.faces[v_offset];
				int j = 0;
				int first = del.faces[v_offset + 1];

				for (; j < nv - 2; ++j) {
					tdel.tris[dst_offset] = first;
					tdel.tris[dst_offset + 1] = del.faces[v_offset + j + 2];
					tdel.tris[dst_offset + 2] = del.faces[v_offset + j + 3];
					dst_offset += 3;
				}

				v_offset += nv + 1;
			}
		}
		return tdel;
	}

	public bool ready() {
		if (triangles == null)
			return false;

		if (triangle2d_delaunay_position < triangles.num_triangles)
			return true;
		return false;
	}

	public void next() {
		triangle2d_delaunay_position++;
	}

	public void retrieve_triangle_points(Point[] triangle_points) {
		if (triangle_points.Length < 3)
			throw new Exception("retrieve_triangle_points() method must be passed an empty Point[] array of length 3.");
		if (!ready())
			throw new Exception("retrieve_triangle_points() called when triangle points are unavailable.");

		for (int loop = 0; loop < 3; loop++) {
			int point_index = triangles.tris[triangle2d_delaunay_position * 3 + loop];
			triangle_points[loop] = new Point();
			triangle_points[loop].X = (int)(triangles.points[point_index].x);
			triangle_points[loop].Y = (int)(triangles.points[point_index].y);
		}
	}
}

}
