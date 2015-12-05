#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "word.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void init(QVector<QVector<QChar> > &grid);
    void update(QVector<QVector<QChar>>& grid, QVector<QVector<Word *> > &words);
    void inputToGrid();
    void nextCell();
    void drawWord(Word* selected);
protected:
    bool eventFilter(QObject* obj, QEvent* event);

signals:
    void fileDropped(QString filePath);
private:
    Ui::MainWindow *ui;
    bool inputOn = false;
    int inputX = 0;
    int inputY = 0;
};

#endif // MAINWINDOW_H
