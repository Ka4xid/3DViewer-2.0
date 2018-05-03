#include "objectshandler.h"
#include <qmath.h>
#include <QDebug>

ObjectsHandler::ObjectsHandler(QGLWidget *widget, QObject *parent) : QObject(parent)
{
    contextWidget = widget;

    glDeleteVertexArrays = (_glDeleteVertexArrays)contextWidget->context()->getProcAddress("glDeleteVertexArrays");
}



void ObjectsHandler::ReceiveNewObject(genericObject *newObject)
{
    ObjectsArray.append(newObject);
}

void ObjectsHandler::Render()
{
    foreach (genericObject* object, ObjectsArray) {
        glPushMatrix();
        object->Draw();
        glPopMatrix();
    }
}

void ObjectsHandler::CreateObject(QString name,
                                  QVector3D newTranslation,
                                  QVector3D newRotation,
                                  QVector3D newScale,
                                  uint newDrawType,
                                  QString newTexturePath,
                                  QString newShaderPath,
                                  QString newFilePath)
{
    objBuilder = new ObjectBuilder(contextWidget);

    connect(objBuilder, SIGNAL(ObjectReady(genericObject*)),
            this, SLOT(ReceiveNewObject(genericObject*)) );

    objBuilder->setName(name)
            ->setPosition(newTranslation)
            ->setRotation(newRotation)
            ->setScale(newScale)
            ->setDrawType(newDrawType)
            ->setTexturePath(newTexturePath)
            ->setShaderPath(newShaderPath)
            ->setFileToLoad(newFilePath)
            ->Build();
}
void ObjectsHandler::TranslateObject(QString name, QVector3D newTranslation)
{
    foreach (genericObject* object, ObjectsArray) {
        if (object->name == name) {
            object->translation = newTranslation;
            break;
        }
    }
}
void ObjectsHandler::RotateObject(QString name, QVector3D newRotation)
{
    foreach (genericObject* object, ObjectsArray) {
        if (object->name == name) {
            object->rotation = newRotation;
            break;
        }
    }
}
void ObjectsHandler::ScaleObject(QString name, QVector3D newScale)
{
    foreach (genericObject* object, ObjectsArray) {
        if (object->name == name) {
            object->scale = newScale;
            break;
        }
    }
}
QVector3D ObjectsHandler::getObjectTranslation(QString name)
{
    foreach (genericObject* object, ObjectsArray) {
        if (object->name == name) {
            return object->translation;
        }
    }
    return QVector3D();
}
QVector3D ObjectsHandler::getObjectRotation(QString name)
{
    foreach (genericObject* object, ObjectsArray) {
        if (object->name == name) {
            return object->rotation;
        }
    }
    return QVector3D();
}
QVector3D ObjectsHandler::getObjectScale(QString name)
{
    foreach (genericObject* object, ObjectsArray) {
        if (object->name == name) {
            return object->scale;
        }
    }
    return QVector3D();
}
void ObjectsHandler::MoveObject(QString name, QVector3D direction)
{
    QVector3D rotation;
    QVector3D relativeVector;
    QVector3D currentPosition;

    currentPosition = getObjectTranslation(name);
    relativeVector = (direction).normalized();

    float x = relativeVector.x();
    float y = relativeVector.y();
    float z = relativeVector.z();

    rotation.setZ( atan2(y, x) * 57.2958 );                         // 57.2958 - magic number to
    rotation.setY( (atan2(z, sqrt(x*x + y*y)) * -1) * 57.2958 );    // convert radians to degrees

    RotateObject(name, rotation);

    TranslateObject(name, currentPosition + direction);
}
void ObjectsHandler::DeleteObject(QString name)
{
    uint id=0;
    foreach (genericObject* object, ObjectsArray) {
        if (object->name == name) {

            ObjectsArray.remove(id);
            ObjectsArray.squeeze();

            object->shader->removeAllShaders();
            object->shader->deleteLater();
            object->vData->destroy();
            object->vIndices->destroy();
            contextWidget->deleteTexture(object->textureBuffer);
            glDeleteVertexArrays(1, &object->VertexArrayObject);
            object->deleteLater();
        } else {
            id++;
        }
    }
}

void ObjectsHandler::CreateTrajectory(QString name, QVector<QVector3D> points)
{
    trajBuilder = new TrajectoryBuilder(contextWidget);

    connect(trajBuilder, SIGNAL(ObjectReady(genericObject*)),
            this, SLOT(ReceiveNewObject(genericObject*)) );

    trajBuilder->setName(name)
            ->setPoints(points)
            ->Build();
}
