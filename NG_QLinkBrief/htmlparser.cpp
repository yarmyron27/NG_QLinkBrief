#include "htmlparser.h"

Htmlparser::Htmlparser(QObject* parent) : QObject(parent) {

    m_manager = new QNetworkAccessManager(this);
    m_currentReply = nullptr;

    connect(m_manager, &QNetworkAccessManager::finished, this, &Htmlparser::downloadFinished);

}

void Htmlparser::downloadFinished(QNetworkReply *reply) {
    if (reply != m_currentReply) {
        reply->deleteLater();
        return;
    }

    m_currentReply = nullptr;

    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError) {
        QString message = reply->errorString();
        if (httpStatus > 0) {
            message += QStringLiteral(" (HTTP ") + QString::number(httpStatus) + QStringLiteral(")");
        }
        emit error(message);
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QString htmlText = QString::fromUtf8(data);

    if (httpStatus > 0 && (httpStatus < 200 || httpStatus >= 300)) {
        emit error(QStringLiteral("HTTP error ") + QString::number(httpStatus));
        reply->deleteLater();
        return;
    }

    emit htmlReady(htmlText);
    reply->deleteLater();
}


void Htmlparser::fetch(const QString& writeUrl) {
    cancelOperation();

    if (!m_manager) {
        emit error(QStringLiteral("Network manager is null"));
        return;
    }

    const QUrl url = QUrl::fromUserInput(writeUrl);
    const QString scheme = url.scheme().toLower();
    if (scheme != QStringLiteral("http") && scheme != QStringLiteral("https")) {
        emit error(QStringLiteral("Only http/https supported"));
        return;
    }

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

    m_currentReply = m_manager->get(request);

    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &Htmlparser::downloadProgress);
}

void Htmlparser::cancelOperation() {
    if (m_currentReply) {
        if (m_currentReply->isRunning()) {
            m_currentReply->abort();
        }
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}

void Htmlparser::downloadProgress(int bytesReceived, int bytesTotal) {
    if (bytesTotal > 0) {
        const int procent = int((bytesReceived * 100) / bytesTotal);
        emit progressBar(procent);
    }
}

