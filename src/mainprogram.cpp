#include "mainprogram.h"
#include <QDebug>

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
    qDebug() << "MainProgram::makeGrid(" << filePath << ") - NOT IMPLEMENTED";
    qDebug() << "MainProgram::makeGrid(" << filePath << " )";
    //Do different things based on the filetype
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
    //Or read from a text file
    //???

    //Then call Solver::solve

}
