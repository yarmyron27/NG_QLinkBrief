#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->line_url->setPlaceholderText(">>Enter URL(http/... or https/...)");

    m_parser = new Htmlparser(this);
    m_extractor = new Htmltextextractor(this);
    m_summarizer = new Textsummarizer(this);
    m_responseModel = new QStringListModel(this);

    ui->response_view->setModel(m_responseModel);
    ui->response_view->setWordWrap(true);

    // connect button and line edit for start parser
    connect(ui->start_samorize, &QPushButton::clicked, this, &MainWindow::onStartSamorize);
    connect(ui->line_url, &QLineEdit::returnPressed, this, &MainWindow::onStartSamorize);
    connect(ui->return_start, &QPushButton::clicked, this, &MainWindow::onReturnToStart);
    connect(ui->line_url, &QLineEdit::textEdited, this, &MainWindow::resetUrlStyle);

    // Parser -> Extractor
    connect(m_parser, &Htmlparser::htmlReady, m_extractor, &Htmltextextractor::extract);

    // Extractor -> Summarizer
    connect(m_extractor, &Htmltextextractor::textReady, m_summarizer, &Textsummarizer::summarize);

    // parser on ui
    //connect(m_parser, &Htmlparser::htmlReady, this, &MainWindow::onHtmlReady);

    // extractor on ui
    //connect(m_extractor, &Htmltextextractor::textReady, this, &MainWindow::onExtractedTextReady);

    // sumarizer on ui
    connect(m_summarizer, &Textsummarizer::summaryReady, this, &MainWindow::onSummaryReady);

    // error handling
    connect(m_parser, &Htmlparser::error, this, &MainWindow::onParserError);
    connect(m_extractor, &Htmltextextractor::error, this, &MainWindow::onExtractorError);
    connect(m_summarizer, &Textsummarizer::error, this, &MainWindow::onSummaryError);

    connect(m_parser, &Htmlparser::progressBar, ui->progres, &QProgressBar::setValue);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onStartSamorize() {
    QString lineUrlText = ui->line_url->text();

    QUrl url = QUrl::fromUserInput(lineUrlText);
    if (!url.isValid() || url.scheme().isEmpty()) {
        ui->line_url->clear();
        ui->line_url->setPlaceholderText("Invalid URL");
        ui->line_url->setStyleSheet("QLineEdit { color: red; }");
        return;
    }

    ui->stackedWidget->setCurrentIndex(1);
    ui->progres->setValue(0);

    m_responseModel->setStringList(QStringList{QStringLiteral("Loading url: ") + lineUrlText});
    m_parser->fetch(url.toString());

}

void MainWindow::onReturnToStart() {
    m_parser->cancelOperation();

    m_responseModel->setStringList(QStringList{});

    ui->line_url->clear();
    resetUrlStyle();

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::resetUrlStyle() {
    if (ui->line_url->styleSheet().isEmpty()) return;

    ui->line_url->setStyleSheet("");
    ui->line_url->setPlaceholderText(">>Enter URL(http/... or https/...)");
}

// void MainWindow::onHtmlReady(const QString& html)
// {
//     // QStringList lines = html.split('\n');
//     // if (lines.size() > 400) {
//     //     lines = lines.mid(0, 400);
//     //     lines << QStringLiteral("... (truncated)");
//     // }

//     m_responseModel->setStringList(lines);
// }

// void MainWindow::onExtractedTextReady(const QString& text) {
//     QStringList lines = text.split('\n');

//     if (lines.size() > 1000) {
//         lines = lines.mid(0, 1000);
//         lines << QStringLiteral("[...Text truncated...]");
//     }

//     m_responseModel->setStringList(lines);
// }

void MainWindow::onSummaryReady(const QString& text) {
    QStringList lines = text.split('\n');
    m_responseModel->setStringList(lines);
}

void MainWindow::onParserError(const QString& message) {
    m_responseModel->setStringList(QStringList{QStringLiteral("PARSER ERROR: ") + message});
}

void MainWindow::onExtractorError(const QString& message) {
    m_responseModel->setStringList(QStringList{QStringLiteral("EXTRACTOR ERROR: ") + message});
}

void MainWindow::onSummaryError(const QString& message) {
    m_responseModel->setStringList(QStringList{QStringLiteral("SUMMARIZER ERROR: ") + message});
}
