#include "mainprogram.h"
#include <QDebug>

MainProgram::MainProgram() : QObject()
{
    qDebug() << "Init MainProgram";
    imgReader = new ImageReader(this);
    solver = new Solver(this);
    //Init data for grid
    char space = ' ';
    QVector<char> empty(4, space);
    grid.fill(empty, 4);
    qDebug() << "Grid is: " << grid;
    //Init data for words (TEST)
}

//Fills currentGrid with the data from the file if possible
void MainProgram::makeGrid(QString filePath)
{
    qDebug() << "MainProgram::makeGrid(" << filePath << ") - NOT IMPLEMENTED";
    //Do different things based on the filetype
    //Parse an image with Tesseract
    bool imageRead = imgReader->initData(filePath, grid);
    if(imageRead) {
        solver->solve(grid, words);
        emit gridUpdated();
    }
    //Or read from a text file
    //???

    //Then call Solver::solve

}
