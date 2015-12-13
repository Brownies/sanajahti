#ifndef MAINPROGRAM_H
#define MAINPROGRAM_H

#include <QObject>
#include <QVector>
#include "solver.h"
#include "imagereader.h"
#include "word.h"


class MainProgram : public QObject
{
    Q_OBJECT
public:
    explicit MainProgram();
    QVector<QVector<QChar>> grid;
    QVector<QVector<Word*> > words;
    void makeGrid(QString filePath);
    void solveGrid(QVector<QVector<QChar>> newGrid);
    void changeLanguage(QString language);

signals:
    void gridUpdated(bool);

private:
    QString filepath;
    Solver* solver = 0;
    ImageReader* imgReader = 0;
    int sizeX = 4;
    int sizeY = 4;

};

#endif // MAINPROGRAM_H
