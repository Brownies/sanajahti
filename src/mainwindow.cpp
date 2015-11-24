#include <QStringList>
#include <QList>
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "Init MainWindow";
    ui->setupUi(this);
    //Disable the drag&drop for the children
    ui->widgetMain->setAcceptDrops(false);
    //And allow it for the main screen
    setAcceptDrops(true);
    setWindowTitle("Sanajahti solver");
    //Load the CSS
    QFile css(":/css/style.css");
    css.open(QFile::ReadOnly);
    QString stylesheet = QLatin1String(css.readAll());
    qApp->setStyleSheet(stylesheet);
}


MainWindow::~MainWindow()
{
    delete ui;
}

//Drag&Drop

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    //Allow drag&drop for files only
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mime = event->mimeData();
    if(mime->hasUrls())
    {
        //Get the URLs
        QList<QUrl> urls = mime->urls();
        //Manipulate the first URL as local path
        QString file = urls.at(0).toLocalFile();
        qDebug() << "emit MainWindow::DropEvent with file " << file;
        emit fileDropped(file);
    }
}

void MainWindow::init(QVector<QVector<char>>& grid)
{
    //Init table
    QTableWidget* gridWidget = ui->tableWidgetGrid;
    int width = grid.length();
    int height = grid.first().length();
    //Set the table size
    gridWidget->setColumnCount(width);
    gridWidget->setRowCount(height);
    gridWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    gridWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //Display the data
    for(auto x = 0; x < width; x++) {
        for(auto y = 0; y < height; y++) {
            char current = grid.at(x).at(y);
            QTableWidgetItem* newItem = new QTableWidgetItem(current);
            newItem->setTextAlignment(Qt::AlignCenter);
            gridWidget->setItem(x, y, newItem);
        }
    }
    gridWidget->viewport()->update();
}

void MainWindow::update(QVector<QVector<char>>& grid, QVector<QVector<Word*>> &words) {
    //Update table
    QTableWidget* gridWidget = ui->tableWidgetGrid;
    int width = grid.length();
    int height = grid.first().length();
    //Set the table size
    gridWidget->setColumnCount(width);
    gridWidget->setRowCount(height);
    gridWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    gridWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //Display the data
    for(auto x = 0; x < width; x++) {
        for(auto y = 0; y < height; y++) {
            //Then call Solver::solve
            char current = grid.at(x).at(y);
            QTableWidgetItem* currentItem = gridWidget->item(x, y);
            currentItem->setText(QString(current));
        }
    }
    gridWidget->viewport()->update();
    //Update word list
    QTreeWidget* wordWidget = ui->treeWidgetWords;
    wordWidget->clear();
    //Create the top level items for each vector
    for(auto i = 0; i < words.length(); ++i)
    {
        //Create a topLevelItem "x letters"
        QVector<Word*> currentWords = words[i];
        QTreeWidgetItem* current = new QTreeWidgetItem();
        QString currentText = QString::number(currentWords.first()->length()) + QString(" letters");
        qDebug() << "Adding words at: " << currentText;
        current->setText(0, currentText);
        //Add each word under the topLevelItem
        for(auto index = 0; index < currentWords.length(); ++index)
        {
            Word* word = currentWords[index];
            current->addChild(word);
        }
        //Add the topLevelItem to the tree
        wordWidget->addTopLevelItem(current);
        wordWidget->expandAll();
    }
}
