#ifndef TERRAINOBJECT_H
#define TERRAINOBJECT_H

#include <QObject>
#include <QGLBuffer>
#include <QGLShaderProgram>

typedef enum {
    lod_0 = 0,
    lod_1 = 1,
    lod_2 = 20,
    lod_3 = 120,
} LODlevel;


// KLUDGE, because VAO is not supported in Qt 4.8.7
// FIXME someday
typedef void (APIENTRY *_glGenVertexArrays)(GLsizei, GLuint*);
typedef void (APIENTRY *_glBindVertexArray)(GLuint);
typedef void (APIENTRY *_glDeleteVertexArrays)(GLsizei, GLuint*);


class terrainObject : public QObject
{
    Q_OBJECT
public:
    terrainObject(QObject *parent = 0);

public:

    QString name;

    GLuint VertexArrayObject;

    QGLBuffer* vData;
    QGLBuffer* vIndices;
    uint vCount;

    GLuint textureBuffer;


    LODlevel lodLevel;

    uint row,
         col;
    QVector3D position;

};

#endif // TERRAINOBJECT_H
