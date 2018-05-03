#ifndef PLYPARSER_H
#define PLYPARSER_H

#include <QObject>
#include <QVector>


class PLYparser : public QObject
{
    Q_OBJECT
public:
    explicit PLYparser(QObject *parent = 0);

    void start(QString modelFilePath);


signals:
    void sendMeshData(QVector<float> vData,
                      QVector<uint> vIndices);
};

#endif // PLYPARSER_H
