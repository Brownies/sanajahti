#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "imagereader.h"
#include <QDebug>
#include <QVector>

ImageReader::ImageReader(QObject *parent) : QObject(parent)
{
    qDebug() << "Init ImageReader";
}

//Returns true if the filePath exists and can be read with Tesseract
//Fills the Grid given as a reference
bool ImageReader::initData(QString filePath, QVector<QVector<char*>>& grid)
{
    qDebug() << "ImageReader::initData(" << filePath << ", " << grid << ") - NOT IMPLEMENTED";
    /*
    //OCR - BROKEN BUT YOU GET THE IDEA
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path

    if (api->Init(NULL, "eng")) { //Crashes at the moment - it can't find the language files or something, maybe do it locally..?
        qDebug() << "Could not initialize tesseract.";
        return false;
    }
    // Open input image with leptonica library
    Pix *image = pixRead(filePath.toUtf8().constData());
    api->SetImage(image);
    // Get OCR result
    outText = api->GetUTF8Text();
    printf("OCR output:\n%s", outText);

    // Destroy used object and release memory
    api->End();
    delete [] outText;
    pixDestroy(&image);
    return true;
    */
    return false;
}
