#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QStatusBar>
#include <QStringListModel>
#include <QUrl>

#include "htmlparser.h"

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
    void on_send_olama();

private:
    Ui::MainWindow *ui;
    Htmlparser *m_parser;

    class QStringListModel *m_responseModel;



};
#endif // MAINWINDOW_H
