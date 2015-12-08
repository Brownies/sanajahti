#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDebug>
#include <QVector>
#include <QTextStream>
#include <locale.h>
#include "imagereader.h"
#include <QString>
#include <QFile>
#include <QImage>
#include <QPixmap>
#include <QDir>

ImageReader::ImageReader(QObject *parent) : QObject(parent)
{
    qDebug() << "Init ImageReader";
}

/*Returns true if the filePath exists and can be read with Tesseract
* Fills the Grid given as a reference
*
* Scales the given screenshot and crops all the letters into separate pixmaps.
* (Works only with pictures with the same hight/width-ratio than the ones in the src file)
* Goes through the vector "letterList" (with the pixmaps) and "reads" the letters one at
* a time and adds them to the result-string.
* After that the letters are added to the grid.
* Tesseract does not recognize Ä/Ö?
*/
bool ImageReader::initData(QString filePath, QVector<QVector<QChar>>& grid)
{    
    //Create a vector for storing all 16 letters
    QVector<QPixmap> letterList;
    //Rescale the given image
    QImage img(filePath);
    QPixmap pm;


    int letterSize = 0;
    int leftPadding = 0;
    int rightPadding = 0;
    int topPadding = 0;
    int bottomPadding = 0;

    //for 3:4 aspect ratio, letters are 222x222 at 1536x2048

    pm = pm.fromImage(img.scaled(1536, 2048, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    letterSize = 222;
    leftPadding = 222;
    rightPadding = 67;
    topPadding = 775;
    bottomPadding = 67;


    //for-loop for cropping all letters from the image one by one
    // if the height/width-ratio is different, it won't crop letters properly
    // checking the ratio beforehand and making separate loops for different ratios could help?
    for(unsigned int i = 0; i < 16; i++){
        //Create a rectangle for cropping each letter
        //QRect box(55+(i%4)*74, 199+(i/4)*73, 50, 50);
        QRect box((leftPadding + (i%4) * (rightPadding + letterSize)),
                  (topPadding + (i/4) * (letterSize + bottomPadding)), letterSize, letterSize);
        QPixmap letter = pm.copy(box);
        //qDebug() << "x: " << leftPadding + (i%4) * (rightPadding + letterSize);
        //qDebug() << "y: " << topPadding + (i/4) * (letterSize + bottomPadding);
        //Add the pixmap of a letter to the vector for letters
        letterList.append(letter);
    }


    //qDebug() << "ImageReader::initData(" << filePath << ", " << grid << ") - NOT IMPLEMENTED";

    /* OCR
     * To get the needed file:
     * wget https://tesseract-ocr.googlecode.com/files/eng.traineddata.gz
     * gunzip eng.traineddata.gz
     * sudo mv -v eng.traineddata /usr/share/tesseract-ocr/tessdata/
     *
     */

    //variable for storing the data from the cropped letter images
    QString outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize tesseract-ocr with English, setlocale to prevent crash from not finding tesseract (fix that?)
    setlocale (LC_NUMERIC, "C");
    if (api->Init("/usr/share/tesseract-ocr/", "deu")) {
        qDebug() << "Could not initialize tesseract.";
        return false;
    }

    // Mode in which tesseract tries to recognize only on character
    api->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    // List of the letters tesseract will try to find (Ä and Ö could be added to the list
    // but it won't recognize them anyway)
    api->SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖ");

    //string for the final grid
    QString result;
    Pix *image;
    //for-loop for reading the letters
    for(unsigned int t = 0; t < 16; t++){
        //Save the letter as .png
        QString newFolder = "../src/OCRtemps/";
        QString newFile = "../src/OCRtemps/templetter.png";
        if(!QDir(newFolder).exists()) {
            QDir().mkpath(newFolder);
        }
        QFile file(newFile);
        file.open(QIODevice::WriteOnly);
        letterList[t].save(newFile, "png");
        file.close();
        // Open input image with leptonica library
        image = pixRead(newFile.toUtf8().constData());

        image = pixContrastTRC(image, image, 0.8f);
        image = pixScaleLI(image, 2, 2);

        api->SetImage(image);
        outText = api->GetUTF8Text();
        QString readChar = outText.simplified().replace(" ", "");
        if (readChar == "") readChar = "I";
        result += readChar;
        //qDebug() << result;
    }

    //qDebug() << "OCR from image:";
    //qDebug() << outText;
    qDebug() << "OCR result:";
    qDebug() << result;

    int i = 0;
    //Save OCR-data in grid - HARDCODED 4X4, fix with a general purpose approach

    for(int x = 0; x < 4; x++) {
        for(int y = 0; y < 4; y++) {
            QChar current = (result.at(i).unicode());
            grid[x][y] = current;
            i++;
        }
    }
    qDebug() << "OCR data saved to grid";
    // Destroy used object and release memory
    api->End();
    pixDestroy(&image);
    qDebug() << "returning from OCR";
    return true;
}
