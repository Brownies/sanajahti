#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <QObject>

class ImageReader : public QObject
{
    Q_OBJECT
public:
    explicit ImageReader(QObject *parent = 0);
    bool initData(QString filePath, QVector<QVector<char> > &grid);

signals:

public slots:
};

#endif // IMAGEREADER_H
