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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;

public class JavaDelaunay extends JFrame {

	MainScreen main;
	
	public static void main(String[] args) {
		new JavaDelaunay();
	}
	
	JavaDelaunay() {
		super("Delaunay Demonstration");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		DelaunayMenu menuBar = new DelaunayMenu();
		setJMenuBar(menuBar);

		main = new MainScreen();
		getContentPane().add(main);
		pack();
		show();
	}
	
	class DelaunayMenu extends JMenuBar {
		DelaunayMenu() {
			JMenu plot_menu = new JMenu("Plot");
			add(plot_menu);

			JMenuItem new_plot = new JMenuItem("New");
			plot_menu.add(new_plot);
			new_plot.addActionListener(new MenuItemNew());
			
			JMenuItem grid_plot = new JMenuItem("Grid");
			plot_menu.add(grid_plot);
			grid_plot.addActionListener(new MenuItemGrid());
			
			JMenuItem random_plot = new JMenuItem("Random");
			plot_menu.add(random_plot);
			random_plot.addActionListener(new MenuItemRandom());
			
			JMenuItem circle_plot = new JMenuItem("Circular");
			plot_menu.add(circle_plot);
			circle_plot.addActionListener(new MenuItemCircle());
			
			JMenuItem vertline_plot = new JMenuItem("One Vertical Line");
			plot_menu.add(vertline_plot);
			vertline_plot.addActionListener(new MenuItemOneVertLine());
			
			JMenuItem horizline_plot = new JMenuItem("One Horizontal Line");
			plot_menu.add(horizline_plot);
			horizline_plot.addActionListener(new MenuItemOneHorizLine());
			
			JMenuItem spiral_plot = new JMenuItem("Spiral");
			plot_menu.add(spiral_plot);
			spiral_plot.addActionListener(new MenuItemSpiral());
		}
	}

	class MenuItemNew implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			main.NewPlot();
		}
	}
	
	class MenuItemGrid implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			main.GridPlot();
		}
	}
	
	class MenuItemRandom implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			main.RandomPlot();
		}
	}
	
	class MenuItemCircle implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			main.CirclePlot();
		}
	}

	class MenuItemOneVertLine implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			main.VertLinePlot();
		}
	}

	class MenuItemOneHorizLine implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			main.HorizLinePlot();
		}
	}
	
	class MenuItemSpiral implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			main.Spiral();
		}
	}
}
