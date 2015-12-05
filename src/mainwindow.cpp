#include <QStringList>
#include <QList>
#include <QDebug>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include <QTextStream>
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
    //Install the custom event filter
    ui->tableWidgetGrid->installEventFilter(this);
    //Connect fileToGrid-pushButton
    connect(ui->buttonInput, &QAbstractButton::clicked, [&]() {
        inputToGrid();
    });
    //Connect fileToGrid-button
    connect(ui->buttonFile, &QAbstractButton::clicked, [&]() {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        "Open file for grid",
                                                        "/home/",
                                                        "Images (*.png *.jpg);;Text (*.txt)");
        emit fileDropped(fileName);
    });
    //Connect TreeWidget item selection to draw the corresponding word on the grid
    connect(ui->treeWidgetWords, &QTreeWidget::itemSelectionChanged, [&]() {
        QTreeWidgetItem* current = ui->treeWidgetWords->selectedItems().first();
        if(current->childCount() == 0) { //Selection == Word, draw it on grid
            Word* selected = (Word*) ui->treeWidgetWords->selectedItems().first();
            drawWord(selected);
        } else { //Header "n letters" -> dont draw anything on grid
            ui->tableWidgetGrid->clearSelection();
        }
    });
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

void MainWindow::init(QVector<QVector<QChar>>& grid)
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
            QChar current = grid.at(x).at(y);
            QTableWidgetItem* newItem = new QTableWidgetItem(current);
            newItem->setTextAlignment(Qt::AlignCenter);
            gridWidget->setItem(x, y, newItem);
        }
    }
    gridWidget->viewport()->update();
}

void MainWindow::update(QVector<QVector<QChar>>& grid, QVector<QVector<Word*>> &words) {
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
            QChar current = QChar(grid.at(x).at(y));
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

void MainWindow::inputToGrid() {
    //Select the grid and start asking for input
    QTableWidget* table = ui->tableWidgetGrid;
    table->setCurrentCell(0, 0);
    ui->tableWidgetGrid->setFocus();
    inputOn = true;
    //table->itemAt(0, 0)->setText(QString(newChar));

}

void MainWindow::nextCell() {
    QTableWidget* table = ui->tableWidgetGrid;
    if(inputX < 3) {
        inputX += 1;
    } else {
        if(inputY >= 3) {
            //End filling
            inputOn = false;
            inputX = 0;
            inputY = 0;
        } else {
            inputX = 0;
            inputY += 1;
        }
    }
    table->setCurrentCell(inputY, inputX);
    ui->tableWidgetGrid->setFocus();
}

void MainWindow::drawWord(Word* selected) {
    QVector<QPair<int, int> > positions = selected->getPosition();
    qDebug() << "Drawing word in grid at: " << positions;
    for(QPair<int, int> pair : positions) {
        ui->tableWidgetGrid->selectionModel()->select(
                    ui->tableWidgetGrid->model()->index(pair.first, pair.second),
                    QItemSelectionModel::Select);
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if(inputOn) {
        if(event->type()==QEvent::KeyPress) {
            QKeyEvent* key = static_cast<QKeyEvent*>(event);
            QString input(key->text());
            QString whitelist("ABCDEFGHIJKLMNOPQRSTUVWXYZÖÄÅ");//Allow ÖÄÅ
            if(input != "" && whitelist.contains(input, Qt::CaseInsensitive)) {
                ui->tableWidgetGrid->item(inputY, inputX)->setText(input);
                nextCell();
                return true;
            }
            if(key->key() == Qt::Key_Escape) {
                inputOn = false;
                inputY = 0;
                inputX = 0;
                ui->tableWidgetGrid->setCurrentCell(0, 0);
                return true;
            }
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
