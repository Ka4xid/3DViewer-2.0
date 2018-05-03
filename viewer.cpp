#include "viewer.h"
#include "objects/objectcomposer.h"
#include "gsslink.h"

ViewerClass::ViewerClass(QObject *parent, QWidget *sceneParent) : QObject(parent)
{
    gssLink();

    if (!(sceneParent == 0)) {
        scene = new Scene(sceneParent);
    } else {
        scene->show();
    }
    scene->setGeometry(0, 0, sceneParent->width(), sceneParent->size().height());

    qRegisterMetaType< QVector<uint> >("QVector<QVector3D>");
    qRegisterMetaType< QVector<float> >("QVector<float>");
    qRegisterMetaType< QVector<uint> >("QVector<uint>");
    qRegisterMetaType< ChunkData >("ChunkData");


    terrHandlerThread = new QThread(this);
    terrHandler = new TerrainHandler(scene);
    //terrHandler->moveToThread(terrHandlerThread);

    connect(this, SIGNAL(terrainCreate(QString,QString)),
            terrHandler, SLOT(GenerateTerrain(QString,QString)));
    connect(scene, SIGNAL(NewCameraPos(QVector3D*)),
            terrHandler, SLOT(SetCameraPosPointer(QVector3D*)));

    //terrHandlerThread->start();

    objHandlerThread = new QThread(this);
    objHandler = new ObjectsHandler(scene);
    //objHandler->moveToThread(objHandlerThread);

    connect(this, SIGNAL(objectTranslate(QString,QVector3D)),
            objHandler, SLOT(TranslateObject(QString,QVector3D)));
    connect(this, SIGNAL(objectRotate(QString,QVector3D)),
            objHandler, SLOT(RotateObject(QString,QVector3D)));
    connect(this, SIGNAL(objectScale(QString,QVector3D)),
            objHandler, SLOT(ScaleObject(QString,QVector3D)));
    connect(this, SIGNAL(objectMove(QString,QVector3D)),
            objHandler, SLOT(MoveObject(QString,QVector3D)));
    connect(this, SIGNAL(objectDelete(QString)),
            objHandler, SLOT(DeleteObject(QString)));
    connect(this, SIGNAL(trajectoryCreate(QString,QVector<QVector3D>)),
            objHandler, SLOT(CreateTrajectory(QString,QVector<QVector3D>)));

    //objHandlerThread->start();.


    connect(scene, SIGNAL(RenderSignal()),
            objHandler, SLOT(Render()), Qt::DirectConnection);
    connect(scene, SIGNAL(RenderSignal()),
            terrHandler, SLOT(Render()), Qt::DirectConnection);


    scene->setCamera(new CameraClass);

}

ViewerClass::~ViewerClass()
{
    terrHandlerThread->quit();
    objHandlerThread->quit();
}

Scene *ViewerClass::getScenePointer()
{
    return scene;
}

void ViewerClass::SetCameraPosition(QVector3D newPosition)
{
    scene->getCamera()->cameraPosition = newPosition;
}

ObjectComposer* ViewerClass::CreateObject()
{
    objComposer = new ObjectComposer;

    connect(objComposer, SIGNAL(CreateObjectWithData(QString,QVector3D,QVector3D,QVector3D,uint,QString,QString,QString)),
            objHandler, SLOT(CreateObject(QString,QVector3D,QVector3D,QVector3D,uint,QString,QString,QString)) );

    return objComposer;
}
void ViewerClass::DeleteObject(QString name)
{
    emit objectDelete(name);
}

void ViewerClass::TranslateObject(QString name, QVector3D newPosition)
{
    emit objectTranslate(name, newPosition);
}
void ViewerClass::RotateObject(QString name, QVector3D newRotation)
{
    emit objectRotate(name, newRotation);
}
void ViewerClass::ScaleObject(QString name, QVector3D newScale)
{
    emit objectScale(name, newScale);
}
void ViewerClass::MoveObjectByDir(QString name, QVector3D direction)
{
    emit objectMove(name, direction);
}

void ViewerClass::CreateTrajectory(QString name, QVector<QVector3D> points)
{
    emit trajectoryCreate(name, points);
}

void ViewerClass::generateTerrain(QString mtwFilePath, QString mapFilePath)
{
    emit terrainCreate(mtwFilePath, mapFilePath);
}
