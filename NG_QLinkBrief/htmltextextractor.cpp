#include "htmltextextractor.h"

Htmltextextractor::Htmltextextractor(QObject* parent) : QObject(parent) {}

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

QString Htmltextextractor::tryJsonLd(const QString& html) {
    QRegularExpression regex(R"(<script type="application/ld\+json">(.*?)</script>)",
                            QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator counter = regex.globalMatch(html);

    while (counter.hasNext()) {
        QRegularExpressionMatch match = counter.next();
        QString jsonString = match.captured(1);

        QJsonParseError parseErr;
        QJsonDocument formatedJson = QJsonDocument::fromJson(jsonString.toUtf8(), &parseErr);
        if (parseErr.error != QJsonParseError::NoError) continue;

        QJsonArray arrayObject;
        if (formatedJson.isArray())
            arrayObject = formatedJson.array();
        else if (formatedJson.isObject())
            arrayObject.append(formatedJson.object());

        foreach (const QJsonValue& value, arrayObject) {
            QJsonObject object = value.toObject();

            QString type = object["@type"].toString();

            if (type.contains("Article") || type.contains("News") || type.contains("Blog")) {
                QString header = object["headline"].toString();
                QString body = object["articleBody"].toString();
                QString description = object["description"].toString();

                if (!body.isEmpty())
                    return header + "\n\n" + body;
            }
        }
    }

    return QString();
}

QString Htmltextextractor::removeJunk(const QString& html) {
    QString text = html;

    QRegularExpression junkRegex(
        R"(<script.*?>.*?</script>|<style.*?>.*?</style>|<svg.*?>.*?</svg>|)",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);

    text.remove(junkRegex);
    return text;
}

