#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "serial_tool/serial_tool.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("test");
    this->ss_ui = new serial_tool(this);
//    ui->tabWidget->addTab(ss_ui,QIcon(":/new/prefix1/img/serialport.png"),u8"串口助手");
    ui->tabWidget->insertTab(0,ss_ui,u8"串口助手");
}

MainWindow::~MainWindow()
{
    delete ui;
}

