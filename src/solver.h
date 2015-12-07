#ifndef SOLVER2_H
#define SOLVER2_H
#include <QObject>
#include <QVector>
#include <QStringList>
#include "word.h"
#include <vector>
#include <set>
#include <map>

class Solver : public QObject
{
public:
    Solver(QObject *parent = 0);

    bool solve(QVector<QVector<QChar> > &grid, QVector<QVector<Word *> > &words);
    //bool compare(const std::pair<QString, QVector<std::pair<int,int>>>&, const std::pair<QString, QVector<std::pair<int,int>>>&);
    QVector<std::pair<int,int>> adjacent(int , int , QChar , std::pair<int,int>, QVector<std::pair<int,int>>, std::map<QChar, std::set<std::pair<int,int>>>);
    QVector<std::pair<int,int>> inner(int, QVector<std::pair<int,int>>, std::pair<int,int>, bool&, QString, std::map<QChar, std::set<std::pair<int,int>>>);

};

#endif // SOLVER2_H
