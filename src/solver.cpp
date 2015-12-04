#include <QDebug>
#include <QPair>
#include <QVector>
#include "solver.h"

Solver::Solver(QObject *parent) : QObject(parent)
{
    qDebug() << "Init Solver";
}

//Solves the words in the given grid
bool Solver::solve(QVector<QVector<char>>& grid, QVector<QVector<Word*> >& words)
{
    qDebug() << "Solver::Solve( " << grid << ") - NOT IMPLEMENTED";
    //TEST WORDS
    //Create a pair where each character of the word is found in the grid
    QPair<int, int> testPair(0, 0);
    QPair<int, int> testPair2(0, 1);
    QPair<int, int> testPair3(0, 2);
    QPair<int, int> testPair4(1, 2);
    //Create a vector for the pairs in the corresponding order (position first letter of the word first)
    QVector<QPair<int, int>> testVect;
    testVect << testPair << testPair2 << testPair3 << testPair4;
    //Create some words for testing purposes
    Word* one = new Word( QString("ONE"), testVect);
    Word* two = new Word(QString("TWO"), testVect);
    Word* three = new Word(QString("THREE"), testVect);
    //Remove the old words
    words.clear();
    //Create vectors for each different word length
    QVector<Word*> threeLetters;
    threeLetters.append(one);
    threeLetters.append(two);
    QVector<Word*> fiveLetters;
    fiveLetters.append(three);
    //Append the vectors to words - longest words first
    words.append(fiveLetters);
    words.append(threeLetters);
    return true;
}
