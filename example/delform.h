#ifndef DELFORM_H
#define DELFORM_H

#include <QWidget>

#include "../delaunay.h"

namespace Ui {
class DelForm;
}

#define MAX_POINTS		512

class DelForm : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(DelForm)
public:
	explicit DelForm(QWidget *parent = 0);
	virtual ~DelForm();

protected:
	virtual void changeEvent(QEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void paintEvent(QPaintEvent *e);

public slots:
	void		newDoc() { num_points = 0; this->repaint(); }
	void		newRandom();
	void		newGrid();
	void		newCircle();
	void		newOneVert();
	void		newOneHoriz();
	void		newTwoVert();
	void		newTwoHoriz();
	void		newOneVertOneHoriz();

private:
	Ui::DelForm *m_ui;
	struct point2d {
		real	x, y;
	};
	point2d		points[MAX_POINTS];
	size_t		num_points;
};

#endif // DELFORM_H
