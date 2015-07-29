#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);
    ui->widget->showPolys(ui->actionPolygon->isChecked());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionTriangle_toggled(bool arg1)
{
    if( arg1 ) {
	    ui->actionPolygon->setChecked(false);
	    ui->widget->repaint();
    }
}

void MainWindow::on_actionPolygon_toggled(bool arg1)
{
    if( arg1 ) {
	    ui->actionTriangle->setChecked(false);
	    ui->widget->repaint();
    }
}
