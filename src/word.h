#ifndef WORD_H
#define WORD_H
#include <QTreeWidgetItem>

class Word : public QTreeWidgetItem
{
public:
    Word();
    Word(QString newText, QVector<QPair<int, int>> newPosition);
    void init(QString newText, QVector<QPair<int, int>> newPosition);
    void setPosition(QVector<QPair<int, int>>);
    QVector<QPair<int, int>> getPosition();
    void setWordText(QString text);
    QString getWordText();
    int length();
private:
    QVector<QPair<int, int>> position;
    QString text;

};

#endif // WORD_H
