#ifndef TERRAINBUILDER_H
#define TERRAINBUILDER_H

#include <QObject>
#include <QGLFunctions>
#include <QThread>
#include "terrain/mtwparser.h"
#include "terrain/terrainobject.h"
#include "terrain/terraintexturegen.h"


class TerrainBuilder : public QObject, private QGLFunctions
{
    Q_OBJECT
public:
    explicit TerrainBuilder(QGLWidget* widget, QObject *parent = 0);


    void BuildTerrainFull(QString newMtwFilePath,
                          QString newMapFilePath);

    void UpdateChunk(terrainObject* chunk, LODlevel newLodLevel );

private:
    void GenerateAllChunks(QString newMtwFilePath);
    terrainObject* CreateNewChunk();


private:

    QGLWidget* contextWidget;

    QThread* mtwParserThread;
    MTWParser* mtwParser;

    void CompileShader(terrainObject *newChunk);
    void BindTexture(terrainObject *newChunk, QImage newTexture);

    _glGenVertexArrays glGenVertexArrays = NULL;
    _glBindVertexArray glBindVertexArray = NULL;

    QString mapFilePath;

    QThread* texGenThread;
    TerrainTextureGen* texGen;

public slots:
    void ReceiveChunkData(ChunkData newChunkData);
    void UpdateChunkTexture(terrainObject* chunk, QImage newTexture);

signals:
    void ChunkReady(terrainObject* newChunk);
    void GenerateUpdateTexture(ChunkData chunkData,
                               terrainObject* chunk );

public slots:
};

#endif // TERRAINBUILDER_H
