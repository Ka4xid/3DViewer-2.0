#ifndef GLVIEWER_H
#define GLVIEWER_H

#include <QObject>
#include <QGLWidget>
#include <QGLFunctions>
#include <QKeyEvent>
#include <QMap>
#include "GL/cameraclass.h"
#include "objects/genericobject.h"
#include "objects/objectshandler.h"

class Scene : public QGLWidget, private QGLFunctions
{
    Q_OBJECT

public:

    explicit Scene(QWidget *parent = 0);

    void setCamera(CameraClass* newCamera);
    CameraClass* getCamera();

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    void (ObjectsHandler::*renderFuncPointer)();

private:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();


    void SetPerspective(float fov, float Znear, float Zfar, float aspect);

    void renderTestAxis(float m);

    void fetchGLerrors();

    void handleInputs();


private:
    CameraClass* camera;

    QVector< QVector<genericObject*>* > RenderQueues;

    bool wireFrameMode;

    struct inputData {
        QMap<int, int> keyMap;
        int mouseWheelDelta;
        QPointF mouseMoveDelta;
        QPointF mouseMovePrev;
    } UserInput;



signals:
    void RenderSignal();
    void NewCameraPos(QVector3D* newPos);

public slots:

};

#endif // GLVIEWER_H
