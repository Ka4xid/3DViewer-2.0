#ifndef TERRAINTEXTUREGEN_H
#define TERRAINTEXTUREGEN_H

#include <QObject>
#include <QRect>
#include <QImage>
#include <terrain/mtwparser.h>

class TerrainTextureGen : public QObject
{
    Q_OBJECT
public:
    explicit TerrainTextureGen(QObject *parent = 0);
    QImage GenerateTexture(ChunkData chunkData,
                           terrainObject* chunk = 0);
    void SetNewMapPath(QString newMapFilePath);

private:
    QMap<terrainObject*, ChunkData> chunkQueue;
    QString mapFilePath;

signals:
    void textureReady(terrainObject* chunk, QImage newTexture);

public slots:
    void GenerateUpdateTexture(ChunkData chunkData,
                               terrainObject* chunk = 0);

};

#endif // TERRAINTEXTUREGEN_H
