#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include <QObject>
#include <QGLWidget>
#include <QMatrix4x4>


class CameraClass : public QObject
{
    Q_OBJECT

public:
    explicit CameraClass(QGLWidget* parent = 0);

    void Process();

    void Move(QVector3D moveBy);
    void Rotate(QVector3D rotateBy);
    void ProcessMouseInput(QPointF mouseDelta);
    void ProcessMouseWheelInput(float wheelDelta);

    QVector3D* GetCameraPos();


public:
    QVector3D cameraPosition;
    QVector3D cameraRotation;
    float orbitDistance;

signals:

public slots:
};

#endif // CAMERACLASS_H
