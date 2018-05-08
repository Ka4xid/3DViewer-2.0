#include "cameraclass.h"
#include <QDebug>

CameraClass::CameraClass(QGLWidget *parent) : QObject(parent)
{
    setParent(parent);

    orbitDistance = 15000;
}

void CameraClass::Process()
{
    glTranslatef(0, 0, -orbitDistance);

    glRotatef(cameraRotation.y(), 0, 1, 0);
    glRotatef(cameraRotation.z(), 0, 0, 1);

    glTranslatef(-cameraPosition.x(),
                 -cameraPosition.y(),
                 -cameraPosition.z() );
}

void CameraClass::Move(QVector3D moveBy)
{
    cameraPosition += moveBy;
}

void CameraClass::Rotate(QVector3D rotateBy)
{
    if ((cameraRotation.y() >= 0) && (cameraRotation.y() <= 180)) {
        cameraRotation += QVector3D(0, rotateBy.y(), 0);
    }
    cameraRotation += QVector3D(0, 0, rotateBy.z());

    if (cameraRotation.y() < 0)
        cameraRotation.setY(0);
    if (cameraRotation.y() > 180)
        cameraRotation.setY(180);

    if (cameraRotation.z() < 0)
        cameraRotation.setZ(360);
    if (cameraRotation.z() > 360)
        cameraRotation.setZ(0);
}

void CameraClass::ProcessMouseInput(QPointF mouseDelta)
{
    float pitch,
          yaw;

    pitch = mouseDelta.y() * 0.2;
    yaw = mouseDelta.x() * 0.2;

    this->Rotate(QVector3D(0, pitch, yaw));
}

void CameraClass::ProcessMouseWheelInput(float wheelDelta)
{
    if ( (orbitDistance >= 0) && (orbitDistance <= 20000) ) {
        orbitDistance -= wheelDelta * (1 + orbitDistance/4);
    }

    if (orbitDistance > 20000)
        orbitDistance = 20000;
    if (orbitDistance < 0)
        orbitDistance = 0;
}

QVector3D* CameraClass::GetCameraPos()
{
    return &cameraPosition;
}
