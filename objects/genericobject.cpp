#include "genericobject.h"

genericObject::genericObject(QObject *parent) : QObject(parent)
{
    name = "default";
    polygonType = GL_TRIANGLES;
    scale = QVector3D(1,1,1);
    objectTime = 0;
}

genericObject::~genericObject()
{
}

void genericObject::Draw()
{
    glPushMatrix();

    glTranslatef(translation.x(), translation.y(), translation.z());

    glRotatef(rotation.z(), 0, 0, 1);
    glRotatef(rotation.y(), 0, 1, 0);
    glRotatef(rotation.x(), 1, 0, 0);

    glScalef(scale.x(), scale.y(), scale.z());

    shader->bind();

    shader->setAttributeValue("time", objectTime);

    glBindTexture(GL_TEXTURE_2D, textureBuffer);

    glBindVertexArray(VertexArrayObject);
    glDrawElements(polygonType, vCount, GL_UNSIGNED_INT, 0 );
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    shader->release();

    glPopMatrix();

    objectTime+=0.1;
}
