#include "textsummarizer.h"

Textsummarizer::Textsummarizer(QObject* parent) : QObject(parent) {}

void Textsummarizer::summarize(const QString& text) {
    if (text.isEmpty()) {
        emit error("Input text for summarization is empty");
        return;
    }
    qDebug() << "summarizer start";

    QString summary = "--- AI SUMMARY ---\n\n";

    emit summaryReady(summary);
}
