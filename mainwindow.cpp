#include "mainwindow.h"
#include <QFileDialog>
#include <qmath.h>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setGeometry(100, 100, 1280, 720);

    Viewer = new ViewerClass(this, this);

    //Viewer->SetCameraPosition(QVector3D(6166111.571, 7412153.522, 0)); // PODOLSK
    //Viewer->SetCameraPosition(QVector3D(3702346.778, 31549569.378, 0)); // ALGERIA
    //Viewer->SetCameraPosition(QVector3D(5407550, 8570850, 0)); // ASHULUK
    Viewer->SetCameraPosition(QVector3D(0, 0, 0));

    testValue = 0;
}

MainWindow::~MainWindow()
{
}


void MainWindow::resizeEvent(QResizeEvent *e)
{
    Viewer->getScenePointer()->resize(e->size().width(),e->size().height());
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    Viewer->getScenePointer()->keyPressEvent(e);
    if (e->key() == Qt::Key_1) {
        QVector<QVector3D> points;

        points.clear();
        points.append(QVector3D(6174350, 7403050, 0));
        points.append(QVector3D(6170350, 7407050, 0));
        points.append(QVector3D(6170350, 7413450, 0));
        points.append(QVector3D(6170350, 7426250, 0));
        points.append(QVector3D(6170350, 7432650, 0));
        Viewer->CreateTrajectory("test_trajectory", points);

    }
    if (e->key() == Qt::Key_2) {
        Viewer->DeleteObject("default");
    }

    if (e->key() == Qt::Key_3) {
        /*QString filePath = QFileDialog::getOpenFileName(this,
                                                        "Select Model",
                                                        "/home/fuego",
                                                        "Stanford PLY files (*.ply)");
        QString texturePath = filePath;
        texturePath.chop(4);
        texturePath += ".png";

        Viewer->CreateObject()->setTexturePath(texturePath)
                              ->setModelFilePath(filePath)
                ->setScale(QVector3D(10,10,10))
                ->Done();*/


        QVector3D position;
        bool even = false;

        position = QVector3D(0, 0, 0);


        for (int i=0; i < 5; i++) {
            for (int j=0; j < 5; j++) {
                for (int k=0; k < 5; k++) {
                    if (even) {
                        Viewer->CreateObject()->setTexturePath("/home/fuego/zsu.png")
                                              ->setModelFilePath("/home/fuego/zsu.ply")
                                ->setScale(QVector3D(10,10,10))
                                ->setTranslation(position)
                                ->Done();

                        even = !even;
                    } else {
                        Viewer->CreateObject()->setTexturePath("/home/fuego/missile.png")
                                              ->setModelFilePath("/home/fuego/missile.ply")
                                ->setScale(QVector3D(10,10,10))
                                ->setTranslation(position)
                                ->Done();

                        even = !even;
                    }
                    position.setZ(position.z()+100);
                }
                position.setX(position.x()+100);
                position.setZ(0);
            }
            position.setY(position.y()+100);
            position.setX(0);
        }
    }

    if (e->key() == Qt::Key_4) {
        QString filePath = QFileDialog::getOpenFileName(this,
                                                        "Select Model",
                                                        "/home/fuego",
                                                        "Stanford PLY files (*.ply)");
        QString texturePath = filePath;
        texturePath.chop(4);
        texturePath += ".png";

        Viewer->CreateObject()->setTexturePath(texturePath)
                              ->setModelFilePath(filePath)
                ->setScale(QVector3D(50,50,50))
                ->Done();
    }

    if (e->key() == Qt::Key_Q) {

        QString mtwFilePath = QFileDialog::getOpenFileName(this,
                                                           "Select Map",
                                                           "/home/fuego",
                                                           "MTW files (*.mtw)");
        QString mapFilePath = mtwFilePath;
        mapFilePath.chop(4);
        mapFilePath += ".map";


        Viewer->generateTerrain(mtwFilePath, mapFilePath);
    }

    if (e->key() == Qt::Key_W) {
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(false);
        connect(timer, SIGNAL(timeout()), this, SLOT(testTimedFunction()) );
        timer->start(10);
    }
}
void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    Viewer->getScenePointer()->keyReleaseEvent(e);
}

void MainWindow::testTimedFunction()
{
    testValue+=0.05;

    QVector3D v;
    v.setX(sin(testValue) * 3 );
    v.setY(cos(testValue) * 3 );
    v.setZ(sin(testValue) * 4 );

    Viewer->MoveObjectByDir("default", v);
}
