# Relatively Robust Divide and Conquer 2D Delaunay Triangulation Algorithm

Copyright 2005(c) Wael El Oraiby, All rights reserved. 

### Introduction

This is a library that builds a 2D Delaunay triangulation using a divide and conquer algorithm very similar to the one by Guibas and Stolfi.

Thanks to:

* Raja Lehtihet for bug correction.
* Dominique Schmitt and Jean-Claude Spehner for their valuable help and support.
* Jonathan Richard Shewchuk for the robust computational geomtry predicates.

### Usage

The algo builds the 2D Delaunay triangulation given a set of points of at least
3 points using:

    int delaunay2d(real *points, int num_points, int **faces);

- `points`	: point set given as a sequence of tuple x0, y0, x1, y1, ....
- `num_points`	: number of given point
- `faces`		: the triangles given as a sequence: num verts, verts indices, num verts, verts indices first face is the external face.
- `return value`	: the number of created faces

See the provided example if you want more information. The example requires Qt 5 however.

### Robustness
The robustness is controlled by the `PREDICATE` macro in `delaunay.h`:

- `FAST_PREDICATE`  : Fast but very error prone 
- `LOOSE_PREDICATE` : Use `EPSILON` defined in `delaunay.c` (less prone to error than `FAST_PREDICATE`)
- `EXACT_PREDICATE` : Use exact predicates.


### Examples
![random](https://github.com/eloraiby/delaunay/raw/master/images/random.png)
![grid](https://github.com/eloraiby/delaunay/raw/master/images/grid.png)
![vertical and horizontal lines](https://github.com/eloraiby/delaunay/raw/master/images/vertical_horizontal.png)
![circles](https://github.com/eloraiby/delaunay/raw/master/images/circles.png)

### Notes

The implementation is relatively robust (take a look at the pictures above, some of these cases will crash most freely available implementations), in case of input or predicate floating point rounding errors, it will assert. 
These stability issues when they happen are related to the in_circle or classify_point_seg predicates. Solving the remaining issues requires exact floating point arithmetic (and will reduce performance). Nonetheless, these issues will be solved when time permits.

### License

The delaunay.c source code is licensed under GPL version 3.0 or later. If you want it under another license, contact me.



