#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

class Htmlparser : public QObject {
    Q_OBJECT

public:
    explicit Htmlparser(QObject* parent=nullptr);

public slots:
    void fetch(const QString& url_text);


private:
    QNetworkAccessManager m_manager;
};

#endif // HTMLPARSER_H
