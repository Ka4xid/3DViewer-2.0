#include "terrainhandler.h"
#include <QDebug>
#include <QtConcurrentRun>
#include <qmath.h>
#include <QFileInfo>

uint size = 0;

TerrainHandler::TerrainHandler(QGLWidget *widget, QObject *parent) : QObject(parent)
{
    contextWidget = widget;

    initializeGLFunctions(contextWidget->context());

    glBindVertexArray = (_glBindVertexArray)contextWidget->context()->getProcAddress("glBindVertexArray");

    commonShader = new QGLShaderProgram(contextWidget->context());

    CompileShader();
}

void TerrainHandler::SetCameraPosPointer(QVector3D *newCamPos)
{
    cameraPos = newCamPos;
}

void TerrainHandler::GenerateTerrain(QString mtwFilePath,
                                     QString mapFilePath)
{
    QFileInfo mtwFile(mtwFilePath);
    QFileInfo mapFile(mapFilePath);

    if (!((mtwFile.exists()) && (mapFile.exists())))
            return;


    terrBuilder = new TerrainBuilder(contextWidget);
    QtConcurrent::run(terrBuilder, &TerrainBuilder::BuildTerrainFull, mtwFilePath, mapFilePath);
    connect(terrBuilder, SIGNAL(ChunkReady(terrainObject*)),
            this, SLOT(ReceiveNewObject(terrainObject*)));
}


void TerrainHandler::CompileShader()
{
    commonShader->removeAllShaders();

    commonShader->addShaderFromSourceFile(QGLShader::Vertex,
                                               ":/shaders/default.vert");
    commonShader->addShaderFromSourceFile(QGLShader::Fragment,
                                               ":/shaders/default.frag");
    commonShader->link();
}

void TerrainHandler::ProcessChunkToShader(terrainObject* newChunk)
{
    glBindVertexArray(newChunk->VertexArrayObject);
        // Positions
        int pos_location = commonShader->attributeLocation("Vert_Pos");
        if (pos_location != -1)
        {
            glVertexAttribPointer(pos_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(pos_location);
        }

        // Normals
        int normals_location = commonShader->attributeLocation("Vert_Normal");
        if (normals_location != -1)
        {
            glVertexAttribPointer(normals_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(normals_location);
        }

        // Texels
        int texels_location = commonShader->attributeLocation("Vert_Texel");
        if (texels_location != -1)
        {
            glVertexAttribPointer(texels_location, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)) );
            glEnableVertexAttribArray(texels_location);
        }
    glBindVertexArray(0);
}

void TerrainHandler::ProcessLODs()
{
    foreach (terrainObject* chunk, ChunksArray) {
        float distance = floor((*cameraPos - chunk->position).length());
        if ( (distance <= 6000) && (chunk->lodLevel != lod_1) ) {
            terrBuilder->UpdateChunk(chunk, lod_1);
        } else
        if ( (distance > 6000) && (distance <= 15000) && (chunk->lodLevel != lod_2) ) {
            terrBuilder->UpdateChunk(chunk, lod_2);
        } else
        if ( (distance > 15000)  && (chunk->lodLevel != lod_3) ) {
            terrBuilder->UpdateChunk(chunk, lod_3);
        }
    }
}

void TerrainHandler::ReceiveNewObject(terrainObject *newChunk)
{
    size += newChunk->vData->size();

    ProcessChunkToShader(newChunk);

    ChunksArray.append(newChunk);
}

void TerrainHandler::Render()
{
//    qDebug() << "Size bytes: " << size;
//    qDebug() << "Size kbytes: " << size/1024;
//    qDebug() << "Size Mbytes: " << size/1024/1024;
    glPushMatrix();

    commonShader->bind();
    foreach (terrainObject* chunk, ChunksArray) {

        float distance = floor((*cameraPos - chunk->position).length());
        if (distance < 100000) {
            glBindVertexArray(chunk->VertexArrayObject);
            glBindTexture(GL_TEXTURE_2D, chunk->textureBuffer);
            glDrawElements(GL_TRIANGLES, chunk->vCount, GL_UNSIGNED_INT, 0 );
            glBindVertexArray(0);
        }
    }
    commonShader->release();

    glPopMatrix();

    ProcessLODs();
}
