#ifndef TEXTSUMMARIZER_H
#define TEXTSUMMARIZER_H

#include <QObject>
#include <QString>
#include <QDebug>

class Textsummarizer : public QObject
{
    Q_OBJECT

public:
    explicit Textsummarizer(QObject* parent=nullptr);

public slots:
    void summarize(const QString& text);

signals:
    void summaryReady(const QString& summaryText);
    void error(const QString& message);


};

#endif // TEXTSUMMARIZER_H
