#ifndef MAINPROGRAM_H
#define MAINPROGRAM_H

#include <QObject>
#include <QVector>
#include "solver.h"
#include "imagereader.h"


class MainProgram : public QObject
{
    Q_OBJECT
public:
    explicit MainProgram();
    void makeGrid(QString filePath);
    QVector<QVector<char>> grid;
    QVector<QString> words;

signals:
    void gridUpdated();

private:
    QString filepath;
    Solver* solver = 0;
    ImageReader* imgReader = 0;

};

#endif // MAINPROGRAM_H