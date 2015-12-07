#include <QDebug>
#include <QPair>
#include <QVector>
#include "solver.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <set>
#include <map>
#include <QStringList>
#include <QList>
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
#include "mainwindow.h"
#include "ui_mainwindow.h"

bool comp(const std::pair<QString, QVector<std::pair<int,int>>>& word1,
                  const std::pair<QString, QVector<std::pair<int,int>>>& word2) {
    if(word1.first.length() == word2.first.length()) {
        return word1.first < word2.first;
    }
    else {
        return word1.first.length() > word2.first.length();
    }
}

Solver::Solver(QObject *parent) : QObject(parent)
{
    qDebug() << "Init Solver";
}
//using namespace std;
const int size = 4;

//Solves the words in the given grid
bool Solver::solve(QVector<QVector<QChar>>& grid, QVector<QVector<Word*>>& words)
{
    qDebug() << "Solver::solve called";

    QVector<QString> words2;

    QFile inputFile("../dictionaries/english.dic");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          words2.push_back(line);
       }
       inputFile.close();
       qDebug() << "dictionary read successful";
    }
    else {
        qDebug() << "Failed to open dictionary file";
        return false;
    }

    //tallennetaan grid vectoriin, rivit kerrallaan
    QVector<QString> grid2;
    std::set<QChar> chars;
    for(int i = 0; i < size; i++) {
        QString line = "";
        for(int j = 0; j < size; j++) {
            line += grid[i][j];
            chars.insert(grid[i][j]);
        }
        grid2.push_back(line);
    }
    qDebug() << "grid saved to a vector";

    //charmap contains char -> Set(coordinates) example: 'S' -> Set(00, 31)
    std::map<QChar, std::set<std::pair<int,int>>> charMap;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {

            QChar c = grid2[i][j];

            //katsotaan ekaks lÃ¶ytyykÃ¶ setistÃ¤ jo kyseinen kirjain, jos ei lÃ¶ydy
            //niin luodaan sille avain-arvo pari
            if (charMap.find (c) == charMap.end()) {
                std::pair<int,int> pari;
                std::set<std::pair<int,int>> koordinaatit;
                pari = std::make_pair(i,j);
                koordinaatit.insert(pari);
                charMap[c] = koordinaatit;
            }
            //jos lÃ¶ytyy jo, niin pÃ¤ivitetÃ¤Ã¤n vanhan arvoa
            else {
                std::set<std::pair<int,int>> vanhat;
                vanhat = charMap.find(c)->second;
                std::pair<int,int> pari;
                std::set<std::pair<int,int>> uudet;
                pari = std::make_pair(i,j);
                uudet.insert(pari);
                uudet.insert(vanhat.begin(),vanhat.end());
                charMap[c] = uudet;
            }

        }
    }
    qDebug() << "charmap created";

    //filterÃ¶idÃ¤Ã¤n ne sanat
    QVector<QString> finalWords;
    //jokainen sana lÃ¤pi
    for(auto str : words2) {
        int i = 0;
        int l = str.length();
        bool found = false;
        //uppercaseen
        str = str.toUpper();
        //kÃ¤ydÃ¤Ã¤n sanan kirjaimet lÃ¤pi ja jos lÃ¶ytyy kirjain mitÃ¤ ei ole gridissÃ¤
        //siirrytÃ¤Ã¤n seuraavaan sanaan
        while (i < l && !found) {
            if(chars.find(str[i]) == chars.end()) {
                found = true;
            }
            i++;
        }
        //jos kaikki kirjaimet ovat gridissÃ¤ ja pituus yli 2, lisÃ¤tÃ¤Ã¤n lopullisten
        //tutkittavien sanojen listaan
        if(!found && l > 2) {
            finalWords.push_back(str);
        }
    }
    qDebug() << "early word filtering done";

    //itse ohjelman ajo, kÃ¤ydÃ¤Ã¤n jokainen tutkittava sana lÃ¤pi

    //tallennetaan wordListiin sanat, mitkÃ¤ lÃ¶ytyy gridistÃ¤
    QVector<std::pair<QString, QVector<std::pair<int,int>>>> wordList;
    qDebug() << "start searching for words in grid";
    for(auto word : finalWords) {
        //startsiin tallennetaan kaikki koordinaatit, mistÃ¤ tuktkittavan
        //ensimmÃ¤inen kirjain lÃ¶ytyy
        std::set<std::pair<int,int>> starts;
        starts = charMap.at(word[0]);
        //wordFound tells us in the end if 'word' can be found from grid
        bool wordFound = false;
        //tutkitaan DFS:lla (inner) kaikista starteista sanaa
        for(auto start : starts) {
            //path sisÃ¤ltÃ¤Ã¤ "reitin" mitÃ¤ pitkin sana on lÃ¶ytynyt
            //TODO: palauta jotenkin reitti (UI hommiin)
            QVector<std::pair<int,int>> path;
            path.push_back(start);
            //kutsutaan inneria, counter = 0, path = aloituspiste, start = aloituspiste
            //wordFound = false, word = word ja charMap = aiemmin luotu charMap
            //koska funktioita ei voi nestata mainin sisÃ¤Ã¤n, pitÃ¤Ã¤ esim
            //charMap viedÃ¤ tÃ¤tÃ¤ kautta parametrina innerille
            QVector<std::pair<int,int>> finalPath;
            finalPath = inner(0, path, start, wordFound, word, charMap);
            //if word was found from grid, add it to 'wordList'
            if(wordFound) {
                bool notDuplicate = true;
                for(int i = 0; i < wordList.size(); i++) {
                    if (word == wordList[i].first) {
                            notDuplicate = false;
                    }
                }
                if(notDuplicate) {
                    wordList.push_back((std::make_pair(word, finalPath)));
                }
            }
        }
    }
    qDebug() << "finished searching for words in grid";

    qDebug() << "sorting word list by word length and lexicographic order";
    std::sort(wordList.begin(), wordList.end(), comp);



