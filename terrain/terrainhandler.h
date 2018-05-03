#ifndef TERRAINHANDLER_H
#define TERRAINHANDLER_H

#include <QObject>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QGLFunctions>
#include "terrain/terrainobject.h"
#include "terrain/terrainbuilder.h"


class TerrainHandler : public QObject, private QGLFunctions
{
    Q_OBJECT
public:
    explicit TerrainHandler(QGLWidget* widget, QObject *parent = 0);

    void BuildTerrain();

    void DrawTerrain();


private:
    void ProcessLODs();

    void CompileShader();

    void ProcessChunkToShader(terrainObject* newChunk);


private:
    _glBindVertexArray glBindVertexArray = NULL;

    TerrainBuilder* terrBuilder;

    QVector<terrainObject*> ChunksArray;

    QGLWidget* contextWidget;

    QString mtwFilePath,
            mapFilePath;

    const QVector3D *cameraPos;

    QGLShaderProgram* commonShader;

signals:

public slots:

    void SetCameraPosPointer(QVector3D* newCamPos);

    void GenerateTerrain(QString mtwFilePath,
                         QString mapFilePath );

    void Render();

    void ReceiveNewObject(terrainObject* newChunk);

};

#endif // TERRAINHANDLER_H
