#include "mtwparser.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

#define BAD_ELEMENT -10002
#define NA -32767

MTWParser::MTWParser(QObject *parent) : QObject(parent)
{
}

void MTWParser::SetFile(QString newMtwFilePath)
{
    mtwFilePath = newMtwFilePath;
    ParseInitialData();
}

void MTWParser::ParseInitialData()
{
    QFile mtwFile;

    mtwFile.setFileName(mtwFilePath);
    if ( mtwFile.open(QIODevice::ReadOnly) ) {

        QDataStream read(&mtwFile);
        read.setByteOrder(QDataStream::LittleEndian);

        read.skipRawData(52);

        read >> Matrix.elementSize;
        read >> Matrix.rowCount;
        read >> Matrix.colCount;
        read >> Matrix.blockColCount;
        read >> Matrix.blockRowCount;
        read >> Matrix.blockHeight;
        read >> Matrix.blockWidth;
        read >> Matrix.blockCutHeight;
        read >> Matrix.blockCutWidth;

        read.skipRawData(16);

        read >> Matrix.blockBeginDescOffset;
        read >> Matrix.blockDescSize;

        read.skipRawData(40);

        read >> Matrix.elementMeters;
        read >> Matrix.leftBottomX;
        read >> Matrix.leftBottomY;

        Matrix.leftTopX = Matrix.leftBottomX + Matrix.rowCount * Matrix.elementMeters;
        Matrix.leftTopY = Matrix.leftBottomY;

        mtwFile.seek(304);
        int32_t value;
        read >> value;
        switch (value) {
            case 0:
                Matrix.measurementUnits = 1;
                break;
            case 1:
                Matrix.measurementUnits = 0.1;
                break;
            case 2:
                Matrix.measurementUnits = 0.01;
                break;
            case 3:
                Matrix.measurementUnits = 0.001;
                break;
            default:
                break;
        }
        mtwFile.seek(280);
        read >> Matrix.minHeight;
        read >> Matrix.maxHeight;
        Matrix.minHeight = Matrix.minHeight * Matrix.measurementUnits;
        Matrix.maxHeight = Matrix.maxHeight * Matrix.measurementUnits;
    }
}

BlockData MTWParser::LoadBlock(uint row, uint col)
{
    BlockData currentBlock;

    quint32 blockIndex,
             blockDescOffset,
             blockOffset,
             blockSize;

    QFile mtwFile;
    mtwFile.setFileName(mtwFilePath);
    mtwFile.open(QIODevice::ReadOnly);
    QDataStream read(&mtwFile);                     // setup data stream
    read.setByteOrder(QDataStream::LittleEndian);   // set byte order in strea


    blockIndex = row * Matrix.blockColCount + col;   // calculate required block index
    blockDescOffset = blockIndex * 8;               // calculate block offset


    read.skipRawData( (Matrix.blockBeginDescOffset + blockDescOffset) );     //
    read >> blockOffset;                                                    // read block offset
    read >> blockSize;                                                      // read block size


    currentBlock.data.reserve(blockSize);                                          // reserve space in block data structure
    mtwFile.seek(blockOffset);                                              //
    currentBlock.data = mtwFile.read(blockSize);                                   // read data to block data structure
    currentBlock.row = row;                                                        //
    currentBlock.col = col;                                                        //

                                                                            // set proper width of height of block
    if ( (row == Matrix.blockRowCount-1)) {
        currentBlock.height = Matrix.blockCutHeight;
    } else {
        currentBlock.height = Matrix.blockHeight;
    }
    if ( (col == Matrix.blockColCount-1)) {
        currentBlock.width = Matrix.blockCutWidth;
    } else {
        currentBlock.width = Matrix.blockWidth;
    }

    return currentBlock;
}

QVector<float> MTWParser::GetHeightsFromBlock(BlockData currentBlock, ChunkData* currentChunk, LODlevel lodLevel)
{
    QVector<float> heightsArray;

    for (uint elementRow = 0; elementRow<currentBlock.height; elementRow+=lodLevel) {
        currentChunk->width = 0;
        for (uint elementCol = 0; elementCol< currentBlock.width; elementCol+=lodLevel) {

            float height = ReadElementInBlock(currentBlock, elementRow, elementCol);
            heightsArray.append(height);
            currentChunk->width++;
        }
        currentChunk->height++;
    }


    return heightsArray;
}

float MTWParser::ReadElementInBlock(BlockData currentBlock, uint row, uint col)
{
    quint32 elementOffset;
    qint32 height;

    if ( (row > currentBlock.height) || (col > currentBlock.width) ) {
        return BAD_ELEMENT;
    }

    elementOffset = (row * currentBlock.width + col) * (Matrix.elementSize/8);

    QDataStream read(currentBlock.data);
    read.setByteOrder(QDataStream::LittleEndian);

    read.skipRawData(elementOffset);
    read >> height;
    height = height * Matrix.measurementUnits;

    if (height == NA) return Matrix.minHeight;    // -32767 - value equal to "NO HEIGHT" in matrix

    return height;
}


