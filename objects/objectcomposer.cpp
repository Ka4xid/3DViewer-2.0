#include "objectcomposer.h"
#include <QFileInfo>

ObjectComposer::ObjectComposer(QObject *parent) : QObject(parent)
{
    name = "default";
    texturePath = ":/checker.png";
    shaderPath = ":/shaders/default";
    translation = QVector3D(0,0,0);
    rotation = QVector3D(0,0,0);
    scale = QVector3D(1,1,1);

    polygonType = 4;
}

ObjectComposer* ObjectComposer::setName(QString newName)
{
    name = newName;
    return this;
}
ObjectComposer* ObjectComposer::setTranslation(QVector3D newTranslation)
{
    translation = newTranslation;
    return this;
}
ObjectComposer* ObjectComposer::setRotation(QVector3D newRotation)
{
    rotation = newRotation;
    return this;
}
ObjectComposer* ObjectComposer::setScale(QVector3D newScale)
{
    scale = newScale;
    return this;
}
ObjectComposer* ObjectComposer::setTexturePath(QString newTexturePath)
{
    QFileInfo file(newTexturePath);
    file.exists() ? texturePath = newTexturePath : 0;

    return this;
}
ObjectComposer* ObjectComposer::setShaderPath(QString newShaderPath)
{
    QFileInfo file(newShaderPath);
    file.exists() ? shaderPath = newShaderPath : 0;
    return this;
}
ObjectComposer* ObjectComposer::setModelFilePath(QString newFilePath)
{
    QFileInfo file(newFilePath);
    file.exists() ? filePath = newFilePath : 0;
    return this;
}
ObjectComposer* ObjectComposer::setPolygonType(uint newType)
{
    polygonType = newType;
    return this;
}

void ObjectComposer::Done()
{
    emit CreateObjectWithData(name,
                              translation,
                              rotation,
                              scale,
                              polygonType,
                              texturePath,
                              shaderPath,
                              filePath);
    this->deleteLater();
}

