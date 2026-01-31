#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QStringListModel>
#include <QUrl>

#include "htmlparser.h"
#include "htmltextextractor.h"
#include "textsummarizer.h"

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

private slots:
    void onStartSamorize();
    void onReturnToStart();
    void resetUrlStyle();

    // void onHtmlReady(const QString& html);
    void onParserError(const QString& message);

    // void onExtractedTextReady(const QString& text);
    void onExtractorError(const QString& message);

    void onSummaryReady(const QString& summary);
    void onSummaryError(const QString& message);

private:
    Ui::MainWindow* ui;
    Htmlparser* m_parser;
    Htmltextextractor* m_extractor;
    Textsummarizer* m_summarizer;

    QStringListModel* m_responseModel;

};
#endif // MAINWINDOW_H
