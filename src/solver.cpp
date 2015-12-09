#include <QDebug>
#include <QPair>
#include <QVector>
#include <QFile>
#include <algorithm>
#include <set>
#include <map>
#include "solver.h"

const int size = 4;



Solver::Solver(QObject *parent) : QObject(parent)
{
    qDebug() << "Init Solver";
}

//Solves the words in the given grid
bool Solver::solve(QVector<QVector<QChar>>& grid, QVector<QVector<Word*>>& words)
{
    qDebug() << "Solver::solve called";

    QVector<QString> words2;

    QFile inputFile("../dictionaries/" + language + ".dic");
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

    //save grid to a vector row by row
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

            //see if the set already contains the letter
            //if not, create an entry for it
            if (charMap.find (c) == charMap.end()) {
                std::pair<int,int> pari;
                std::set<std::pair<int,int>> koordinaatit;
                pari = std::make_pair(i,j);
                koordinaatit.insert(pari);
                charMap[c] = koordinaatit;
            }
            //if found, update the value of the old entry
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

    //filter out unnecessary words from dict
    QVector<QString> finalWords;
    for(auto str : words2) {
        int i = 0;
        bool found = false;
        str = str.toUpper();
        while (i < str.length() && !found) {
            if(chars.find(str[i]) == chars.end()) {
                found = true;
            }
            i++;
        }
        if(!found && str.length() > 2) {
            finalWords.push_back(str);
        }
    }
    qDebug() << "dictionary filtering done";


    //the actual solving. words found in the grid are saved to wordList.
    QVector<std::pair<QString, QVector<std::pair<int,int>>>> wordList;
    qDebug() << "start searching for words in grid";
    for(auto word : finalWords) {
        //starts contains all coordinates that contain the first letter of the current word
        std::set<std::pair<int,int>> starts;
        starts = charMap.at(word[0]);

        //wordFound tells us in the end if 'word' can be found from grid
        bool wordFound = false;

        //for each starting location: search the grid with DFS
        for(auto start : starts) {
            QVector<std::pair<int,int>> path;
            path.push_back(start);

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
    if (wordList.isEmpty()) {
        qDebug() << "found no solutions, returning from solver";
        return false;
    }
    qDebug() << "sorting word list by descending word length and ascending lexicographic order";
    std::sort(wordList.begin(), wordList.end(), compare);


    //create the Word objects
    qDebug() << "start creating Word objects";
    QVector<QVector<Word*> > result;
    QVector<Word*> temp;
    int lastLength = wordList.first().first.length();
    int currentIndex = 0;
    for (auto elem : wordList) {
        QVector<QPair<int, int>> v;
        auto p = elem.second;
        for(int j = 0; j < p.size(); j++) {
            v.push_back(qMakePair(p[j].first,p[j].second));
        }
        Word* w = new Word(elem.first, v);
        temp.push_back(w);
        if(elem.first.length() != lastLength) {
            qDebug() << "Adding words of length: " << lastLength << " to result";
            lastLength = elem.first.length();
            result.insert(currentIndex, temp);
            temp.clear();
            currentIndex++;
        }
    }

    words.clear();
    words = result;
    qDebug() << words;
    qDebug() << "returning from solver";
    return true;
}

QVector<std::pair<int,int>> Solver::adjacent(int x, int y, QChar c,
                                                 std::pair<int,int> prev,
                                                 QVector<std::pair<int,int>> path,
                                                 std::map<QChar, std::set<std::pair<int,int>>> cMap)
    {
        QVector<std::pair<int,int>> adja;
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                adja.push_back(std::make_pair(x + i, y + j));
            }
        }

        QVector<std::pair<int,int>> filtered;
        //accept only those coordinates that are inside the grid, contain the desired letter and have not been visited yet
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

    if(counter == word.length() -1) {
        wordFound = true;
        return path;
    }
    else{
        //get adjacent tiles and recurse
        std::pair<int,int> current;
        current = path.back();
        int c = counter + 1;
        QVector<std::pair<int,int>> adj;
        adj = adjacent(current.first,current.second,word[c], previous, path, cMap);

        for(auto branch : adj) {
            QVector<std::pair<int,int>> newPath;
            newPath = path;
            newPath.push_back(branch);
            return inner(c, newPath, current, wordFound, word, cMap);
        }
    }
    return path;
}

//function for comparing elements of word list
bool Solver::compare(const std::pair<QString, QVector<std::pair<int,int>>>& word1,
                  const std::pair<QString, QVector<std::pair<int,int>>>& word2) {
    if(word1.first.length() == word2.first.length()) {
        return word1.first < word2.first;
    }
    else {
        return word1.first.length() > word2.first.length();
    }
}
