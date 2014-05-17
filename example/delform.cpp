#include "delform.h"
#include "ui_delform.h"

#include <QPainter>

#include <cstdio>
#include <cmath>

DelForm::DelForm(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::DelForm),
	num_points(0)
{
	m_ui->setupUi(this);
}

DelForm::~DelForm()
{
	delete m_ui;
}

void DelForm::changeEvent(QEvent *e)
{
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void DelForm::mousePressEvent(QMouseEvent *e)
{
	if( num_points < MAX_POINTS )
	{
		points[num_points].x	= e->x();
		points[num_points].y	= e->y();
		//		points[num_points]		= &(_points[num_points]);
		num_points++;

		this->repaint();
	}
}


void DelForm::paintEvent(QPaintEvent *e)
{
	size_t		i;

	QPainter painter;


	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);


	//qsort(points, num_points, sizeof(point2d_t*), cmp_points);

	if( num_points >= 3 )
	{
		/*del.points	= points;
		del_divide_and_conquer( &del, 0, num_points - 1 );

		del_build_faces( &del );

		del_show_cp(painter, &del );

		//draw_points();
		//del_test_faces( &del );
		del_free_halfedges( &del );
		*/
		int *faces = NULL;
		int offset = 0;
		int num_faces = delaunay2d((real*)points, num_points, &faces);

		char str[512];



		QPointF	pf[512];

		for( i = 0; i < num_faces; i++ )
		{
			int num_verts = faces[offset];
			offset++;
			for( int j = 0; j < num_verts; j++ )
			{
				int p0 = faces[offset + j];
				int p1 = faces[offset + (j+1) % num_verts];
				pf[j] = QPointF(points[p0].x, points[p0].y);
				//painter.drawLine(points[p0].x, points[p0].y, points[p1].x, points[p1].y);
			}

			int c	= (rand() % 256 + rand() % 256 + rand() % 256) / 3;
			painter.setBrush(QBrush(QColor(c, c, c)));//QBrush(QColor(rand() % 256, rand() % 256, rand() % 256)));
			painter.drawPolygon(pf, num_verts);
			offset += num_verts;
		}

		//for(int i = 0; i < num_points; ++i) {
		//	sprintf(str, "%d", i);
		//	painter.setBrush(Qt::NoBrush);
		//	painter.drawText(QPointF(points[i].x, points[i].y), str);
		//}

		//sprintf(str, "number of convex hull vertices: %d, points: %ld", faces[0], num_points);

		free(faces);

		//painter.setBrush(Qt::NoBrush);
		//painter.drawText(rect(), Qt::AlignCenter, str);
	}

	for( i = 0; i < num_points; i++ )
	{
		painter.fillRect(QRect(points[i].x - 2, points[i].y - 2, 4, 4), QBrush(QColor(0, 0, 0)));
	}

	painter.end();
}

void
DelForm::newGrid() {
	printf("new Grid\n");

	num_points	= 0;

	for(int y = 0; y < 16; ++y) {
		for( int x = 0; x < 16; ++x ) {
			points[num_points].x	= 100 + x * 32;
			points[num_points].y	= 100 + y * 32;

			num_points++;
		}
	}

	this->repaint();
}

void
DelForm::newRandom() {
	printf("new Random\n");

	num_points	= 0;

	for(int y = 0; y < 16; ++y) {
		for( int x = 0; x < 16; ++x ) {
			points[num_points].x	= 100 + (rand() & 0x1FF) + (rand() & 0x7) / 11.0f;
			points[num_points].y	= 100 + (rand() & 0x1FF) + (rand() & 0x7) / 11.0f;

			num_points++;
		}
	}

	this->repaint();
}

void
DelForm::newCircle() {
	printf("new Circle\n");

	num_points	= 0;


	for(int y = 0; y < 3; ++y) {
		for( int x = 0; x < 3; ++x ) {
			int cx	= 100 + x * 150;
			int cy	= 100 + y * 150;
			int radius	= 10 + (rand() & 0x3F);

			for(real a = 0.0; a < 3.1415 * 2.0f; a += 0.5) {
				points[num_points].x	= 100 + cx + cos(a) * radius;
				points[num_points].y	= 100 + cy + sin(a) * radius;

				num_points++;
			}
		}
	}

	this->repaint();
}

void
DelForm::newOneVert() {
	num_points	= 0;

	int	x = 0;
	for(int y = 0; y < 16; ++y) {
		points[num_points].x	= 232 * 2 + x * 32;
		points[num_points].y	= 100 + y * 32;

		num_points++;
	}

	this->repaint();
}

void
DelForm::newOneHoriz() {
	num_points	= 0;

	int	x = 0;

	for(int y = 0; y < 16; ++y) {
		points[num_points].x	= 116 + y * 32;
		points[num_points].y	= 100 + x * 32;

		num_points++;
	}

	this->repaint();
}

void
DelForm::newTwoVert() {

}

void
DelForm::newTwoHoriz() {

}

void
DelForm::newOneVertOneHoriz() {
	num_points	= 0;

	int	x = 0;
	for(int y = 0; y < 16; ++y) {
		points[num_points].x	= 100 + x * 32;
		points[num_points].y	= 100 + y * 32;

		num_points++;
	}

	x = 8;
	for(int y = 0; y < 16; ++y) {
		points[num_points].x	= 132 + y * 32;
		points[num_points].y	= 100 + x * 32;

		num_points++;
	}

	this->repaint();
}
