#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->line_url->setPlaceholderText(">>Enter URL(http/... or https/...)");

    m_parser = new Htmlparser(this);
    m_responseModel = new QStringListModel(this);

    ui->response_view->setModel(m_responseModel);

    connect(ui->start_samorize, &QPushButton::clicked, this, &MainWindow::on_start_samorize);
    connect(ui->line_url, &QLineEdit::returnPressed, this, &MainWindow::on_start_samorize);
    connect(ui->return_start, &QPushButton::clicked, this, &MainWindow::on_return_to_start);
    connect(ui->line_url, &QLineEdit::textEdited, this, &MainWindow::reset_url_style);

    connect(m_parser, &Htmlparser::htmlReady, this, &MainWindow::on_html_ready);
    connect(m_parser, &Htmlparser::error, this, &MainWindow::on_parser_error);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_samorize() {
    QString url_text = ui->line_url->text();

    QUrl url = QUrl::fromUserInput(url_text);
    if (!url.isValid() || url.scheme().isEmpty()) {
        ui->line_url->clear();
        ui->line_url->setPlaceholderText("Invalid URL");
        ui->line_url->setStyleSheet("QLineEdit { color: red; }");
        return;
    }

    ui->stackedWidget->setCurrentIndex(1);

    m_responseModel->setStringList(QStringList{QStringLiteral("Loading url: ") + url_text});
    m_parser->fetch(url.toString());

}

void MainWindow::on_return_to_start() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::reset_url_style() {
    if (ui->line_url->styleSheet().isEmpty()) return;

    ui->line_url->setStyleSheet("");
    ui->line_url->setPlaceholderText(">>Enter URL(http/... or https/...)");
}

void MainWindow::on_html_ready(const QString& html)
{
    QStringList lines = html.split('\n');
    if (lines.size() > 400) {
        lines = lines.mid(0, 400);
        lines << QStringLiteral("... (truncated)");
    }
    m_responseModel->setStringList(lines);
}

void MainWindow::on_parser_error(const QString& message)
{
    m_responseModel->setStringList(QStringList{QStringLiteral("ERROR: ") + message});
}

