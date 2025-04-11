#include "view/Mainwindow.h"
#include "./ui_Mainwindow.h"

#include "store/ObjectIdStore.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    REGISTER_OBJECT("main_window", this);
    REGISTER_OBJECT("le_num", ui->le_num);
    REGISTER_OBJECT("label_num", ui->label_num);

    m_value_binder.bindCallback([&](int v){ ui->label_num->setText(QString::number(v)); });
    m_value_binder.bindCallback([&](int v){ ui->le_num->setText(QString::number(v)); });

    m_value_binder = 0;
    connect(ui->btn_add_num, &QPushButton::clicked, [&](){ m_value_binder.setValue(m_value_binder.getValue() + 1); });
}


MainWindow::~MainWindow()
{
    delete ui;
}
