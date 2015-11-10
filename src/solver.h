#ifndef SOLVER_H
#define SOLVER_H
#include <QObject>
#include <QVector>
#include <QStringList>

class Solver : public QObject
{
public:
    Solver(QObject *parent = 0);
    QStringList solve(QVector<QVector<char> > &grid, QStringList& words);
};

#endif // SOLVER_H
