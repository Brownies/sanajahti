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
    w.update(program.grid, program.words);
    w.showMaximized();
    return a.exec();
}
