#ifndef _MAINWINDOW_H__
#define  _MAINWINDOW_H__

#include <QMainWindow>
#include "binder/ValueBinder.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    binder::ValueBinder<int> m_value_binder;

};

#endif //  _MAINWINDOW_H__