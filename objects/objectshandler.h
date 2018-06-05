#ifndef OBJECTSHANDLER_H
#define OBJECTSHANDLER_H

#include <QObject>
#include <QVector3D>
#include "objects/genericobject.h"
#include "objects/objectbuilder.h"
#include "objects/trajectorybuilder.h"

class ObjectsHandler : public QObject
{
    Q_OBJECT
public:
    explicit ObjectsHandler(QGLWidget* widget, QObject *parent = 0);



private:
    QGLWidget* contextWidget;

    QMap<QString, genericObject*> ObjectsArray;

    ObjectBuilder* objBuilder;
    TrajectoryBuilder* trajBuilder;

    // Kludge, see genericobject header
    _glDeleteVertexArrays glDeleteVertexArrays = NULL;



public slots:

    void ReceiveNewObject(genericObject* newObject);

    void CreateObject(QString name,
                      QVector3D newPosition,
                      QVector3D newRotation,
                      QVector3D newScale,
                      uint newDrawType,
                      QString newTexturePath,
                      QString newShaderPath,
                      QString newFilePath );

    void TranslateObject(QString name, QVector3D newPosition);
    QVector3D getObjectTranslation(QString name);
    void RotateObject(QString name, QVector3D newRotation);
    QVector3D getObjectRotation(QString name);
    void ScaleObject(QString name, QVector3D newScale);
    QVector3D getObjectScale(QString name);
    void MoveObject(QString name, QVector3D direction);
    void DeleteObject(QString name);

    void CreateTrajectory(QString name, QVector<QVector3D> points);

    void Render();


};

#endif // OBJECTSHANDLER_H
