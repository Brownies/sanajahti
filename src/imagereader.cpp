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
bool ImageReader::initData(QString filePath, QVector<QVector<char>>& grid)
{    
    //Create a vector for storing all 16 letters
    QVector<QPixmap> letterList;
    //Rescale the given image
    QImage img(filePath);
    QPixmap pm;
    pm = pm.fromImage(img.scaled(384, 512, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));


    //for-loop for cropping all letters from the image one by one
    // if the height/width-ratio is different, it won't crop letters properly
    // checking the ratio beforehand and making separate loops for different ratios could help?
    for(unsigned int i = 0; i < 16; i++){
        //Create a rectangle for cropping each letter
        QRect box(55+(i%4)*74, 199+(i/4)*73, 50, 50);
        QPixmap letter = pm.copy(box);
        //Add the pixmap of a letter to the vector for letters
        letterList.append(letter);
    }

    //The file path where the letters will be saved before ocr-handling
    QString newFile = "../src/OCRtemps/templetter.png";

    qDebug() << "ImageReader::initData(" << filePath << ", " << grid << ") - NOT IMPLEMENTED";

    /* OCR
     * To get the needed file:
     * wget https://tesseract-ocr.googlecode.com/files/eng.traineddata.gz
     * gunzip eng.traineddata.gz
     * sudo mv -v eng.traineddata /usr/share/tesseract-ocr/tessdata/
     *
     */

    //variable for storing the data from the cropped letter images
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize tesseract-ocr with English, setlocale to prevent crash from not finding tesseract (fix that?)
    setlocale (LC_NUMERIC, "C");
    if (api->Init("/usr/share/tesseract-ocr/", "eng")) {
        qDebug() << "Could not initialize tesseract.";
        return false;
    }

    // Mode in which tesseract tries to recognize only on character
    api->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    // List of the letters tesseract will try to find (Ä and Ö could be added to the list
    // but it won't recognize them anyway)
    api->SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    //string for the final grid
    QString result;
    Pix *image;
    //for-loop for reading the letters
    for(unsigned int t = 0; t < 16; t++){
        //Save the letter as .png
        QFile file(newFile);
        if(!file.exists()){
            //the file did not exist
        } else {
            file.open(QIODevice::WriteOnly);
            letterList[t].save(newFile, "png");
        }
        file.close();
        // Open input image with leptonica library
        image = pixRead(newFile.toUtf8().constData());
        api->SetImage(image);
        outText = api->GetUTF8Text();
        result += QString(outText).simplified().replace(" ", "");
        qDebug() << result;
    }

    qDebug() << "OCR from image:";
    qDebug() << outText;
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
