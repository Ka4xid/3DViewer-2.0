#include "trajectorybuilder.h"
#include <qmath.h>
#include <QtConcurrentRun>

#define SIDES_NUMBER 8
#define RADIUS 3

TrajectoryBuilder::TrajectoryBuilder(QGLWidget *widget, QObject *parent) : QObject(parent)
{
    initializeGLFunctions(widget->context());

    glGenVertexArrays = (_glGenVertexArrays)widget->context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray)widget->context()->getProcAddress("glBindVertexArray");

    newTrajectory = new genericObject;

    newTrajectory->shader = new QGLShaderProgram();

    newTrajectory->vData = new QGLBuffer(QGLBuffer::VertexBuffer);
    newTrajectory->vData->create();
    newTrajectory->vData->bind();
    newTrajectory->vData->setUsagePattern(QGLBuffer::DynamicDraw);
    newTrajectory->vData->release();

    newTrajectory->vIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    newTrajectory->vIndices->create();
    newTrajectory->vIndices->bind();
    newTrajectory->vIndices->setUsagePattern(QGLBuffer::DynamicDraw);
    newTrajectory->vIndices->release();

    newTrajectory->polygonType = GL_QUADS;

    newTrajectory->glBindVertexArray = glBindVertexArray;

    newTrajectory->textureBuffer = widget->bindTexture(":/checker.png");

    even = 0;
}

TrajectoryBuilder* TrajectoryBuilder::setName(QString newName)
{
    newTrajectory->name = newName;
    return this;
}

TrajectoryBuilder* TrajectoryBuilder::setPoints(QVector<QVector3D> newPoints)
{
    trajectoryPoints = newPoints;
    return this;
}

void TrajectoryBuilder::Build()
{
    CompileShader();

    connect(this, SIGNAL(MeshDone(QVector<float>,QVector<uint>)),
            this, SLOT(ReceiveMesh(QVector<float>,QVector<uint>)));

    QtConcurrent::run(this, &TrajectoryBuilder::GenerateTrajectoryMesh, trajectoryPoints);
}


void TrajectoryBuilder::CompileShader()
{
    newTrajectory->shader->removeAllShaders();

    newTrajectory->shader->addShaderFromSourceFile(QGLShader::Vertex, ":/shaders/trajectory.vert");
    newTrajectory->shader->addShaderFromSourceFile(QGLShader::Fragment, ":/shaders/trajectory.frag");
    newTrajectory->shader->link();
}

void TrajectoryBuilder::GenerateTrajectoryMesh(QVector<QVector3D> points)
{
    if (points.size() > 2) {
        even=1;
        CreateQuadsForPoints(points.at(0),
                             points.at(0),
                             points.at(1) );

        for (int n=1; n < points.size()-1; n++) {
            even++;
            CreateQuadsForPoints(points.at(n-1),
                                 points.at(n),
                                 points.at(n+1) );
        }
        even++;
        CreateQuadsForPoints(points.at(points.size()-2),
                             points.at(points.size()-1),
                             points.at(points.size()-1) );
    }
    else if (points.size() == 2) {
        CreateQuadsForPoints(points.at(0),
                             points.at(0),
                             points.at(1) );
        even++;
        CreateQuadsForPoints(points.at(0),
                             points.at(1),
                             points.at(1) );
    }
    else {
        qDebug() << "UNABLE TO BUILD TRAJECTORY FOR 1 POINT";
        return;
    }

    for (int i=0; i <= pointsData.size()/8-SIDES_NUMBER*2; i+=SIDES_NUMBER) {
        for(int n=0; n<SIDES_NUMBER-1; n++) {
            pointsIndices.append(i+n);
            pointsIndices.append(i+n+SIDES_NUMBER);
            pointsIndices.append(i+n+SIDES_NUMBER+1);
            pointsIndices.append(i+n+1);
        }
        pointsIndices.append(i+(SIDES_NUMBER-1));
        pointsIndices.append(i+(SIDES_NUMBER*2-1));
        pointsIndices.append(i+SIDES_NUMBER);
        pointsIndices.append(i);
    }

    emit MeshDone(pointsData, pointsIndices);
}

void TrajectoryBuilder::CreateQuadsForPoints(QVector3D point_previous,
                                             QVector3D point,
                                             QVector3D point_next)
{
    QVector3D v;
    QVector3D relativeVector;
    float angle;
    QQuaternion quat_yaw,
                quat_pitch,
                quat_summ;

    relativeVector = point_next - point_previous;

    // 57.2958 - MAGIC NUMBER TO CONVERT FROM RADIANS TO DEGREES
    angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
    quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
    angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
    quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

    quat_summ = (quat_yaw * quat_pitch).normalized();


    // TO MAKE MESH SMOOTHER, JUST ADJUST SIDES_NUMBER VALUE

    // FIND RADIANS IN ONE SIDE
    float radians = M_PI / (SIDES_NUMBER/2);

    // CREATE QUADS FOR SIDES
    for (int n=0; n<SIDES_NUMBER; n++) {

        // GENERATE POSITION 1
            v = point + quat_summ.rotatedVector(QVector3D(cos(radians * n) * RADIUS, sin(radians * n) * RADIUS, 0));
            pointsData.append(v.x());
            pointsData.append(v.y());
            pointsData.append(v.z());
        // GENERATE NORMALS 1
            v = quat_summ.rotatedVector(QVector3D(cos(radians * n) * RADIUS, sin(radians * n) * RADIUS, 0));
            pointsData.append(v.x());
            pointsData.append(v.y());
            pointsData.append(v.z());
        // GENERATE TEXTURE COORDS 1
            pointsData.append(0);
            pointsData.append(0);

    }
}

void TrajectoryBuilder::ReceiveMesh(QVector<float> vData,
                                    QVector<uint> vIndices)
{
    glGenVertexArrays(1, &newTrajectory->VertexArrayObject);
    glBindVertexArray(newTrajectory->VertexArrayObject);

        newTrajectory->vData->bind();
        newTrajectory->vData->allocate( vData.constData(), vData.size() * sizeof(float) );

        newTrajectory->vIndices->bind();
        newTrajectory->vIndices->allocate( vIndices.constData(), vIndices.size() * sizeof(uint) );

        newTrajectory->vCount = vIndices.size();


        // Positions
        int pos_location = newTrajectory->shader->attributeLocation("Vert_Pos");
        if (pos_location != -1)
        {
            glVertexAttribPointer(pos_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(pos_location);
        }

        // Normals
        int normals_location = newTrajectory->shader->attributeLocation("Vert_Normal");
        if (normals_location != -1)
        {
            glVertexAttribPointer(normals_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(normals_location);
        }

        // Texels
        int texels_location = newTrajectory->shader->attributeLocation("Vert_Texel");
        if (texels_location != -1)
        {
            glVertexAttribPointer(texels_location, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)) );
            glEnableVertexAttribArray(texels_location);
        }

    emit ObjectReady(newTrajectory);
    this->deleteLater();
}
