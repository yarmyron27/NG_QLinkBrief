#include "htmlparser.h"

Htmlparser::Htmlparser(QObject* parent) : QObject(parent) {

    m_manager = new QNetworkAccessManager(this);
    m_currentReply = nullptr;

    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &Htmlparser::onTimeout);

    connect(m_manager, &QNetworkAccessManager::finished, this, &Htmlparser::downloadFinished);

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

    connect(m_currentReply, &QNetworkReply::sslErrors, this, &Htmlparser::onSslErrors);

    m_timeoutTimer->start(10000);
}

void Htmlparser::downloadFinished(QNetworkReply *reply) {
    m_timeoutTimer->stop();

    if (reply != m_currentReply) {
        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        emit error("Network error " + reply->errorString());
        cancelOperation();
        return;
    }

    const int statusVar = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusVar >= 300) {
            emit error("HTTP eror " + QString::number(statusVar));
            cancelOperation();
            return;
        }

    QByteArray data = reply->readAll();
    QString htmlText = QString::fromUtf8(data);

    if (htmlText.isEmpty()) {
        emit error("Server returned empty data");
    } else {
        emit htmlReady(htmlText);
    }

    cancelOperation();
}

void Htmlparser::cancelOperation() {
    if (m_timeoutTimer->isActive())
        m_timeoutTimer->stop();

    if (m_currentReply) {
        m_currentReply->disconnect(this);
        if (m_currentReply->isRunning()) {
            m_currentReply->abort();
        }
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}

void Htmlparser::onTimeout() {
    emit error("Connection timed out Server not found");
    cancelOperation();
}

void Htmlparser::onSslErrors(const QList<QSslError> &errors) {
    QString message = "SSL Error " + errors.first().errorString();
    emit error(message);
    cancelOperation();
}

void Htmlparser::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    if (bytesTotal > 0) {
        const int procent = int((bytesReceived * 100) / bytesTotal);
        emit progressBar(procent);
    }
}