ChunkData MTWParser::GetChunkData(uint row, uint col, LODlevel lodLevel, bool isReturn)
{
    ChunkData chunk;
    BlockData currentBlock;
    QVector<float> heightsArray;

    chunk.row = row;
    chunk.col = col;
    chunk.width = 0;
    chunk.height = 0;
    chunk.lodLevel = lodLevel;



    switch (lodLevel) {
    case lod_0:
        currentBlock = LoadBlock(row, col);
        heightsArray.append( ReadElementInBlock(currentBlock, 0, 0) );
        chunk.width++;
        chunk.height++;
        currentBlock = LoadBlock(row, col+1);
        heightsArray.append( ReadElementInBlock(currentBlock, 0, 0) );
        chunk.width++;
        currentBlock = LoadBlock(row+1, col);
        heightsArray.append( ReadElementInBlock(currentBlock, 0, 0) );
        chunk.height++;
        currentBlock = LoadBlock(row+1, col+1);
        heightsArray.append( ReadElementInBlock(currentBlock, 0, 0) );
    break;

    default:
        currentBlock = LoadBlock(row, col);
        heightsArray = GetHeightsFromBlock(currentBlock, &chunk, lodLevel);

        // FILL SIDE PATCH
        if (col < Matrix.blockColCount-1) {
            BlockData patchBlock = LoadBlock(row, col+1);
            for (uint elementRow = 0; elementRow < patchBlock.height; elementRow+=lodLevel) {
                float height = ReadElementInBlock(patchBlock, elementRow, 0);
                heightsArray.insert( ((chunk.width * ((elementRow/lodLevel)+1)) + (elementRow/lodLevel)), height);
            }
            chunk.width++;
        }


        if (row < Matrix.blockRowCount-1) {
            BlockData patchBlock = LoadBlock(row+1, col);
            for (uint elementCol = 0; elementCol < patchBlock.width; elementCol+=lodLevel) {
                float height = ReadElementInBlock(patchBlock, 0, elementCol);
                heightsArray.append(height);
            }
            chunk.height++;
        }


        if ( (row < Matrix.blockRowCount-1) && (col < Matrix.blockColCount-1)) {
            BlockData patchBlock = LoadBlock(row+1, col+1);
            float height = ReadElementInBlock(patchBlock, 0, 0);
            heightsArray.append(height);
        }
    break;
    }



    float multiplier_height;
    float multiplier_width;

    if (col < Matrix.blockColCount-1) {
        multiplier_width = (Matrix.elementMeters * Matrix.blockWidth) / (chunk.width-1);
    } else {
        multiplier_width = (Matrix.elementMeters * Matrix.blockCutWidth) / (chunk.width-1);
    }
    if (row < Matrix.blockRowCount-1) {
        multiplier_height = (Matrix.elementMeters * Matrix.blockHeight) / (chunk.height-1);
    } else {
        multiplier_height = (Matrix.elementMeters * Matrix.blockCutHeight) / (chunk.height-1);
    }



    // FILL ARRAY WITH UNIQUE POINTS
    float x = 0;
    float y = 0;

    float s = 0;
    float t = 0;

    foreach (float point_height, heightsArray) {

        if (y >= chunk.width)
        {
            y = 0;
            x += 1;
            s = 0;
            t += 1;
        }

        float translation_x = row * Matrix.blockHeight * Matrix.elementMeters;
        float translation_y = col * Matrix.blockWidth * Matrix.elementMeters;

        // position
        chunk.pData.append( Matrix.leftTopX - translation_x - x * multiplier_height);
        chunk.pData.append( Matrix.leftTopY + translation_y + y * multiplier_width);
        chunk.pData.append( point_height );
        // normals
        chunk.pData.append( 0 );
        chunk.pData.append( 0 );
        chunk.pData.append( 1 );
        // texels
        chunk.pData.append( (1.0/((float)chunk.width - 1.0)) * s );
        chunk.pData.append( 1.0 - (1.0/((float)chunk.height - 1.0)) * t );

        y += 1;
        s += 1;
    }


    uint q=0;
    for (uint n=0; n<(chunk.pData.size()/8-chunk.width); n++)
    {
        if (q == chunk.width-1)      // OPTIMIZE ME
        {
            q=0;
            continue;
        }

        chunk.pIndices.append(n);
        chunk.pIndices.append(n + 1);
        chunk.pIndices.append(n + chunk.width);

        chunk.pIndices.append(n+1);
        chunk.pIndices.append(n + chunk.width);
        chunk.pIndices.append(n + chunk.width+1);

        q++;
    }


    chunk.leftTop.setX(     Matrix.leftTopX - ((row * Matrix.blockHeight) * Matrix.elementMeters));
    chunk.leftTop.setY(     Matrix.leftTopY + ((col * Matrix.blockWidth) * Matrix.elementMeters));

    chunk.rightBottom.setX( chunk.leftTop.x() + (Matrix.blockHeight+1) * Matrix.elementMeters);
    chunk.rightBottom.setY( chunk.leftTop.y() + (Matrix.blockWidth+1) * Matrix.elementMeters);


    if (isReturn) {
        return chunk;
    } else {
        emit ChunkDataReady(chunk);
        return chunk;
    }
}
