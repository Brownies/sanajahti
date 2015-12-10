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
    bool nextCell();
    bool changeTreeSelection(int direction);
    void drawWord(Word* selected);
    void drawNext();
    void solveCurrent();
    void initTableColors();
    void notFound();
    void flashWord(Word* word);

protected:
    bool eventFilter(QObject* obj, QEvent* event);
    void timerEvent(QTimerEvent *event);

signals:
    void fileDropped(QString filePath);
    void requestSolve(QVector<QVector<QChar> > currentGrid);
    void languageChanged(QString language);

private:
    QVector<QVector<QChar> > currentGrid;
    int charTimerID;
    bool charTimerOn = false;
    int charTimerDelay = 100; //milliseconds
    int wordTimerID;
    bool wordTimerOn = false;
    int wordTimerDelay = 1500; //milliseconds
    bool flashWordOn = false;
    QColor defaultBG = QColor(0, 51, 102);
    QColor defaultText = QColor(Qt::white);
    int lightFactor = 100;

protected:
    Ui::MainWindow *ui;
    QVector<QPair<int, int> > currentWord;
    QVector<QPair<int, int> >::iterator currentIterator;
    bool inputOn = false;
    int inputX = 0;
    int inputY = 0;
    int lastX = inputX;
    int lastY = inputY;
};

#endif // MAINWINDOW_H
