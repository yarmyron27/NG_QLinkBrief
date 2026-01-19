#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->send_olama, &QPushButton::clicked, this, &MainWindow::on_send_olama);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_send_olama() {
    QString url = ui->line_url->text();

}