/*
    //loppujÃ¤rjestÃ¤miset yms

    QVector<QString> tempWordList;
    //joudutaan tekemÃ¤Ã¤n uusi sanalista sorttausta ja duplikaattien poistoa
    //varten, jonka jÃ¤lkeen haetaan pathit takaisin kiinni

    for(int i = 0; i < wordList.size(); i++) {
        tempWordList.push_back(wordList[i].first);
    }

    //sortataan sanat ja poistetaan duplikaatit
    //a dirty hack to remove duplicates
    qDebug() << "remove duplicates, sort and reverse word list...";
    auto tempset = tempWordList.toList().toSet();
    tempWordList = tempset.toList().toVector();
    qSort(tempWordList);
    //tempWordList.erase( unique( tempWordList.begin(), tempWordList.end() ), tempWordList.end() );
    //sort(tempWordList.begin(),tempWordList.end(), compare); TODO: compare ei toimi, sanat ei jÃ¤rkÃ¤ssÃ¤
    std::reverse(tempWordList.begin(),tempWordList.end());
    qDebug() << "... done.";

    //lisÃ¤tÃ¤Ã¤n pathit takaisin
    qDebug() << "start adding paths back to words";
    QVector<std::pair<QString, QVector<std::pair<int,int>>>> finalWordList;
    for(int i = 0; i < tempWordList.size(); i++) {
        QString s = tempWordList[i];
        for(int j = 0; j < wordList.size(); j++) {
            if(wordList[j].first == s) {
                finalWordList.push_back(make_pair(s, wordList[j].second));
            }
        }
    }
    qDebug() << "paths added to words";

    QVector<QVector<std::pair<QString, QVector<std::pair<int,int>>>>> wordsBySize;
    bool hasNext = true;
    //biggest wrodsize
    int biggest = 0;
    for(auto pair : finalWordList) { //std::pair<QString, QVector<std::pair<int,int>>>
        QString w = pair.first;
        if (w.length() > biggest) {
            biggest = w.length();
        }
    }
    while(hasNext) {
        QVector<std::pair<QString, QVector<std::pair<int,int>>>> wordsXsize;
        for(auto pair : finalWordList) {
            QString w = pair.first;
            if(w.length() > biggest) {
                wordsXsize.push_back(pair);
            }
        }
        wordsBySize.push_back(wordsXsize);

        int nextBiggest = 0;
        for(auto pair : finalWordList) {
            QString w = pair.first;
            if(w.length() > nextBiggest && w.length() < biggest) {
                nextBiggest = w.length();
            }
        }
        biggest = nextBiggest;
        if (biggest == 0) hasNext = false;
    }
*/
    //luodaan Word oliot ja lisÃ¤tÃ¤Ã¤n ne lopulliseen QVectoriin
    qDebug() << "start creating Word objects";
    QVector<Word*> temp;
    for (auto elem : wordList) {
        QVector<QPair<int, int>> qVec;
        auto p = elem.second;
        for(int j = 0; j < p.size(); j++) {
            qVec.push_back(qMakePair(p[j].first,p[j].second));
        }
        Word* w = new Word(elem.first, qVec);
        temp.push_back(w);
    }

    words.clear();
    words.push_back(temp);
    /*QVector<Word*> kaikki;
    for(int i = 0; i < wordList.size(); i++) {
        QVector<QPair<int, int>> qVec;
        auto p = wordList[i].second;
        for(int j = 0; j < p.size(); j++) {
            qVec.push_back(qMakePair(p[j].first,p[j].second));
        }
        QString tmp = wordList[i].first;
        Word* w = new Word(tmp, qVec);
        kaikki.append(w);
    }
    qDebug() << "Word objects created";
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    //qDebug() << "wordsBySizeSize: " << wordsBySize.size();
    //Remove the old words
    words.clear();

    //kaikki = solverin hakemat sanat
    words.append(kaikki);
    */
    qDebug() << "returning from solver";
    return true;
}

