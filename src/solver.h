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

    bool solve(QVector<QVector<char> > &grid, QVector<QVector<Word *> > &words);
    bool compare(const std::string& ,const std::string& );
    std::vector<std::pair<int,int> > adjacent(int , int , char , std::pair<int,int>, std::vector<std::pair<int,int> >, std::map<char, std::set<std::pair<int,int> > >);
    std::vector<std::pair<int,int> > inner(int, std::vector<std::pair<int,int> >, std::pair<int,int>, bool&, std::string, std::map<char, std::set<std::pair<int,int> > >);

};

#endif // SOLVER2_H
