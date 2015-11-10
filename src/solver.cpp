#include "solver.h"
#include <QDebug>

Solver::Solver(QObject *parent) : QObject(parent)
{
    qDebug() << "Init Solver";
}

//Solves the words in the given grid
QStringList Solver::solve(QVector<QVector<char>>& grid, QStringList& words)
{
    qDebug() << "Solver::Solve( " << grid << ", " << words << ") - NOT IMPLEMENTED";
    return QStringList();
}
