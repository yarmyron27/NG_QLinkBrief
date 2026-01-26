#ifndef HTMLTEXTEXTRACTOR_H
#define HTMLTEXTEXTRACTOR_H

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "htmlparser.h"

class Htmltextextractor {

public:
    Htmltextextractor();

    QString process(const QString& rawHtml);

private:
    QString tryJsonLd(const QString& html);
    QString removeJunk(const QString& html);
    QString trySmartParagraphs(const QString& html);
    QString regularClean(const QString& html);


};

#endif // HTMLTEXTEXTRACTOR_H
