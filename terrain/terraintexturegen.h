#ifndef TERRAINTEXTUREGEN_H
#define TERRAINTEXTUREGEN_H

#include <QObject>
#include <QRect>
#include <QImage>
#include <terrain/mtwparser.h>
#include <QHash>


class TerrainTextureGen : public QObject
{
    Q_OBJECT
public:
    explicit TerrainTextureGen(QObject *parent = 0);
    QImage GenerateTexture(ChunkData chunkData,
                           terrainObject* chunk = 0);
    void SetNewMapPath(QString newMapFilePath);

private:
    bool isProcessing;

private:
    QMap<terrainObject*, ChunkData> chunkQueue;
    QVector< QMap<terrainObject*, ChunkData>::iterator > highResCounter,
                                                         midResCounter,
                                                         lowResCounter;

    QString mapFilePath;

signals:
    void textureReady(terrainObject* chunk, QImage newTexture);
    void StartProcess();

public slots:
    void GenerateUpdateTexture(ChunkData chunkData,
                               terrainObject* chunk = 0);
    void Process();

};

#endif // TERRAINTEXTUREGEN_H
