#ifndef MAINPROGRAM_H
#define MAINPROGRAM_H

#include <QObject>
#include <QVector>
#include "solver.h"
#include "imagereader.h"

typedef QVector<QVector<char*>> Grid;


class MainProgram : public QObject
{
    Q_OBJECT
public:
    explicit MainProgram();
    void makeGrid(QString filePath);

private:
    QString filepath;
    Grid grid;
    QStringList words;
    Solver* solver = 0;
    ImageReader* imgReader = 0;

};

#endif // MAINPROGRAM_H
