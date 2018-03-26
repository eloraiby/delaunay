/*
**  DelaunayTest.pde
**  Copyright (C) 2018 David V. Makray
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

import java.util.*;
import java.awt.*;

ArrayList<Point> points = new ArrayList<Point>();

void setup()
{
  size(500,500);
}

void draw()
{
  background(#FFFFFF);
  if (points.size() < 3)
  {
    if (points.size() == 1)
      point(points.get(0).x, points.get(0).y);
    if (points.size() == 2)
      line(points.get(0).x, points.get(0).y, points.get(1).x, points.get(1).y);
  }

  for (Delaunay del = new Delaunay(points); del.ready(); del.next())  //Iterates through triangles.
  {
    Point[] triangle_points = new Point[3];
    del.retrieve_triangle_points(triangle_points);

    //Line plot loop
    for (int j = 0; j < 3; j++)
    {
      line(triangle_points[j].x, triangle_points[j].y, triangle_points[((j + 1) % 3)].x, triangle_points[((j + 1) % 3)].y);
    }
  }
}

void mousePressed()
{
  points.add(new Point(mouseX, mouseY));
  redraw();
}