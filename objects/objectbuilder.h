#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include <QObject>
#include <QVector3D>
#include <QGLWidget>
#include "objects/genericobject.h"
#include <QGLFunctions>

// We need to get QGLWidget, to implement functions, not defined in QGLFinctions
// Consider it as a "temporary" kludge

class ObjectBuilder : public QObject, private QGLFunctions
{
    Q_OBJECT
public:
    explicit ObjectBuilder(QGLWidget* widget, QObject *parent = 0);

    ObjectBuilder* setName(QString name);
    ObjectBuilder* setPosition(QVector3D newPosition);
    ObjectBuilder* setRotation(QVector3D newRotation);
    ObjectBuilder* setScale(QVector3D newScale);
    ObjectBuilder* setDrawType(uint newDrawType);
    ObjectBuilder* setTexturePath(QString newTexturePath);
    ObjectBuilder* setShaderPath(QString newShaderPath);
    ObjectBuilder* setFileToLoad(QString newFilePath);

    void Build();

private:

    void CompileShader();

    void BindTexture();

    void LoadMeshFromFile();

private:

    QGLWidget* contextWidget;

    genericObject* newObject;

    QString texturePath;
    QString shaderPath;
    QString modelFilePath;

    _glGenVertexArrays glGenVertexArrays = NULL;
    _glBindVertexArray glBindVertexArray = NULL;


signals:
    void ObjectReady(genericObject* newObject);

private slots:
    void receiveMeshData(QVector<float> pointsData,
                         QVector<uint> pointsIndices);
};

#endif // OBJECTBUILDER_H
