#ifndef OBJECTCOMPOSER_H
#define OBJECTCOMPOSER_H

#include <QObject>
#include <QVector3D>


class ObjectComposer : public QObject
{
    Q_OBJECT
public:
    explicit ObjectComposer(QObject *parent = 0);

    ObjectComposer* setName(QString newName);
    ObjectComposer* setTranslation(QVector3D newTranslation);
    ObjectComposer* setRotation(QVector3D newRotation);
    ObjectComposer* setScale(QVector3D newScale);
    ObjectComposer* setTexturePath(QString newTexturePath);
    ObjectComposer* setShaderPath(QString newShaderPath);
    ObjectComposer* setModelFilePath(QString newFilePath);
    ObjectComposer* setPolygonType(uint newType);
    void Done();


private:
    QString name,
            texturePath,
            shaderPath,
            filePath;

    QVector3D translation,
              rotation,
              scale;

    uint polygonType;

signals:
    void CreateObjectWithData(QString name,
                              QVector3D newPosition,
                              QVector3D newRotation,
                              QVector3D newScale,
                              uint newDrawType,
                              QString newTexturePath,
                              QString newShaderPath,
                              QString newFilePath );
};

#endif // OBJECTCOMPOSER_H
