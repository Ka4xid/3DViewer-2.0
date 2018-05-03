#include "terrainbuilder.h"
#include <QtConcurrentRun>
#include <QDebug>


TerrainBuilder::TerrainBuilder(QGLWidget* widget, QObject *parent) : QObject(parent)
{
    contextWidget = widget;


    texGenThread = new QThread();
    texGen = new TerrainTextureGen();
    texGen->moveToThread(texGenThread);
    connect(this, SIGNAL(GenerateUpdateTexture(ChunkData,terrainObject*)),
            texGen, SLOT(GenerateUpdateTexture(ChunkData,terrainObject*)), Qt::QueuedConnection);

    connect(texGen, SIGNAL(textureReady(terrainObject*,QImage)),
            this, SLOT(UpdateChunkTexture(terrainObject*,QImage)), Qt::QueuedConnection);
    texGenThread->start();


    mtwParserThread = new QThread();
    mtwParser = new MTWParser;
    mtwParser->moveToThread(mtwParserThread);
    connect(mtwParser, SIGNAL(ChunkDataReady(ChunkData)),
            this, SLOT(ReceiveChunkData(ChunkData)));
    mtwParserThread->start();


    initializeGLFunctions(contextWidget->context());

    glGenVertexArrays = (_glGenVertexArrays)contextWidget->context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray)contextWidget->context()->getProcAddress("glBindVertexArray");
}

void TerrainBuilder::BuildTerrainFull(QString newMtwFilePath, QString newMapFilePath)
{
    texGen->SetNewMapPath(newMapFilePath);

    GenerateAllChunks(newMtwFilePath);
}

void TerrainBuilder::GenerateAllChunks(QString newMtwFilePath)
{
    mtwParser->SetFile(newMtwFilePath);

    for (quint32 row = 0; row < mtwParser->Matrix.blockRowCount; row++) {
        for (quint32 column = 0; column < mtwParser->Matrix.blockColCount; column++ ) {
            mtwParser->GetChunkData(row, column, lod_2);
        }
    }

}


terrainObject* TerrainBuilder::CreateNewChunk()
{
    terrainObject* chunk;

    chunk = new terrainObject();

    chunk->vData = new QGLBuffer(QGLBuffer::VertexBuffer);
    chunk->vData->create();
    chunk->vData->bind();
    chunk->vData->setUsagePattern(QGLBuffer::DynamicDraw);
    chunk->vData->release();

    chunk->vIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    chunk->vIndices->create();
    chunk->vIndices->bind();
    chunk->vIndices->setUsagePattern(QGLBuffer::DynamicDraw);
    chunk->vIndices->release();

    return chunk;
}


void TerrainBuilder::BindTexture(terrainObject* newChunk, QImage newTexturePath){
    newChunk->textureBuffer = contextWidget->bindTexture(newTexturePath, GL_TEXTURE_2D, GL_RGB);
    glBindTexture(GL_TEXTURE_2D, newChunk->textureBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void TerrainBuilder::ReceiveChunkData(ChunkData newChunkData)
{
    terrainObject* newChunk = CreateNewChunk();

    newChunk->name = QString("Chunk") + QString::number(newChunkData.row) + QString("-") + QString::number(newChunkData.col);

    newChunk->row = newChunkData.row;
    newChunk->col = newChunkData.col;
    newChunk->lodLevel = newChunkData.lodLevel;


    //QImage newTexture = texGen->GenerateTexture(mapFilePath, newChunkData);
    emit GenerateUpdateTexture(newChunkData, newChunk);
    //BindTexture(newChunk, newTexture);


    float translation_x = newChunkData.leftTop.x() - (newChunkData.height/2) * mtwParser->Matrix.elementMeters * newChunkData.lodLevel;
    float translation_y = newChunkData.leftTop.y() + (newChunkData.width/2) * mtwParser->Matrix.elementMeters * newChunkData.lodLevel;


    newChunk->position = QVector3D(translation_x,
                                   translation_y,
                                   0);

    glGenVertexArrays(1, &newChunk->VertexArrayObject);
    glBindVertexArray(newChunk->VertexArrayObject);

        newChunk->vData->bind();
        newChunk->vData->allocate(newChunkData.pData.constData(), newChunkData.pData.size() * sizeof(float));

        newChunk->vIndices->bind();
        newChunk->vIndices->allocate(newChunkData.pIndices.constData(), newChunkData.pIndices.size() * sizeof(uint));

        newChunk->vCount = newChunkData.pIndices.size();

   glBindVertexArray(0);

        emit ChunkReady(newChunk);
}


void TerrainBuilder::UpdateChunk(terrainObject *chunk, LODlevel newLodLevel)
{
    chunk->lodLevel = newLodLevel;
    ChunkData newChunkData = mtwParser->GetChunkData(chunk->row, chunk->col, newLodLevel, true);

    chunk->vData->bind();
    chunk->vData->allocate(newChunkData.pData.constData(), newChunkData.pData.size() * sizeof(float));

    chunk->vIndices->bind();
    chunk->vIndices->allocate(newChunkData.pIndices.constData(), newChunkData.pIndices.size() * sizeof(uint));

    chunk->vCount = newChunkData.pIndices.size();

    emit GenerateUpdateTexture(newChunkData, chunk);
}


void TerrainBuilder::UpdateChunkTexture(terrainObject* chunk, QImage newTexture)
{
    BindTexture(chunk, newTexture);
}
