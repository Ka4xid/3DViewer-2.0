#ifndef VIEWERCLASS_H
#define VIEWERCLASS_H

#include <QObject>
#include <QThread>
#include "GL/scene.h"
#include "terrain/terrainhandler.h"
#include "objects/objectshandler.h"
#include "objects/objectcomposer.h"


class ViewerClass : public QObject
{
    Q_OBJECT
public:
    explicit ViewerClass(QObject *parent = 0, QWidget* sceneParent = 0);
    ~ViewerClass();

    Scene* getScenePointer();

    void generateTerrain(QString mtwFilePath, QString mapFilePath);


    ObjectComposer* CreateObject();
    void DeleteObject(QString name);
    void TranslateObject(QString name, QVector3D newPosition);
    void RotateObject(QString name, QVector3D newRotation);
    void ScaleObject(QString name, QVector3D newScale);
    void MoveObjectByDir(QString name, QVector3D direction);

    void CreateTrajectory(QString name, QVector<QVector3D> points);

    void SetCameraPosition(QVector3D newPosition);


private:

    Scene* scene;
    TerrainHandler* terrHandler;
    QThread* terrHandlerThread;
    ObjectsHandler* objHandler;
    QThread* objHandlerThread;

    ObjectComposer* objComposer;


signals:
    void objectTranslate(QString name, QVector3D newPosition);
    void objectRotate(QString name, QVector3D newRotation);
    void objectScale(QString name, QVector3D newScale);
    void objectMove(QString name, QVector3D direction);
    void objectDelete(QString name);

    void trajectoryCreate(QString name, QVector<QVector3D> points);

    void terrainCreate(QString mtwFilePath, QString mapFilePath);



public slots:
};

#endif // VIEWERCLASS_H
