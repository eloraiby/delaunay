#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtWidgets/QMainWindow>

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionTriangle_toggled(bool arg1);

    void on_actionPolygon_toggled(bool arg1);

private:
    Ui::MainWindowClass *ui;
};

#endif // MAINWINDOW_H
