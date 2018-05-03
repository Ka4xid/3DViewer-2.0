#include "terraintexturegen.h"
#include <QRect>
#include <QPainter>
#include <QDir>
#include <QDebug>
#include <QtConcurrentRun>

// that include MUST BE LAST, else - wont compile
#include "mapapi.h"


TerrainTextureGen::TerrainTextureGen(QObject *parent) : QObject(parent)
{
}

void TerrainTextureGen::SetNewMapPath(QString newMapFilePath)
{
    mapFilePath = newMapFilePath;
}

void TerrainTextureGen::GenerateUpdateTexture(ChunkData chunkData,
                                              terrainObject *chunk)
{
    GenerateTexture(chunkData, chunk);
}


QImage TerrainTextureGen::GenerateTexture(ChunkData chunkData,
                                          terrainObject *chunk )
{
    QImage img;

    uint resolution;
    switch (chunkData.lodLevel) {
        case lod_0:
            resolution = 300000;
        break;
        case lod_1:
            resolution = 50000;
        break;
        case lod_2:
            resolution = 100000;
        break;
        case lod_3:
            resolution = 200000;
        break;
    }


    double Top = chunkData.leftTop.x();
    double Left = chunkData.leftTop.y();
    double Bottom = chunkData.rightBottom.x();
    double Right = chunkData.rightBottom.y();

    // PANORAMA's code, I dont even know what they are doing there
    HMAP hMap = 0;
    RECT RectDraw;

    int width, height;

    hMap = mapOpenData( mapFilePath.toUtf8().data(), GENERIC_READ );

    if (hMap)
    {
        QPainter p;

        mapSetRealShowScale(hMap, resolution);

        mapPlaneToPicture(hMap, &Top, &Left);
        mapPlaneToPicture(hMap, &Bottom, &Right);

        height= Bottom - Top;
        width = Left - Right;

        RectDraw.left = (int)Top;
        RectDraw.top  = (int)Left;
        RectDraw.right  = (int)Top + (int)width;
        RectDraw.bottom = (int)Left + (int)height;

        // Количество бит, выделяемое на одну точку изображения карты
        int  mapdepth = 32;
        XIMAGEDESC Ximagedesc;

        // Дополнить до mapdepth байт
        long allignwidth = width * (mapdepth / 8);
        long size   = allignwidth*height;
        char * bits = AllocateTheMemory(size);

        if (!bits) return QImage();
        memset(bits, 0x0, size);

        Ximagedesc.Point     = bits;                        // Адрес начала области пикселов
        Ximagedesc.Width     = width;                       // Ширина строки в пикселах
        Ximagedesc.Height    = height;                      // Число строк
        Ximagedesc.Depth     = mapdepth;                    // Размер элемента в битах (8,15,16,24,32)
        Ximagedesc.CellSize  = mapdepth / 8;                // Размер элемента(пиксела) в байтах
        Ximagedesc.RowSize   = width * Ximagedesc.CellSize; // Ширина строки в байтах

        // Отобразим фрагмент карты в памяти
        mapPaintToXImage(hMap, &Ximagedesc, 0, 0, &RectDraw);

        QImage image((uchar *)bits, width, height, QImage::Format_RGB32);

        p.drawImage(Top, Left, image, width, height);

        img = image.copy();

        FreeTheMemory(bits);

        mapCloseData(hMap);

        if (chunk != 0) {
            emit textureReady(chunk, img);
            return QImage();
        } else {
            return img;
        }
    }
    return QImage(":/checker.png");
}


