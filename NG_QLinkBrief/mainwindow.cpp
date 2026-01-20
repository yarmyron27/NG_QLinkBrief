#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->line_url->setPlaceholderText(">>Enter URL(http/...)");

    m_parser = new Htmlparser(this);
    m_responseModel = new QStringListModel(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_send_olama() {
    QString url = ui->line_url->text();

}

