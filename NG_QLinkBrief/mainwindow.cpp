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

    ui->response_view->setModel(m_responseModel);

    connect(ui->start_samorize, &QPushButton::clicked, this, &MainWindow::on_start_samorize);
    connect(ui->line_url, &QLineEdit::returnPressed, this, &MainWindow::on_start_samorize);
    connect(ui->return_start, &QPushButton::clicked, this, &MainWindow::on_return_to_start);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_samorize() {
    QString url = ui->line_url->text();

    ui->stackedWidget->setCurrentIndex(1);

    m_responseModel->setStringList(QStringList{QStringLiteral("Loading url: ") + url});
    m_parser->fetch(url);

}

void MainWindow::on_return_to_start() {
    ui->stackedWidget->setCurrentIndex(0);
}

