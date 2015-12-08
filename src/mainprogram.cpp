#include "mainprogram.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

MainProgram::MainProgram() : QObject()
{
    qDebug() << "Init MainProgram";
    imgReader = new ImageReader(this);
    solver = new Solver(this);
    //Init data for grid
    QChar space = ' ';
    QVector<QChar> empty(4, space);
    grid.fill(empty, 4);
    qDebug() << "Grid is: " << grid;
    //Init data for words (TEST)
}

//Fills currentGrid with the data from the file if possible
void MainProgram::makeGrid(QString filePath)
{
    //Empty grid & words
    //Init data for grid
    char space = ' ';
    QVector<QChar> empty(4, space);
    grid.fill(empty, 4);
    words.clear();
    qDebug() << "MainProgram::makeGrid(" << filePath << " )";

    QString ext = filePath.split(".").last().toLower();
    QVector<QString> imagetypes = {"gif", "jpeg", "jpg", "png"};
    if(imagetypes.contains(ext)) {
        //Parse an image with Tesseract
        qDebug() << "start OCR";
        bool imageRead = imgReader->initData(filePath, grid);
        qDebug() << "OCR finished";
        if(imageRead) {
            qDebug() << "OCR was successful";
            solver->solve(grid, words);
            qDebug() << "Solved: " << grid;
            emit gridUpdated();
        }
    }
    else if (ext == "txt") {
        qDebug() << "reading to grid from text file";
        QFile inputFile(filePath);
        if (inputFile.open(QIODevice::ReadOnly)) {
           QTextStream in(&inputFile);
           for (int i = 0; i < 4; i++) {
               if (in.atEnd()) {
                   qDebug() << "file had insufficient lines";
                   return;
               }

               QString line = in.readLine();
               if (line.length() < 4) {
                   qDebug() << "line " << i << " did not have enough characters";
                   return;
               }

               for (int j = 0; j < 4; j++) {
                   grid[i][j] = line[j];
               }
           }
           inputFile.close();
        }
        else {qDebug() << "unable to open file " << filePath; return;}
        qDebug() << "text file read successful";
        solver->solve(grid, words);
        emit gridUpdated();
    }
}
