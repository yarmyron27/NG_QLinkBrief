#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet(R"(
    QWidget {
        background-color: #282c34;
        color: #abb2bf;
        font-family: "Segoe UI", "Roboto", sans-serif;
        font-size: 15px;
    }

    QMainWindow {
        background-color: #282c34;
    }

    QLineEdit {
        background-color: #21252b;
        border: 1px solid #3e4451;
        border-radius: 10px;
        padding: 5px;
        color: #dcdfe4;
        selection-background-color: #61afef;
        selection-color: #282c34;
    }

    QPushButton {
        background-color: #3b4048;
        color: #abb2bf;
        border: 1px solid #181a1f;
        border-radius: 10px;
        padding: 5px 30px;
        font-weight: 600;
    }
    QPushButton:hover {
        background-color: #4b5263;
        border: 1px solid #61afef;
        color: #ffffff;
    }
    QPushButton:pressed {
        background-color: #2c313a;
    }

    QPushButton#start_samorize {
        background-color: #61afef;
        color: #282c34;
        border: none;
    }
    QPushButton#start_samorize:hover {
        background-color: #56b6c2;
    }
    QPushButton#start_samorize:pressed {
        background-color: #4d78cc;
    }

    QListView {
        background-color: #21252b;
        border: 1px solid #181a1f;
        border-radius: 12px;
        padding: 8px;
        outline: 0;
    }

    QListView::item {
        padding: 12px;
        border-bottom: 1px solid #2c313a;
        color: #abb2bf;
        border-radius: 6px;
        margin-bottom: 4px;
    }

    QListView::item:hover {
        background-color: #2c313a;
    }

    QListView::item:selected {
        background-color: #3e4451;
        border-left: 3px solid #61afef;
        color: #ffffff;
    }

    QLabel#Q_LinkBrief {
        font-size: 48px;
        font-weight: 800;
        font-family: "Segoe UI", "Roboto", sans-serif;
        color: #61afef;
        background-color: transparent;
        padding: 10px;
        qproperty-alignment: AlignCenter;
    }
)");

    MainWindow w;
    w.show();
    return a.exec();
}
