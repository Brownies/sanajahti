#include <QStringList>
#include <QList>
#include <QDebug>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include <QTextStream>
#include <QTimer>
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
        if(!fileName.isEmpty()) {
            emit fileDropped(fileName);
        }
    });
    //Connect TreeWidget item selection to draw the corresponding word on the grid
    connect(ui->treeWidgetWords, &QTreeWidget::itemSelectionChanged, [&]() {
        lightFactor = 100;
        if(charTimerOn) {
            killTimer(charTimerID);
            charTimerOn = false;
        }
        QTreeWidgetItem* current = ui->treeWidgetWords->selectedItems().first();
        if(current->childCount() == 0) { //Selection == Word, draw it on grid
            Word* selected = (Word*) ui->treeWidgetWords->selectedItems().first();
            drawWord(selected);
        } else { //Header "n letters" -> dont draw anything on grid
            ui->tableWidgetGrid->clearSelection();
            initTableColors();
            ui->labelWord->clear();
        }
    });
    //Connect next/previous buttons
    connect(ui->buttonPrevious, &QPushButton::clicked, [&](){
       changeTreeSelection(1);
    });
    connect(ui->buttonNext, &QPushButton::clicked, [&](){
       changeTreeSelection(-1);
    });
    //Connect play button
    connect(ui->buttonPlay, &QPushButton::clicked, [&]() {
        startPlay();
    });
    //Connect settings button
    connect(ui->buttonSettings, &QPushButton::clicked, [&]() {
        QStackedWidget* stack = ui->stackedWidget;
        int current = stack->currentIndex();
        if(current == 0) {
            stack->setCurrentIndex(1);
            ui->buttonSettings->setText("Word list");
        } else {
            stack->setCurrentIndex(0);
            ui->buttonSettings->setText("Settings");
            ui->lineEditCharDelay->clear();
            ui->lineEditWordDelay->clear();
        }
        //Update placeholders
        ui->lineEditCharDelay->setPlaceholderText(QString::number(charTimerDelay));
        ui->lineEditWordDelay->setPlaceholderText(QString::number(wordTimerDelay));
        ui->checkBoxBlacken->setChecked(flashWordOn);
    });
    //Connect language change
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [=](int value) {
        if(value == 1) {
            emit languageChanged(QString("english"));
        } else {
            emit languageChanged(QString("finnish"));
        }
        solveCurrent();
    });
    //Connect char delay change
    connect(ui->lineEditCharDelay, &QLineEdit::textChanged, [=](QString string) {
       int value = string.toInt();
       if(value == 0 && ui->lineEditCharDelay->text().length() > 0) {
           ui->lineEditCharDelay->setStyleSheet("color:red");
       } else {
           ui->lineEditCharDelay->setStyleSheet("color:black");

       }
       if(value > 0) {
           charTimerDelay = value;
       }
    });

    //Connect word delay change
    connect(ui->lineEditWordDelay, &QLineEdit::textChanged, [=](QString string) {
       int value = string.toInt();
       if(value == 0 && ui->lineEditWordDelay->text().length() > 0) {
           ui->lineEditWordDelay->setStyleSheet("color:red");
       } else {
           ui->lineEditWordDelay->setStyleSheet("color:black");

       }
       if(value > 0) {
           wordTimerDelay = value;
           if(wordTimerOn) {//Reset the timer
               startPlay();
               startPlay();
           }
       }
    });
    //Connect blacken option
    connect(ui->checkBoxBlacken, &QCheckBox::clicked, [=](bool checked){
        flashWordOn = checked;
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
    inputEnabled = false;
    const QMimeData* mime = event->mimeData();
    if(mime->hasUrls())
    {
        //Get the URLs
        QList<QUrl> urls = mime->urls();
        //Manipulate the first URL as local path
        QString file = urls.at(0).toLocalFile();
        qDebug() << "emit MainWindow::DropEvent with file " << file;
        initTableColors();
        ui->labelWord->clear();
        emit fileDropped(file);
    }
}

void MainWindow::init(QVector<QVector<QChar>>& grid)
{
    //Init table
    QTableWidget* gridWidget = ui->tableWidgetGrid;
    int width = grid.length();
    int height = grid.first().length();
    currentGrid = grid;
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
    ui->treeWidgetWords->setSortingEnabled(false);

    //Connect automagical solve
    connect(ui->tableWidgetGrid, &QTableWidget::itemChanged, [=](QTableWidgetItem* item) {
        if(!inputOn && inputEnabled && item == ui->tableWidgetGrid->currentItem()) {
           qDebug() << "Changed to " << item->text();
           inputEnabled = false;
           ui->treeWidgetWords->selectionModel()->reset();
           ui->tableWidgetGrid->selectionModel()->reset();
           solveCurrent();
       }
    });
}

void MainWindow::update(QVector<QVector<QChar>>& grid, QVector<QVector<Word*>>& words) {
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
        QDebug debug = qDebug();
        for(auto y = 0; y < height; y++) {
            QChar current = QChar(grid.at(x).at(y));
            QTableWidgetItem* currentItem = gridWidget->item(x, y);
            currentItem->setText(QString(current));
            debug << current << ":" << currentItem->text() << " | ";
        }
    }
    gridWidget->viewport()->update();
    //Update word list
    QTreeWidget* wordWidget = ui->treeWidgetWords;
    ui->tableWidgetGrid->clearSelection();
    wordWidget->selectionModel()->reset();
    wordWidget->clear();
    //Create the top level items for each vector
    for(auto i = 0; i < words.length(); ++i)
    {
        //Create a topLevelItem "x letters"
        QVector<Word*> currentWords = words[i];
        QTreeWidgetItem* current = new QTreeWidgetItem();
        QString currentText = QString::number(currentWords.first()->length()) + QString(" letters");
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
    inputEnabled = true;
}

void MainWindow::inputToGrid() {
    //Stop playing words
    if(wordTimerOn) {
        startPlay();
    }
    //Stop showing chars
    if(charTimerOn) {
        killTimer(charTimerID);
        charTimerOn = false;
    }
    initTableColors();
    //Select the grid and start asking for input
    QTableWidget* table = ui->tableWidgetGrid;
    table->setCurrentCell(0, 0);
    ui->tableWidgetGrid->setFocus();
    inputX = 0;
    inputY = 0;
    inputOn = true;
}

bool MainWindow::nextCell() {
    QTableWidget* table = ui->tableWidgetGrid;
    if(inputX < 3) {
        inputX += 1;
    } else {
        if(inputY >= 3) {
            //End filling
            inputOn = false;
            inputX = 0;
            inputY = 0;
            solveCurrent();
            ui->tableWidgetGrid->selectionModel()->reset();
            changeTreeSelection(-1);
            ui->buttonNext->setFocus();
            return true;
        } else {
            inputX = 0;
            inputY += 1;
        }
    }
    table->setCurrentCell(inputY, inputX);
    ui->tableWidgetGrid->setFocus();
    return false;
}

void MainWindow::startPlay() {
    if(wordTimerOn) {
        killTimer(wordTimerID);
        lightFactor = 100;
        wordTimerOn = false;
        ui->buttonPlay->setText("Play");
    } else {
        changeTreeSelection(-1);
        wordTimerID = startTimer(wordTimerDelay);
        wordTimerOn = true;
        ui->buttonPlay->setText("Pause");
    }
}

bool MainWindow::changeTreeSelection(int direction) {
    lightFactor = 100;
    QTreeWidgetItem* selected = ui->treeWidgetWords->currentItem();
    QTreeWidgetItem* startFrom = ui->treeWidgetWords->itemBelow(selected);
    if(direction > 0) startFrom = ui->treeWidgetWords->itemAbove(selected);
    if(startFrom != NULL && startFrom->childCount() > 0) {
        if(direction > 0) {
            selected = ui->treeWidgetWords->itemAbove(selected);
        } else {
            selected = ui->treeWidgetWords->itemBelow(selected);
        }
    }
    if(selected == NULL){
        selected = ui->treeWidgetWords->topLevelItem(0);
    } else if(startFrom == NULL && !wordTimerOn){
        ui->treeWidgetWords->setCurrentItem(ui->treeWidgetWords->topLevelItem(0));
        selected = ui->treeWidgetWords->currentItem();
    }
    QTreeWidgetItem* nextItem = selected;
    if(direction > 0) {
        nextItem = ui->treeWidgetWords->itemAbove(selected);
    } else {
        nextItem = ui->treeWidgetWords->itemBelow(selected);
    }
    if(nextItem != NULL) {
        ui->treeWidgetWords->setCurrentItem(nextItem);
        ui->labelWord->setText(nextItem->text(0));
        return true;
    }
    return false;
}

void MainWindow::drawWord(Word* selected) {
    initTableColors();
    if(flashWordOn) flashWord(selected);
    ui->tableWidgetGrid->clearSelection();
    QString wordText = selected->getWordText();
    ui->labelWord->setText(selected->getWordText());
    QVector<QPair<int, int> > positions = selected->getPosition();
    currentIterator = positions.begin();
    currentWord = positions;
    charTimerID = startTimer(charTimerDelay);
    charTimerOn = true;
}

void MainWindow::drawNext() {
    QTableWidgetItem* current = ui->tableWidgetGrid->item(currentIterator->first, currentIterator->second);
    if(currentIterator != currentWord.end()) {
        QColor currentColor = defaultBG.lighter(lightFactor);
        //QColor currentText = defaultText.darker(lightFactor);
        current->setBackgroundColor(currentColor);
        current->setTextColor(defaultText);
        currentIterator++;
    } else {
        if(charTimerOn) {
            killTimer(charTimerID);
            charTimerOn = false;
            lightFactor = 100;
        }
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    int currentTimer = event->timerId();
    if(charTimerOn && currentTimer == charTimerID) {
        drawNext();
        lightFactor += 10;
    }
    if(wordTimerOn && currentTimer == wordTimerID) {
        if(!changeTreeSelection(-1)) {
            killTimer(wordTimerID);
            wordTimerOn = false;
            ui->buttonPlay->setText("Play");
        }
    }
}

void MainWindow::notFound() {
    ui->labelWord->setText("No solutions found");
    QTreeWidget* wordWidget = ui->treeWidgetWords;
    ui->tableWidgetGrid->clearSelection();
    wordWidget->selectionModel()->reset();
    wordWidget->clear();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if(event->type()==QEvent::KeyPress) {
        QKeyEvent* key = (QKeyEvent*)(event);
        QString input(key->text().toUpper());
        QString whitelist("ABCDEFGHIJKLMNOPQRSTUVWXYZÖÄÅ");//Allow ÖÄÅ
        if(inputOn) {
                if(input != "" && whitelist.contains(input, Qt::CaseInsensitive)) {
                    ushort unic = input.at(0).unicode();
                    QChar inChar(unic);
                    ui->tableWidgetGrid->item(inputY, inputX)->setText(inChar);
                    lastX = inputX;
                    lastY = inputY;
                    nextCell();
                    return true;
                }
                if(key->key() == Qt::Key_Escape) {
                    inputOn = false;
                    inputY = 0;
                    inputX = 0;
                    ui->tableWidgetGrid->setCurrentCell(0, 0);
                    //solveCurrent();
                    return true;
                }
                if(key->key() == Qt::Key_Backspace) {
                    if(inputX - 1 < 0) {
                        if(inputY > 0) {
                            inputY = inputY - 1;
                            inputX = 3;
                        }
                    } else {
                        inputX -= 1;
                    }
                    ui->tableWidgetGrid->item(inputY, inputX)->setText("");
                    ui->tableWidgetGrid->setCurrentCell(inputY, inputX);
                    return true;
                }
                return true;
            }
        if(ui->tableWidgetGrid->selectedItems().length() > 0) {
            QKeyEvent* key = static_cast<QKeyEvent*>(event);
            QString input(key->text());
            QString whitelist("ABCDEFGHIJKLMNOPQRSTUVWXYZÖÄÅ");//Allow ÖÄÅ
            if(input != "" && whitelist.contains(input, Qt::CaseInsensitive)) {
                ui->tableWidgetGrid->selectedItems().first()->setText(input);
                return true;
            }
            if(key->key() == Qt::Key_Delete || key->key() == Qt::Key_Backspace) {
                ui->tableWidgetGrid->selectedItems().first()->setText(QString(" "));
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::solveCurrent() {
    inputEnabled = false;
    qDebug() << "Solving current grid";
    int columns = ui->tableWidgetGrid->columnCount();
    int rows = ui->tableWidgetGrid->rowCount();
    //Update current grid
    QVector<QChar> temp;
    QVector<QVector<QChar> > result;
    for(auto x = 0; x < columns; x++) {
        for(auto y = 0; y < rows; y++) {
            QString currentText = ui->tableWidgetGrid->item(x, y)->text().toUpper();
            QChar current = currentText.at(0).unicode();
            temp.insert(y, current);
        }
        result.insert(x, temp);
        temp.clear();
    }
    currentGrid = result;
    inputEnabled = true;
    emit requestSolve(currentGrid);
}

void MainWindow::initTableColors() {
    int columns = ui->tableWidgetGrid->columnCount();
    int rows = ui->tableWidgetGrid->rowCount();
    //Remove old selections
    for(auto x = 0; x < columns; x++) {
        for(auto y = 0; y < rows; y++) {
            ui->tableWidgetGrid->item(x, y)->setBackgroundColor(QColor(Qt::white));
            ui->tableWidgetGrid->item(x, y)->setTextColor(QColor(Qt::black));
        }
    }
}

void MainWindow::flashWord(Word* word) {
    QVector<QPair<int, int> > letters = word->getPosition();
    int width = 4;
    int height = 4;
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height; y++) {
            ui->tableWidgetGrid->item(x, y)->setBackgroundColor(QColor(Qt::black));
        }
    }
    for(auto coord : letters) {
        int x = coord.first;
        int y = coord.second;
        ui->tableWidgetGrid->item(x, y)->setBackgroundColor(QColor(Qt::white));

    }
}
