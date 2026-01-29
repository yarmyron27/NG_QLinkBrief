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
    QString tryJsonLd(const QString& rawHtml);
    QString removeJunk(const QString& rawHtml);
    QString extractMainChunk(const QString& cleanHtml);
    QString regularClean(const QString& cleanHtml);
    //QString regularClean(const QString& html);
    QString postFilterText(const QString& primaryText);
    QString trimText(const QString& text);
};

#endif // HTMLTEXTEXTRACTOR_H
