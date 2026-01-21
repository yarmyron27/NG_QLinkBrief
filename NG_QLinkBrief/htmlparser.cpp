#include "htmlparser.h"

Htmlparser::Htmlparser(QObject* parent) : QObject(parent) {

    connect(&m_manager, &QNetworkAccessManager::finished, this, &Htmlparser::downloadFinished);

}

void Htmlparser::downloadFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QString htmlText = QString::fromUtf8(data);

    emit htmlReady(htmlText);
    reply->deleteLater();
}


void Htmlparser::fetch(const QString& url_text) {

}

