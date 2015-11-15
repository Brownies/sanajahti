#include "mainprogram.h"
#include "mainwindow.h"
#include <QApplication>

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
    //Update the grid in the UI if the data has changed
    QObject::connect(&program, &MainProgram::gridUpdated, [&]() {
        w.update(program.grid, program.words);
    });
    w.update(program.grid, program.words);
    w.show();
    return a.exec();
}
