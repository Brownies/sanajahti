#include "mainprogram.h"
#include "mainwindow.h"
#include <QApplication>

struct word {
    QString text;
    QVector<QPair<int, int> > charPositions;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Init the main program
    MainProgram program;
    //Init the UI - maybe in its own thread later?
    MainWindow w;
    w.init(program.grid);
    //Make a grid of if a file is dropped
    QObject::connect(&w, &MainWindow::fileDropped, [&](QString filePath){
        program.makeGrid(filePath);
    });
    //Solve the new grid given in the UI
    QObject::connect(&w, &MainWindow::requestSolve, [&](QVector<QVector<QChar>> newGrid) {
        program.solveGrid(newGrid);
    });
    //Update the grid in the UI if the data has changed
    QObject::connect(&program, &MainProgram::gridUpdated, [&](bool solved) {
        if(solved) w.update(program.grid, program.words);
        else w.notFound();
    });
    //Change language
    QObject::connect(&w, &MainWindow::languageChanged, [&](QString language){
        program.changeLanguage(language);
    });
    w.update(program.grid, program.words);
    w.show();
    return a.exec();
}
