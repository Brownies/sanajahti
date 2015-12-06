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
    void startPlay();
    void nextCell();
    bool changeTreeSelection(int direction);
    void drawWord(Word* selected);
    void drawNext();
protected:
    bool eventFilter(QObject* obj, QEvent* event);
    void timerEvent(QTimerEvent *event);

signals:
    void fileDropped(QString filePath);
private:
    int charTimerID;
    bool charTimerOn = false;
    int charTimerDelay = 50; //milliseconds
    int wordTimerID;
    bool wordTimerOn = false;
    int wordTimerDelay = 1000; //milliseconds

protected:
    Ui::MainWindow *ui;
    QVector<QPair<int, int> > currentWord;
    QVector<QPair<int, int> >::iterator currentIterator;
    bool inputOn = false;
    int inputX = 0;
    int inputY = 0;
};

#endif // MAINWINDOW_H
