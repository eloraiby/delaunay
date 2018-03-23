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

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.*;

public class MainScreen extends JPanel {
	private ArrayList<Point> m_points;

	public MainScreen() {
		m_points = new ArrayList<>();

		setPreferredSize(new Dimension(400, 300));
		addMouseListener(new PointListener());
	}

	public void paintComponent(Graphics graphics) {
		super.paintComponent(graphics);

		for (Delaunay del = new Delaunay(m_points); del.ready(); del.next()) {	//Iterates through triangles.
			Point[] triangle_points = new Point[3];
			del.retrieve_triangle_points(triangle_points);

			//Line plot loop
			for (int j = 0; j < 3; j++) {
				graphics.drawLine(triangle_points[j].x, triangle_points[j].y, triangle_points[((j + 1) % 3)].x, triangle_points[((j + 1) % 3)].y);
			}
		}
		
		final int circle_size = 5;
		for (Point current : m_points)
			graphics.fillOval(current.x - (circle_size / 2), current.y - (circle_size / 2), circle_size, circle_size);
	}

	class PointListener implements MouseListener {

		@Override
		public void mouseClicked(MouseEvent e) {
		}

		@Override
		public void mousePressed(MouseEvent e) {
			m_points.add(e.getPoint());
			repaint();
		}

		@Override
		public void mouseReleased(MouseEvent e) {
		}

		@Override
		public void mouseEntered(MouseEvent e) {
		}

		@Override
		public void mouseExited(MouseEvent e) {
		}

	}

	void NewPlot() {
		m_points.clear();
		repaint();
	}	

	void GridPlot() {
		m_points.clear();

		int grid_width = 10;
		int x, y;
		for( int i = 0; i < (grid_width * grid_width); ++i ) {
			x = (i % grid_width) * 20 + 20;
			y = (i / grid_width) * 20 + 20;
			m_points.add(new Point(x, y));
		}
		repaint();
	}

	void RandomPlot() {
		m_points.clear();
		Random rand = new Random();
		
		int x_value, y_value;
		for(int y = 0; y < 16; ++y) {
			for( int x = 0; x < 16; ++x ) {
				x_value = 20 + (int)(rand.nextDouble() * 300);
				y_value = 20 + (int)(rand.nextDouble() * 300);
				m_points.add(new Point(x_value, y_value));
			}
		}
		repaint();
	}

	void CirclePlot() {
		m_points.clear();
		Random rand = new Random();

		for(int y = 0; y < 3; ++y) {
			for( int x = 0; x < 3; ++x ) {
				int cx	= 100 + x * 150;
				int cy	= 100 + y * 150;
				double radius	= 10 + (rand.nextDouble() * 50);

				for(double a = 0.0; a < 3.1415 * 2.0f; a += 0.5) {
					double x_value = 100 + (double)cx + Math.cos(a) * radius;
					double y_value = 100 + (double)cy + Math.sin(a) * radius;

					m_points.add(new Point((int)x_value, (int)y_value));
				}
			}
		}
		repaint();
	}

	void VertLinePlot() {
		m_points.clear();

		int x = 0;
		for(int y = 0; y < 16; ++y)
			m_points.add(new Point(232 * 2 + x * 32, 100 + y * 32));

		repaint();
	}

	void HorizLinePlot() {
		m_points.clear();

		int	x = 0;
		for(int y = 0; y < 16; ++y) 
			m_points.add(new Point(116 + y * 32, 100 + x * 32));

		repaint();
	}
	
	void Spiral() {
		m_points.clear();

		int xc = 200;
		int yc = 200;
		for (int steps = 0; steps < 200; steps++)
		{
			int x1 = (int)(steps * Math.cos((double)steps)) + xc;
			int y1 = (int)(steps * Math.sin((double)steps)) + yc;

			m_points.add(new Point(x1, y1));
		}

		repaint();
	}
}
