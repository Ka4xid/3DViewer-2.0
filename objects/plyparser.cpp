#include "plyparser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegExp>
#include <QTextStream>


PLYparser::PLYparser(QObject *parent) : QObject(parent)
{

}

void PLYparser::start(QString modelFilePath)
{
    QFile modelFile;

    QVector<float> verticesData;
    QVector<uint> verticesIndices;



    modelFile.setFileName(modelFilePath);

    if ( !modelFile.open(QIODevice::ReadOnly) ) {
        qDebug() << "No such file!";
        return;
    }


    // One regex to rule them all
    QRegExp RegExpData("([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})");
    QRegExp RegExpIndices("[3][ ]([0-9]*)[ ]([0-9]*)[ ]([0-9]*)");       // Accept only triangulated models


    QTextStream Stream;
    QString line;
    Stream.setDevice(&modelFile);
    line = Stream.readLine();
    if (line != "ply") {
        qDebug() << "Wrong file format!";
        return;
    }

    while( (!(Stream.atEnd()))) {
        line = Stream.readLine();

        if (RegExpData.exactMatch(line)) {
            for (int n=1; n<=8; n++) {
                // REMEMBER TO SCALE -1 MODEL IN YOUR EDITOR
                verticesData.append( RegExpData.cap(n).toFloat() );
            }
        }

        if (RegExpIndices.exactMatch(line)) {
            for (int n=1; n<=3; n++) {
                verticesIndices.append( RegExpIndices.cap(n).toUInt() );
            }
        }

    }
    modelFile.close();

    emit sendMeshData(verticesData, verticesIndices);
    this->deleteLater();
}
