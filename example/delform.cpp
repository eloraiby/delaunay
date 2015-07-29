#include "delform.h"
#include "ui_delform.h"

#include <QPainter>

#include <cstdio>
#include <cmath>

DelForm::DelForm(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::DelForm),
	num_points_(0)
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
	if( num_points_ < MAX_POINTS )
	{
		points_[num_points_].x	= e->x();
		points_[num_points_].y	= e->y();
		//		points[num_points]		= &(_points[num_points]);
		num_points_++;

		this->repaint();
	}
}


void DelForm::paintEvent(QPaintEvent *e)
{
	size_t		i;
	QPainter	painter;

	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);


	if( num_points_ >= 3 )
	{
		QPointF		pf[512];
		delaunay2d_t*	res = delaunay2d_from(points_, num_points_);

		if( showPolys_ ) {
			unsigned int	offset = res->faces[0] + 1;
			for( i = 1; i < res->num_faces; i++ )
			{
				int num_verts = res->faces[offset];
				offset++;
				for( int j = 0; j < num_verts; j++ )
				{
					int p0 = res->faces[offset + j];
					pf[j] = QPointF(res->points[p0].x, res->points[p0].y);
				}

				int c	= (rand() % 256 + rand() % 256 + rand() % 256) / 3;
				painter.setBrush(QBrush(QColor(c, c, c)));
				painter.drawPolygon(pf, num_verts);
				offset += num_verts;
			}

		} else {
			tri_delaunay2d_t*	tdel	= tri_delaunay2d_from(res);

			for( i = 0; i < tdel->num_triangles; i++ )
			{
				for( int j = 0; j < 3; j++ )
				{
					int p0 = tdel->tris[i * 3 + j];
					pf[j] = QPointF(tdel->points[p0].x, tdel->points[p0].y);
				}

				int c	= (rand() % 256 + rand() % 256 + rand() % 256) / 3;
				painter.setBrush(QBrush(QColor(c, c, c)));
				painter.drawPolygon(pf, 3);
			}

			tri_delaunay2d_release(tdel);

		}
		delaunay2d_release(res);
	}

	for( i = 0; i < num_points_; i++ )
	{
		painter.fillRect(QRect(points_[i].x - 2, points_[i].y - 2, 4, 4), QBrush(QColor(0, 0, 0)));
	}

	painter.end();
}

void
DelForm::newGrid() {
	printf("new Grid\n");

	num_points_	= 0;

	num_points_	= 100;
	real x1[num_points_], y1[num_points_];
	real x[num_points_], y[num_points_];
	for( int i = 0; i < num_points_; ++i ) {
		x1[ i ] = y1[ i ] = i * 20 + 5;
	}
	for( int i = 0; i < num_points_; ++i ){
		x[ i ] = x1[ i / 10 ];
		y[ i ] = y1[ i % 10 ];
	}
	int j = 0, k= 0;
	for(int i=0; i < 2 * num_points_; i++) {
		points_[i].x = x[j];
		j++;
		points_[i].y = y[k];
		k++;

	}

	this->repaint();
}

void
DelForm::newRandom() {
	printf("new Random\n");

	num_points_	= 0;

	for(int y = 0; y < 16; ++y) {
		for( int x = 0; x < 16; ++x ) {
			points_[num_points_].x	= 100 + (rand() & 0x1FF) + (rand() & 0x7) / 11.0f;
			points_[num_points_].y	= 100 + (rand() & 0x1FF) + (rand() & 0x7) / 11.0f;

			num_points_++;
		}
	}

	this->repaint();
}

void
DelForm::newCircle() {
	printf("new Circle\n");

	num_points_	= 0;


	for(int y = 0; y < 3; ++y) {
		for( int x = 0; x < 3; ++x ) {
			int cx	= 100 + x * 150;
			int cy	= 100 + y * 150;
			int radius	= 10 + (rand() & 0x3F);

			for(real a = 0.0; a < 3.1415 * 2.0f; a += 0.5) {
				points_[num_points_].x	= 100 + cx + cos(a) * radius;
				points_[num_points_].y	= 100 + cy + sin(a) * radius;

				num_points_++;
			}
		}
	}

	this->repaint();
}

void
DelForm::newOneVert() {
	num_points_	= 0;

	int	x = 0;
	for(int y = 0; y < 16; ++y) {
		points_[num_points_].x	= 232 * 2 + x * 32;
		points_[num_points_].y	= 100 + y * 32;

		num_points_++;
	}

	this->repaint();
}

void
DelForm::newOneHoriz() {
	num_points_	= 0;

	int	x = 0;

	for(int y = 0; y < 16; ++y) {
		points_[num_points_].x	= 116 + y * 32;
		points_[num_points_].y	= 100 + x * 32;

		num_points_++;
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
	num_points_	= 0;

	int	x = 0;
	for(int y = 0; y < 16; ++y) {
		points_[num_points_].x	= 100 + x * 32;
		points_[num_points_].y	= 100 + y * 32;

		num_points_++;
	}

	x = 8;
	for(int y = 0; y < 16; ++y) {
		points_[num_points_].x	= 132 + y * 32;
		points_[num_points_].y	= 100 + x * 32;

		num_points_++;
	}

	this->repaint();
}

void
DelForm::showPolys(bool arg1) {
	showPolys_	= arg1;
}
