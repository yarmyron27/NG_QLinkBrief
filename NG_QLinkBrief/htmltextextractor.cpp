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

    QString chunk = extractMainChunk(cleanHtml);
    QString base = chunk.isEmpty() ? cleanHtml : chunk;

    QString smartResult = trySmartParagraphs(base);
    smartResult = postFilterText(smartResult);
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
        R"(<script.*?>.*?</script>|<style.*?>.*?</style>|<svg.*?>.*?</svg>|<noscript\b.*?>.*?</noscript>)",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);

    QRegularExpression layoutRegex(
        R"(<(header|nav|footer|aside|form)\b.*?>.*?</\1>)",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);

    QRegularExpression classIdJunk(
        R"(<[^>]+\b(class|id)\s*=\s*["'][^"']*(menu|navbar|breadcrumb|footer|header|sidebar|share|social|comment|related|subscribe|cookie|banner|ads|advert)[^"']*["'][^>]*>.*?</[^>]+>)",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
    text.remove(classIdJunk);

    text.remove(junkRegex);
    return text;
}

QString Htmltextextractor::extractMainChunk(const QString& html) {
    const QRegularExpression articleRe(
        R"(<article\b[^>]*>(.*?)</article>)",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch search = articleRe.match(html);
    if (search.hasMatch())
        qDebug() << "search article";
        return search.captured(1);

    const QRegularExpression mainRe(
        R"(<main\b[^>]*>(.*?)</main>)",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);

    search = mainRe.match(html);
    if (search.hasMatch())
        qDebug() << "search main";
        return search.captured(1);

    return QString();
}

QString Htmltextextractor::trySmartParagraphs(const QString& html) {
    QStringList results;
    QStringList blocks = html.split(QRegularExpression("</(p|li|h\\d)>"), Qt::SkipEmptyParts);

    foreach (const QString& block, blocks) {
        QString cleanBlock = block;

        QTextDocument doc;
        doc.setHtml(cleanBlock);
        cleanBlock = doc.toPlainText();

        cleanBlock = cleanBlock.trimmed();

        if (cleanBlock.contains('|') || cleanBlock.contains(QChar(u'»')) || cleanBlock.contains(QChar(u'·')))
            continue;

        const QStringList words = cleanBlock.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        const bool hasSentencePunct = cleanBlock.contains(QRegularExpression("[\\.!\\?:;]"));
        if (words.size() <= 8 && !hasSentencePunct)
            continue;

        if (cleanBlock.length() < 100) continue;

        if (cleanBlock.contains("copyright", Qt::CaseInsensitive) ||
            cleanBlock.contains("all rights reserved", Qt::CaseInsensitive))
            continue;

        results << cleanBlock;
    }
    return results.join("\n");
}

QString Htmltextextractor::regularClean(const QString& html) {
    QString text = html;

    text.replace(QRegularExpression("</(div|p|h\\d|li)>", QRegularExpression::CaseInsensitiveOption), "\n");

    QTextDocument doc;
    doc.setHtml(text);
    text = doc.toPlainText();

    text.replace(QRegularExpression("[ \\t]+"), " ");
    text.replace(QRegularExpression("\\n\\s*\\n+"), "\n\n");
    return text.trimmed();
}

QString Htmltextextractor::postFilterText(const QString& rawText) {
    QString text = rawText;

    const QRegularExpression imgSizeLineRe(R"(^\s*\d{2,4}\s*[x×]\s*\d{2,4}\s*$)");
    const QRegularExpression pxRe(R"(\b\d{2,4}\s*px\b)", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression punctRe(R"([\.!\?:;])");
    const QRegularExpression urlRe(R"((https?://|www\.|doi:))", QRegularExpression::CaseInsensitiveOption);


    QStringList out;
    foreach (QString line, text.split('\n')) {
        line = line.trimmed();
        if (line.isEmpty()) continue;

        if (imgSizeLineRe.match(line).hasMatch()) continue;
        if (pxRe.match(line).hasMatch()) continue;
        if (urlRe.match(line).hasMatch()) continue;

        out << line;
    }

    if (!out.isEmpty())
        qDebug() << "postFilter done work";

    return out.join("\n").trimmed();
}

