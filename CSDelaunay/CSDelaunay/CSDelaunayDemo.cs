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
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

namespace Delaunay
{
	public class DelaunayDemo
	{
		public static void Main(string[] arg)
		{
			System.Windows.Forms.Application.Run (new DelaunayForm ());
		}
	}

	public class DelaunayForm : Form
	{
		List<Point> m_points = new List<Point> ();
		public DelaunayForm()
		{
			Text = "Delaunay Demo";
			Paint += new PaintEventHandler (OnPaint);
			MouseDown += new MouseEventHandler (ClickHandler);
			createMenu ();
		}

		void createMenu()
		{
			MenuItem new_plot = new MenuItem ("New", new EventHandler(NewPlot));
			MenuItem grid_plot = new MenuItem ("Grid", new EventHandler(GridPlot));
			MenuItem random_plot = new MenuItem ("Random", new EventHandler(RandomPlot));
			MenuItem circle_plot = new MenuItem ("Circle", new EventHandler(CirclePlot));
			MenuItem vertical_plot = new MenuItem ("Vertical Line", new EventHandler(OneVertLinePlot));
			MenuItem horizontal_plot = new MenuItem ("Horizontal Line", new EventHandler(OneHorizLinePlot));
			MenuItem spiral_plot = new MenuItem ("Spiral", new EventHandler(SpiralPlot));
			MenuItem plots = new MenuItem("Plot", new MenuItem[] {new_plot, grid_plot, random_plot, circle_plot, vertical_plot, horizontal_plot, spiral_plot});
			MainMenu main = new MainMenu(new MenuItem[] { plots });
			Menu = main;
		}

		void ClickHandler(object sender, MouseEventArgs clickevent)
		{
			Point pt = clickevent.Location;
			m_points.Add (pt);
			Invalidate ();
		}

		void OnPaint(object sender, PaintEventArgs paintevent)
		{
			Graphics graphics = paintevent.Graphics;

			for (Delaunay del = new Delaunay(m_points); del.ready(); del.next()) {	//Iterates through triangles.
				Point[] triangle_points = new Point[3];
				del.retrieve_triangle_points(triangle_points);

				//Line plot loop
				for (int j = 0; j < 3; j++)
					graphics.DrawLine(new Pen(Color.Black), triangle_points[j].X, triangle_points[j].Y, triangle_points[((j + 1) % 3)].X, triangle_points[((j + 1) % 3)].Y);
			}

			const int circle_size = 5;
			foreach(Point point in m_points)
				graphics.FillEllipse(new SolidBrush(Color.Black), point.X - (circle_size / 2), point.Y - (circle_size / 2), circle_size, circle_size);
		}

		void NewPlot(object sender, EventArgs menuevent) {
			m_points.Clear();
			Invalidate();
		}

		void GridPlot(object sender, EventArgs menuevent) {
			m_points.Clear();

			int grid_width = 10;
			int x, y;
			for( int i = 0; i < (grid_width * grid_width); ++i ) {
				x = (i % grid_width) * 20 + 20;
				y = (i / grid_width) * 20 + 20;
				m_points.Add(new Point(x, y));
			}
			Invalidate();
		}

		void RandomPlot(object sender, EventArgs menuevent) {
			m_points.Clear();
			Random rand = new Random();

			int x_value, y_value;
			for(int y = 0; y < 16; ++y) {
				for( int x = 0; x < 16; ++x ) {
					x_value = 20 + (int)(rand.NextDouble() * 300);
					y_value = 20 + (int)(rand.NextDouble() * 300);
					m_points.Add(new Point(x_value, y_value));
				}
			}
			Invalidate();
		}

		void CirclePlot(object sender, EventArgs menuevent)
		{
			m_points.Clear();
			Random rand = new Random();

			for(int y = 0; y < 3; ++y) {
				for( int x = 0; x < 3; ++x ) {
					int cx	= 100 + x * 150;
					int cy	= 100 + y * 150;
					double radius	= 10 + (rand.NextDouble() * 50);

					for(double a = 0.0; a < 3.1415 * 2.0f; a += 0.5) {
						double x_value = 100 + (double)cx + Math.Cos(a) * radius;
						double y_value = 100 + (double)cy + Math.Sin(a) * radius;

						m_points.Add(new Point((int)x_value, (int)y_value));
					}
				}
			}
			Invalidate();
		}

		void OneVertLinePlot(object sender, EventArgs menuevent) {
			m_points.Clear();

			int x = 0;
			for(int y = 0; y < 16; ++y)
				m_points.Add(new Point(232 * 2 + x * 32, 100 + y * 32));

			Invalidate();
		}

		void OneHorizLinePlot(object sender, EventArgs menuevent) {
			m_points.Clear();

			int	x = 0;
			for(int y = 0; y < 16; ++y) 
				m_points.Add(new Point(116 + y * 32, 100 + x * 32));

			Invalidate();
		}

		void SpiralPlot(object sender, EventArgs menuevent) {
			m_points.Clear();

			int xc = 200;
			int yc = 200;
			for (int steps = 0; steps < 200; steps++) {
				int x1 = (int)(steps * Math.Cos((double)steps)) + xc;
				int y1 = (int)(steps * Math.Sin((double)steps)) + yc;

				m_points.Add(new Point(x1, y1));
			}

			Invalidate();
		}
	}
}
