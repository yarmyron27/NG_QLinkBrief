#include "htmltextextractor.h"

Htmltextextractor::Htmltextextractor(QObject* parent) : QObject(parent) {}

void Htmltextextractor::extract(const QString& rawHtml) {
    if (rawHtml.isEmpty() || rawHtml.length() < 200) {
        emit error(QStringLiteral("Empty HTML"));
        return;
    }

    const QString text = process(rawHtml);
    if (text.trimmed().isEmpty()) {
        return;
    }
    emit textReady(text);
}

QString Htmltextextractor::process(const QString& rawHtml) {
    QString jsonResult = tryJsonLd(rawHtml);
    if (!jsonResult.isEmpty() && jsonResult.length() > 200) {
        qDebug() << "Algorithm: used Json-ld";
        jsonResult = trimText(jsonResult);
        qDebug() << jsonResult.size();
        return jsonResult;
    }

    QString cleanHtml = removeJunk(rawHtml);
    QString chunk = extractMainChunk(cleanHtml);
    QString base = chunk.isEmpty() ? cleanHtml : chunk;

    QString regularResult = regularClean(base);

    if (regularResult.length() > 50) {
    regularResult = postFilterText(regularResult);
    regularResult = trimText(regularResult);
    qDebug() << "Algorithm: used smart paragraphs";
    qDebug() << regularResult.size();
    return regularResult;
    }

    emit error(QStringLiteral("Extracted text is short after cleaning"));
    return QString();
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

    text.remove(junkRegex);
    text.remove(layoutRegex);
    text.remove(classIdJunk);

    return text;
}

QString Htmltextextractor::extractMainChunk(const QString& cleanHtml) {
    const QRegularExpression articleRe(
        R"(<article\b[^>]*>(.*?)</article>)",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch search = articleRe.match(cleanHtml);
    if (search.hasMatch()) {
        qDebug() << "search article";
        return search.captured(1);
    }

    const QRegularExpression mainRe(
        R"(<main\b[^>]*>(.*?)</main>)",
        QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);

    search = mainRe.match(cleanHtml);
    if (search.hasMatch()) {
        qDebug() << "search main";
        return search.captured(1);
    }

    return QString();
}

QString Htmltextextractor::regularClean(const QString& cleanHtml) {
    QStringList results;
    QStringList blocks = cleanHtml.split(QRegularExpression("</(p|div|br|li|h\\d|tr|td|th|table)>"), Qt::SkipEmptyParts);

    foreach (const QString& block, blocks) {
        QString cleanBlock = block;

        QTextDocument doc;
        doc.setHtml(cleanBlock);
        cleanBlock = doc.toPlainText();
        cleanBlock = cleanBlock.trimmed();

        cleanBlock.remove(QChar(0xFFFC));

        if (cleanBlock.contains('|') || cleanBlock.contains(QChar(u'»')) || cleanBlock.contains(QChar(u'·')))
            continue;

        if (cleanBlock.length() < 100)
            continue;

        if (cleanBlock.contains("copyright", Qt::CaseInsensitive) ||
            cleanBlock.contains("all rights reserved", Qt::CaseInsensitive))
            continue;

        results << cleanBlock;
    }
    return results.join("\n");
}

QString Htmltextextractor::postFilterText(const QString& primaryText) {
    QString text = primaryText;

    const QRegularExpression imgSizeLineRe(R"(^\s*\d{2,4}\s*[x×]\s*\d{2,4}\s*$)");
    const QRegularExpression pxRe(R"(\b\d{2,4}\s*px\b)", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression urlRe(R"((https?://|www\.|doi:))", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression wikiRefStart(R"(^\s*[↑^])");
    const QRegularExpression citationKeywords(
        R"((Процитовано|Retrieved|Accessed|Архів оригіналу|Archived|DOI:|ISBN)\b.*?(19|20)\d{2})",
        QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression strictSentenceEnd(R"([.!?]$)");

    QStringList out;
    foreach (QString line, text.split('\n')) {
        line = line.trimmed();
        if (line.isEmpty()) continue;

        if (imgSizeLineRe.match(line).hasMatch()) continue;
        if (pxRe.match(line).hasMatch()) continue;
        if (wikiRefStart.match(line).hasMatch()) continue;
        if (citationKeywords.match(line).hasMatch()) continue;
        if (urlRe.match(line).hasMatch()) continue;

        const QStringList words = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        bool endsWithDot = line.contains(strictSentenceEnd);
        if (words.size() <= 3 && !endsWithDot) {
            continue;
        }

        short digits = 0;
        short letters = 0;
        foreach (QChar symbol, line) {
            if (symbol.isDigit()) digits++;
            if (symbol.isLetter()) letters++;
        }
        if (digits > letters && words.size() < 10) continue;

        out << line;
    }

    if (!out.isEmpty())
        qDebug() << "postFilter done work";

    return out.join("\n").trimmed();
}

QString Htmltextextractor::trimText(const QString& text) {
    QString firstText = text.trimmed();
    const int minCharsToTrim = 2000;
    const int percentToTrim = 15;

    if (firstText.size() < minCharsToTrim) return firstText;

    const int cutChars = (firstText.size() * percentToTrim) / 100;
    const int newLen = firstText.size() - cutChars;

    firstText.truncate(newLen);

    int lastBreak = firstText.lastIndexOf('\n');
    lastBreak = qMax(lastBreak, firstText.lastIndexOf(' '));
    if (lastBreak > 0 && (firstText.size() - lastBreak) < 40) {
        firstText.truncate(lastBreak);
    }

    qDebug() << "trimText successful" << cutChars;
    return firstText.trimmed();
}
