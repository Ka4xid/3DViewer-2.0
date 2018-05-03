#ifndef MTWPARSER_H
#define MTWPARSER_H

#include <QObject>
#include <QVector>
#include <QRectF>
#include "terrainobject.h"


typedef struct {
    QByteArray data;
    quint32 row,
            col;
    quint32 height,
            width;
} BlockData;


typedef struct {
    QVector<float> pData;
    QVector<uint> pIndices;
    quint32 row,
            col;
    quint32 height,
            width;
    LODlevel lodLevel;
    QPointF leftTop;
    QPointF rightBottom;
} ChunkData;


typedef struct {
    quint32 elementSize;           // in bits
    quint32 rowCount;              // image height in elements
    quint32 colCount;              // image width in elements

    quint32 blockRowCount;         // number of blocks in image
    quint32 blockColCount;         // number of blocks in image
    quint32 blockHeight;           // in elements
    quint32 blockWidth;            // in elements
    quint32 blockCutHeight;        // in elements
    quint32 blockCutWidth;         // in elements
    quint32 blockBeginDescOffset;
    quint32 blockDescSize;

    float minHeight,              // in units
          maxHeight;              // in units

    float elementMeters;           // element size in meters
    float leftBottomX;             // X coord of left-bottom point in meters
    float leftBottomY;             // Y coord of left-bottom point in meters

    float leftTopX;
    float leftTopY;

    float measurementUnits;
} MatrixData;


class MTWParser : public QObject
{
    Q_OBJECT

public:
    explicit MTWParser(QObject *parent = 0);
    void SetFile(QString newMtwFilePath);
    ChunkData GetChunkData(uint row, uint col, LODlevel lodLevel, bool isReturn = false);


private:
    void ParseInitialData();
    BlockData LoadBlock(uint row, uint col);
    QVector<float> GetHeightsFromBlock(BlockData currentBlock, ChunkData* currentChunk, LODlevel lodLevel);
    float ReadElementInBlock(BlockData currentBlock, uint row, uint col);

public:
    MatrixData Matrix;


private:
    QString mtwFilePath;


signals:
    void ChunkDataReady(ChunkData chunkData);

public slots:

};

#endif // MTWPARSER_H
