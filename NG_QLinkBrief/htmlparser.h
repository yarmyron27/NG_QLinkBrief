#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>

class Htmlparser : public QObject
{
    Q_OBJECT

public:
    explicit Htmlparser(QObject* parent=nullptr);

public slots:
    void fetch(const QString& writeUrl);
    void cancelOperationParser();

private slots:
    void downloadFinished(QNetworkReply* reply);

    void onTimeout();
    void onSslErrors(const QList<QSslError> &errors);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

signals:
    void htmlReady(const QString& html);
    void error(const QString& message);
    void progressBar(int num);


private:
    QNetworkAccessManager* m_manager = nullptr;
    QNetworkReply* m_currentReply = nullptr;
    QTimer* m_timeoutTimer;
};

#endif // HTMLPARSER_H
