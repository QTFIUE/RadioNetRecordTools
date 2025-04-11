#include "view/Mainwindow.h"
#include "./ui_Mainwindow.h"

#include "store/ObjectIdStore.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    REGISTER_OBJECT("main_window", this);
}


MainWindow::~MainWindow()
{
    delete ui;
}
