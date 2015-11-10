#include "mainprogram.h"
#include <QDebug>

MainProgram::MainProgram() : QObject()
{
    qDebug() << "Init MainProgram";
    imgReader = new ImageReader(this);
    solver = new Solver(this);
}

//Fills currentGrid with the data from the file if possible
void MainProgram::makeGrid(QString filePath)
{
    qDebug() << "MainProgram::makeGrid(" << filePath << ") - NOT IMPLEMENTED";
    //Do different things based on the filetype
    //Parse an image with Tesseract
    imgReader->initData(filePath, grid);
    //Or read from a text file
    //???

    //Then call Solver::solve

}