QVector<std::pair<int,int>> Solver::adjacent(int x, int y, QChar c,
                                                 std::pair<int,int> prev,
                                                 QVector<std::pair<int,int>> path,
                                                 std::map<QChar, std::set<std::pair<int,int>>> cMap)
    {
        //tungetaan adja vectoriin kaikki viereiset ruudut
        QVector<std::pair<int,int>> adja;
        adja.push_back(std::make_pair(x-1,y-1));
        adja.push_back(std::make_pair(x-1,y));
        adja.push_back(std::make_pair(x-1,y+1));
        adja.push_back(std::make_pair(x,y-1));
        adja.push_back(std::make_pair(x,y+1));
        adja.push_back(std::make_pair(x+1,y-1));
        adja.push_back(std::make_pair(x+1,y));
        adja.push_back(std::make_pair(x+1,y+1));
        QVector<std::pair<int,int>> filtered;

        //kÃ¤ydÃ¤Ã¤n lÃ¤pi adja vectori ja lisÃ¤tÃ¤Ã¤n filterediin vain ne, mitkÃ¤ ovat ruudukon sisÃ¤llÃ¤, eivÃ¤tkÃ¤ ole pathissa
        //ja tÃ¤ytyy olla haluttu kirjain
        for(auto it = adja.begin(); it != adja.end(); ++it) {
            if(it->first > -1 && it->first < size && it->second > -1 && it->second < size &&
               !(it->first == prev.first && it->second == prev.second)) {
                bool inPath = false;
                for(auto p = path.begin(); p != path.end(); ++p) {
                    if(*it == *p) {
                        inPath = true;
                    }
                }
                if(!inPath) {
                    bool wantedCharacter = true;
                    //lets filter all but the characters we are looking for
                    std::set<std::pair<int,int>> charLocations;
                    charLocations = cMap.at(c);
                    if(charLocations.find(*it) == charLocations.end()) {
                        wantedCharacter = false;
                    }
                    if(wantedCharacter) {
                        filtered.push_back(*it);
                    }
                }
            }
        }
        return filtered;
    }

QVector<std::pair<int,int>> Solver::inner(int counter,
                                               QVector<std::pair<int,int>> path,
                                               std::pair<int,int> previous,
                                               bool& wordFound, QString word,
                                               std::map<QChar, std::set<std::pair<int,int>>> cMap) {
    //jos counter tarpeeksi iso, ollaan lÃ¶ydetty sana
    int length = word.length();
    if(counter == length -1) {
        wordFound = true;
        return path;
    }
    else{
        //haetaan nykyisen ruudun kaikki viereiset ruudut mihin voidaan siirtyÃ¤ ja kutsutaan inneriÃ¤ kaikille
        std::pair<int,int> current;
        current = path.back();
        int c = counter + 1;
        QVector<std::pair<int,int>> adj;
        adj = adjacent(current.first,current.second,word[c], previous, path, cMap);

        for(auto branch : adj) {
            QVector<std::pair<int,int>> newPath;
            newPath = path;
            newPath.push_back(branch);
            inner(c, newPath, current, wordFound, word, cMap);
        }
    }
    return path;
}


