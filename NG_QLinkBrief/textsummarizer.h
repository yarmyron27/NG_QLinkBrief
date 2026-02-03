#ifndef TEXTSUMMARIZER_H
#define TEXTSUMMARIZER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QUrlQuery>
#include <QUrl>

class Textsummarizer : public QObject
{
    Q_OBJECT

public:
    explicit Textsummarizer(QObject* parent=nullptr);

public slots:
    void summarize(const QString& text);

private slots:
    void networkReply(QNetworkReply* reply);

signals:
    void summaryReady(const QString& summaryText);
    void error(const QString& message);

private:
    QNetworkAccessManager* m_managerSummary = nullptr;
    const QString m_apiKey;
    const QString m_apiUrl;

};

#endif // TEXTSUMMARIZER_H
