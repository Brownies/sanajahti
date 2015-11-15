#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDebug>
#include <QVector>
#include <QTextStream>
#include <locale.h>
#include "imagereader.h"

ImageReader::ImageReader(QObject *parent) : QObject(parent)
{
    qDebug() << "Init ImageReader";
}

//Returns true if the filePath exists and can be read with Tesseract
//Fills the Grid given as a reference
bool ImageReader::initData(QString filePath, QVector<QVector<char>>& grid)
{
    qDebug() << "ImageReader::initData(" << filePath << ", " << grid << ") - NOT IMPLEMENTED";

    //OCR - BROKEN BUT YOU GET THE IDEA
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, setlocale to prevent crash from not finding tesseract (fix that?)
    setlocale (LC_NUMERIC, "C");
    if (api->Init(NULL, "eng")) {
        qDebug() << "Could not initialize tesseract.";
        return false;
    }
    // Open input image with leptonica library
    Pix *image = pixRead(filePath.toUtf8().constData());
    api->SetImage(image);
    // Get OCR result
    outText = api->GetUTF8Text();
    qDebug() << "OCR from image:";
    qDebug() << outText;
    QString result = QString(outText).simplified().replace(" ", "");
    qDebug() << "Parsed to QString:";
    qDebug() << result;
    int i = 0;
    //Save OCR-data in grid - HARDCODED 4X4, fix with a general purpose approach
    for(int x = 0; x < 4; x++) {
        for(int y = 0; y < 4; y++) {
            QChar current = result[i];
            char currentChar = current.toLatin1();
            grid[x][y] = currentChar;
            i++;
        }
    }
    // Destroy used object and release memory
    api->End();
    delete [] outText;
    pixDestroy(&image);
    return true;
}
