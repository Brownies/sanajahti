#include "word.h"
#include <QDebug>

Word::Word()
{

}

Word::Word(QString newText, QVector<QPair<int, int>> newPosition)
{
    init(newText, newPosition);
}

void Word::init(QString newText, QVector<QPair<int, int>> newPosition)
{
    setWordText(newText);
    setPosition(newPosition);
}

void Word::setPosition(QVector<QPair<int, int>> newPosition)
{
    position = newPosition;
}

QVector<QPair<int, int>> Word::getPosition()
{
    return position;
}

void Word::setWordText(QString newText)
{
    text = newText;
    setText(0, newText);
}

QString Word::getWordText()
{
    return text;
}

int Word::length()
{
    return getWordText().length();
}
