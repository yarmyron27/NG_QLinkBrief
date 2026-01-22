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
    void fetch(const QString& writeUrl);
    void cancelOperation();

private slots:
    void downloadFinished(QNetworkReply *reply);

signals:
    void htmlReady(const QString& html);
    void error(const QString& message);


private:
    QNetworkAccessManager m_manager;
    QNetworkReply *m_currentReply = nullptr;
};

#endif // HTMLPARSER_H
