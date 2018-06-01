#include "objectbuilder.h"
#include "objects/plyparser.h"
#include <QtConcurrentRun>


ObjectBuilder::ObjectBuilder(QGLWidget* widget ,QObject *parent) : QObject(parent)
{
    contextWidget = widget;

    initializeGLFunctions(contextWidget->context());

    glGenVertexArrays = (_glGenVertexArrays)contextWidget->context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray)contextWidget->context()->getProcAddress("glBindVertexArray");

    newObject = new genericObject();

    newObject->glBindVertexArray = glBindVertexArray;

}


ObjectBuilder* ObjectBuilder::setName(QString name)
{
    newObject->name = name;
    return this;
}
ObjectBuilder* ObjectBuilder::setPosition(QVector3D newPosition)
{
    newObject->translation = newPosition;
    return this;
}
ObjectBuilder* ObjectBuilder::setRotation(QVector3D newRotation)
{
    newObject->rotation = newRotation;
    return this;
}
ObjectBuilder* ObjectBuilder::setScale(QVector3D newScale)
{
    newObject->scale = newScale;
    return this;
}
ObjectBuilder* ObjectBuilder::setDrawType(uint newDrawType)
{
    newObject->polygonType = newDrawType;
    return this;
}
ObjectBuilder* ObjectBuilder::setTexturePath(QString newTexturePath)
{
    texturePath = newTexturePath;
    return this;
}
ObjectBuilder* ObjectBuilder::setShaderPath(QString newShaderPath)
{
    shaderPath = newShaderPath;
    return this;
}
ObjectBuilder* ObjectBuilder::setFileToLoad(QString newFilePath)
{
    modelFilePath = newFilePath;
    return this;
}
void ObjectBuilder::Build()
{
    newObject->shader = new QGLShaderProgram();

    newObject->vData = new QGLBuffer(QGLBuffer::VertexBuffer);
    newObject->vData->create();
    newObject->vData->bind();
    newObject->vData->setUsagePattern(QGLBuffer::DynamicDraw);
    newObject->vData->release();

    newObject->vIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    newObject->vIndices->create();
    newObject->vIndices->bind();
    newObject->vIndices->setUsagePattern(QGLBuffer::DynamicDraw);
    newObject->vIndices->release();

    CompileShader();

    BindTexture();

    LoadMeshFromFile();
}

void ObjectBuilder::CompileShader()
{
    newObject->shader->removeAllShaders();

    newObject->shader->addShaderFromSourceFile(QGLShader::Vertex,
                                               QString("%1.%2").arg(shaderPath, "vert"));
    newObject->shader->addShaderFromSourceFile(QGLShader::Fragment,
                                               QString("%1.%2").arg(shaderPath, "frag"));
    newObject->shader->link();
}

void ObjectBuilder::BindTexture()
{
    newObject->textureBuffer = contextWidget->bindTexture(texturePath, GL_TEXTURE_2D);
}

void ObjectBuilder::LoadMeshFromFile()
{
    PLYparser* parser = new PLYparser;

    connect(parser, SIGNAL(sendMeshData(QVector<float>,QVector<uint>)),
            this, SLOT(receiveMeshData(QVector<float>,QVector<uint>)) );

    QtConcurrent::run(parser, &PLYparser::start, modelFilePath);
}

void ObjectBuilder::receiveMeshData(QVector<float> pointsData,
                                    QVector<uint> pointsIndices )
{
    glGenVertexArrays(1, &newObject->VertexArrayObject);
    glBindVertexArray(newObject->VertexArrayObject);

        newObject->vData->bind();
        newObject->vData->allocate( pointsData.constData(), pointsData.size() * sizeof(float) );

        newObject->vIndices->bind();
        newObject->vIndices->allocate( pointsIndices.constData(), pointsIndices.size() * sizeof(uint) );

        newObject->vCount = pointsIndices.size();


        // Positions
        int pos_location = newObject->shader->attributeLocation("Vert_Pos");
        if (pos_location != -1)
        {
            glVertexAttribPointer(pos_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(pos_location);
        }

        // Normals
        int normals_location = newObject->shader->attributeLocation("Vert_Normal");
        if (normals_location != -1)
        {
            glVertexAttribPointer(normals_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(normals_location);
        }

        // Texels
        int texels_location = newObject->shader->attributeLocation("Vert_Texel");
        if (texels_location != -1)
        {
            glVertexAttribPointer(texels_location, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)) );
            glEnableVertexAttribArray(texels_location);
        }

    glBindVertexArray(0);


    emit ObjectReady(newObject);
    this->deleteLater();
}

