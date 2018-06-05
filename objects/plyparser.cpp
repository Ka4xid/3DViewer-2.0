#include "plyparser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegExp>
#include <QTextStream>
#include <QStringList>

#define LOG false


PLYparser::PLYparser(QObject *parent) : QObject(parent)
{

}

void PLYparser::start(QString modelFilePath)
{
    QFile modelFile;


    QString propertyPattern("([-]?[0-9.]*)[ \n\r]?");

    QVector<float> verticesData;
    QVector<uint> verticesIndices;

    QRegExp RegExpData;

    uint vertex_counter = 0,
         face_counter = 0,
         properties_counter = 0;

    bool normals_enabled = false,
         uv_enabled = false;




    modelFile.setFileName(modelFilePath);

    if ( !modelFile.open(QIODevice::ReadOnly) ) {
        qDebug() << "No model file!";
        this->deleteLater();
        return;
    }


    /*
    // One regex to rule them all
    QRegExp RegExpData("([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})");
    QRegExp RegExpIndices("[3][ ]([0-9]*)[ ]([0-9]*)[ ]([0-9]*)");       // Accept only triangulated models
    */

    QTextStream Stream;
    QString line;
    Stream.setDevice(&modelFile);
    line = Stream.readLine();
    if (line != "ply") {
        qDebug() << "Wrong file format!";
        this->deleteLater();
        return;
    }



    /* READING HEADER */
    QString regExpDataStr;

    while ( !line.contains(QString("end_header"), Qt::CaseInsensitive) ) {

        line = Stream.readLine();

        /* How many vertices */
        if ( line.contains("element vertex") ) {
            QRegExp rx("element vertex ([0-9]*)");
            rx.exactMatch(line);
            vertex_counter = rx.cap(1).toUInt();
        }

        /* How many faces */
        if ( line.contains("element face") ) {
            QRegExp rx("element face ([0-9]*)");
            rx.exactMatch(line);
            face_counter = rx.cap(1).toUInt();
        }

        /* How many properties */
        if ( line.contains(QRegExp("property float ..?")) )
        {
            if ( line.contains(QRegExp(" n.")) )
                normals_enabled = true;
            if ( line.contains(QRegExp(" [s|t]")) )
                uv_enabled = true;

            regExpDataStr.append(propertyPattern);

            properties_counter++;
        }

    }

    RegExpData.setPattern(regExpDataStr);


    while ( vertex_counter != 0 ) {
        line = Stream.readLine();

        RegExpData.exactMatch(line);

        uint counter = 1;

        /* COORDS */
        for (uint n=0; n < 3; n++) {
            verticesData.append( RegExpData.cap(counter+n).toFloat() );
        }

        counter = 4;

        /* NORMALS */
        if (normals_enabled) {
            for (uint n=0; n < 3; n++) {
                verticesData.append( RegExpData.cap(counter+n).toFloat() );
            }
            counter = 7;
        } else {
            verticesData.append(0);
            verticesData.append(0);
            verticesData.append(0);
        }

        /* UV */
        if (uv_enabled) {
            for (uint n=0; n < 2; n++) {
                verticesData.append( RegExpData.cap(counter+n).toFloat() );
            }
        } else {
            verticesData.append(0);
            verticesData.append(0);
        }

        vertex_counter--;
    }


    QRegExp TriangleRegExp("3 ([0-9]*) ([0-9]*) ([0-9]*)");
    QRegExp QuadRegExp("4 ([0-9]*) ([0-9]*) ([0-9]*) ([0-9]*)");

    while( face_counter != 0 ) {
        line = Stream.readLine();

        if (TriangleRegExp.exactMatch(line)) {
            verticesIndices.append( TriangleRegExp.cap(1).toUInt() );
            verticesIndices.append( TriangleRegExp.cap(2).toUInt() );
            verticesIndices.append( TriangleRegExp.cap(3).toUInt() );
        }

        if (QuadRegExp.exactMatch(line)) {
            verticesIndices.append( QuadRegExp.cap(1).toUInt() );
            verticesIndices.append( QuadRegExp.cap(2).toUInt() );
            verticesIndices.append( QuadRegExp.cap(3).toUInt() );

            verticesIndices.append( QuadRegExp.cap(1).toUInt() );
            verticesIndices.append( QuadRegExp.cap(3).toUInt() );
            verticesIndices.append( QuadRegExp.cap(4).toUInt() );
        }


        face_counter--;
    }


        /*
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

    }*/

    modelFile.close();


    emit sendMeshData(verticesData, verticesIndices);
    this->deleteLater();
}
