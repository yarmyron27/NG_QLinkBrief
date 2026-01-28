#ifndef HTMLTEXTEXTRACTOR_H
#define HTMLTEXTEXTRACTOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QTextDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

class Htmltextextractor : public QObject
{
    Q_OBJECT
public:
    explicit Htmltextextractor(QObject* parent = nullptr);

public slots:
    void extract(const QString& rawHtml);

signals:
    void textReady(const QString& text);
    void error(const QString& message);

private:
    QString process(const QString& rawHtml);
    QString tryJsonLd(const QString& html);
    QString removeJunk(const QString& html);
    QString extractMainChunk(const QString& html);
    QString trySmartParagraphs(const QString& html);
    QString regularClean(const QString& html);
};

#endif // HTMLTEXTEXTRACTOR_H
