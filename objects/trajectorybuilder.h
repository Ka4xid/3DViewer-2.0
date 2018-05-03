#ifndef TRAJECTORYBUILDER_H
#define TRAJECTORYBUILDER_H

#include <QObject>
#include <QVector3D>
#include <QGLWidget>
#include <QGLFunctions>
#include "objects/genericobject.h"


class TrajectoryBuilder : public QObject, private QGLFunctions
{
    Q_OBJECT
public:
    explicit TrajectoryBuilder(QGLWidget* widget, QObject *parent = 0);

    TrajectoryBuilder* setName(QString newName);
    TrajectoryBuilder* setPoints(QVector<QVector3D> newPoints);
    void Build();


private:
    _glGenVertexArrays glGenVertexArrays = NULL;
    _glBindVertexArray glBindVertexArray = NULL;

    void GenerateTrajectoryMesh(QVector<QVector3D> points);
    void CreateQuadsForPoints(QVector3D point_previous,
                              QVector3D point,
                              QVector3D point_next);


private:
    genericObject* newTrajectory;

    QVector<QVector3D> trajectoryPoints;

    void CompileShader();
    void BindTexture();

    QVector<float> pointsData;
    QVector<uint> pointsIndices;

    int even;

signals:
    void ObjectReady(genericObject* newObject);
    void MeshDone(QVector<float> vData, QVector<uint> vIndices);

public slots:
    void ReceiveMesh(QVector<float> vData, QVector<uint> vIndices);
};

#endif // TRAJECTORYBUILDER_H
