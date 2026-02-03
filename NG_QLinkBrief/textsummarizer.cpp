#include "textsummarizer.h"

Textsummarizer::Textsummarizer(QObject* parent) : QObject(parent)
    , m_apiKey(QString::fromUtf8(qgetenv("GEMINI_API_KEY")).trimmed())
    , m_apiUrl("https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent")
{
    m_managerSummary = new QNetworkAccessManager(this);
    m_currentReplySummary = nullptr;

    connect(m_managerSummary, &QNetworkAccessManager::finished, this, &Textsummarizer::networkReply);
}

void Textsummarizer::summarize(const QString& text) {
    if (text.isEmpty()) {
        emit error("Input text for summarization is empty");
        return;
    }
    if (m_apiKey.isEmpty()) {
        emit error("Missing GEMINI_API_KEY");
        return;
    }

    cancelOperationSummary();
    qDebug() << "summarizer start";

    // requests
    QNetworkRequest request(m_apiUrl);
    request.setRawHeader("x-goog-api-key", m_apiKey.trimmed().toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString promptAndText = "Analyze the text provided. First, write a brief description of the main topic in one sentence. Then immediately after that, write a concise summary of the content. Write all this in the language in which the text is written " + trimText(text);

    //create object with prompt and text and put on list
    QJsonObject partObject;
    partObject["text"] = promptAndText;
    QJsonArray partsArray;
    partsArray.append(partObject);

    //create object content and put list part in it
    QJsonObject contentObject;
    contentObject["parts"] = partsArray;
    QJsonArray contentsArray;
    contentsArray.append(contentObject);

    // create root object and insert list content
    QJsonObject rootObject;
    rootObject["contents"] = contentsArray;

    //convert json document to byte array
    QByteArray jsonData = QJsonDocument(rootObject).toJson();

    m_currentReplySummary = m_managerSummary->post(request, jsonData);
}

void Textsummarizer::networkReply(QNetworkReply* reply) {
    if (reply != m_currentReplySummary) {
        reply->deleteLater();
        return;
    }
    m_currentReplySummary = nullptr;
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit error("Gemini network error " + reply->errorString());
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit error("JSON Parse error " + parseError.errorString());
        return;
    }

    QJsonObject root = jsonResponse.object();

    if (root.contains("error")) {
        QJsonObject errorObject = root["error"].toObject();
        QString apiMessage = errorObject["message"].toString();
        QString status = errorObject["status"].toString();
        emit error("Gemini API error " + status + apiMessage);
        return;
    }

    if (root.contains("candidates")) {
        QJsonArray candidates = root["candidates"].toArray();

        QString fullTextResult;
        foreach (const QJsonValue &candidateVal, candidates) {
            QJsonObject candidate = candidateVal.toObject();
            if (candidate.contains("finishReason")) {
                QString reason = candidate["finishReason"].toString();
                if (reason != "STOP") {
                    qDebug() << "Skip candidate to" << reason;
                    continue;
                }
            }

            if (candidate.contains("content")) {
                QJsonObject content = candidate["content"].toObject();
                if (content.contains("parts")) {
                    QJsonArray parts = content["parts"].toArray();

                    foreach (const QJsonValue &partVal, parts) {
                        QJsonObject part = partVal.toObject();
                        if (part.contains("text"))
                            fullTextResult += part["text"].toString();
                    }
                }
            }
        }

        if (!fullTextResult.isEmpty()) {
            emit summaryReady(fullTextResult);
            return;
        }
    }

    emit error("Gemini returned generic empty response");
}

void Textsummarizer::cancelOperationSummary() {
    if (m_currentReplySummary) {
        m_currentReplySummary->disconnect(this);
        if (m_currentReplySummary->isRunning()) {
            m_currentReplySummary->abort();
        }
        m_currentReplySummary->deleteLater();
        m_currentReplySummary = nullptr;
    }
}

QString Textsummarizer::trimText(const QString& text) {
    const int maxChars = 40000;

    if (text.size() < maxChars) return text;

    return text.left(maxChars);
}

