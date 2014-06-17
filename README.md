# Relatively Robust Divide and Conquer 2D Delaunay Construction Algorithm

Copyright 2005(c) Wael El Oraiby, All rights reserved. 

### Introduction

This is a library that builds a 2D Delaunay Construction using a divide and conquer algorithm very similar to the one by Guibas and Stolfi.

Given a set of inputs points, the program will output Delaunay faces (not necessarily triangles, as long as the points are circumscribed to a circle).

Thanks to:

* Raja Lehtihet for bug fixes and suggestions.
* Dominique Schmitt and Jean-Claude Spehner for their valuable help and support.
* Jonathan Richard Shewchuk for the robust computational geometry predicates.

### Usage

The algorithm builds the 2D Delaunay triangulation given a set of points of at least
3 points using:

    delaunay2d_t* delaunay2d(del_point2d_t *points, unsigned int num_points, incircle_predicate_t pred);

- `points`	: point set given as a sequence of tuple x0, y0, x1, y1, ....
- `num_points`	: number of given point
- `pred`		: the incircle predicate.
- `return value`	: the number of created faces

the returned `delaunay2d_t` structures contains:
- `num_points`	: the input point count
- `points`	: a copy of the input points
- `num_faces`	: the output face count
- `faces`	: the output faces indices (faces are not necessarily triangles). The first face is the external face.

See the provided example if you want more information. The example requires Qt 5 however.

### Robustness
The robustness is controlled by the `PREDICATE` macro in `delaunay.h`:

- `FAST_PREDICATE`  : Fast but very error prone 
- `LOOSE_PREDICATE` : Use `EPSILON` defined in `delaunay.c` (less prone to error than `FAST_PREDICATE`)
- `EXACT_PREDICATE` : Use exact predicates.

Also the `USE_DOUBLE` is defined to true as double numbers decrease floating point errors. Comment it to use floating point. Note however that using `EXACT_PREDICATE` will automatically define it, if it's not already defined.

### Examples
![random](https://github.com/eloraiby/delaunay/raw/master/images/random.png)
![grid](https://github.com/eloraiby/delaunay/raw/master/images/grid.png)
![vertical and horizontal lines](https://github.com/eloraiby/delaunay/raw/master/images/vertical_horizontal.png)
![circles](https://github.com/eloraiby/delaunay/raw/master/images/circles.png)

### Notes

The implementation is relatively robust (take a look at the pictures above, some of these cases will crash most freely available implementations), in case of input or predicate floating point rounding errors, it will assert. 
These stability issues when they happen are related to the in_circle or classify_point_seg predicates. Solving the remaining issues requires exact floating point arithmetic (and will reduce performance). Nonetheless, these issues will be solved when time permits.

### License

The delaunay.c source code is licensed under Affero GPL version 3.0 or later. While I think this license is the best for OSS, you can obtain a more permissive license suitable for closed source programs for a modest fee. You can contact me for that [contact me](https://github.com/eloraiby/eloraiby.github.com/raw/master/email.svg).



