#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include <QObject>
#include <QVector3D>
#include <QGLBuffer>
#include <QGLShaderProgram>

// KLUDGE, because VAO is not supported in Qt 4.8.7
// FIXME someday
typedef void (APIENTRY *_glGenVertexArrays)(GLsizei, GLuint*);
typedef void (APIENTRY *_glBindVertexArray)(GLuint);
typedef void (APIENTRY *_glDeleteVertexArrays)(GLsizei, GLuint*);


class genericObject : public QObject
{
    Q_OBJECT
public:
    explicit genericObject(QObject *parent = 0);
    ~genericObject();

    void Draw();

public:
    QString name;

    QVector3D translation,
              rotation,
              scale;

    uint polygonType;

    GLuint VertexArrayObject;

    QGLBuffer* vData;
    QGLBuffer* vIndices;
    uint vCount;

    QGLShaderProgram *shader;

    GLuint textureBuffer;

    QMatrix4x4 transformMatrix;
    float objectTime;

    // see typedef above
    _glBindVertexArray glBindVertexArray = NULL;

private:

};

#endif // OBJECT_CLASS_H
