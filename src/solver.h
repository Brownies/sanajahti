#ifndef SOLVER_H
#define SOLVER_H
#include <QObject>
#include <QVector>
#include <QStringList>
#include "word.h"

class Solver : public QObject
{
public:
    Solver(QObject *parent = 0);
    bool solve(QVector<QVector<char> > &grid, QVector<QVector<Word *> > &words);
};

#endif // SOLVER_H
