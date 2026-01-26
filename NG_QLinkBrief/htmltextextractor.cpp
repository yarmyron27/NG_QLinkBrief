#include "htmltextextractor.h"

Htmltextextractor::Htmltextextractor(QObject* parent)
    : QObject(parent) {}


void Htmltextextractor::extract(const QString& rawHtml) {
    if (rawHtml.isEmpty()) {
        emit error(QStringLiteral("Empty HTML"));
        return;
    }

    const QString text = process(rawHtml);
    emit textReady(text);
}

QString Htmltextextractor::process(const QString& rawHtml) {
    QString jsonResult = tryJsonLd(rawHtml);

    if (!jsonResult.isEmpty()) {
        qDebug() << "Algorithm: used Json-ld";
        return jsonResult;
    }
    QString cleanHtml = removeJunk(rawHtml);

    QString smartResult = trySmartParagraphs(cleanHtml);
    if (smartResult.length() > 200) {
        qDebug() << "Algorithm: used smart paragraphs";
        return smartResult;
    }

    qDebug() << "Algorithm: used regular clean";
    return regularClean(cleanHtml);
}
