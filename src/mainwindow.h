#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void init(QVector<QVector<char> > &grid);
    void update(QVector<QVector<char>>& grid, QVector<QString> &words);

signals:
    void fileDropped(QString filePath);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
