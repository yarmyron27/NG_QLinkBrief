#include "htmltextextractor.h"

Htmltextextractor::Htmltextextractor(QObject* parent)
    : QObject(parent) {}


void Htmltextextractor::extract(const QString& rawHtml)
{
    if (rawHtml.isEmpty()) {
        emit error(QStringLiteral("Empty HTML"));
        return;
    }

    const QString text = process(rawHtml);
    emit textReady(text);
}
