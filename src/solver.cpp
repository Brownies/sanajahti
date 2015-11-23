#include "solver.h"
#include <QDebug>

Solver::Solver(QObject *parent) : QObject(parent)
{
    qDebug() << "Init Solver";
}

//Solves the words in the given grid
bool Solver::solve(QVector<QVector<char>>& grid, QVector<QString>& words)
{
    qDebug() << "Solver::Solve( " << grid << ", " << words << ") - NOT IMPLEMENTED";
    words.clear();
    words << "TESTI1" << "TESTI2" << "TESTI3";
    qDebug() << "words is " << words;
    return true;
}
