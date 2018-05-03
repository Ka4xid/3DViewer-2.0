#include "mainwindow.h"
#include <QFileDialog>
#include <qmath.h>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setGeometry(100, 100, 1280, 720);

    Viewer = new ViewerClass(this, this);

    Viewer->TranslateObject("test", QVector3D());

    Viewer->SetCameraPosition(QVector3D(6166111.571, 7412153.522, 0));

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
//        for (int n=0; n<=360; n+=10) {
//            points.append(QVector3D(cos(n * 0.0174533) * 30,
//                                    sin(n * 0.0174533) * 30,
//                                    sin(n * 0.0174533) * 30 ));
//        }

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
        QString filePath = QFileDialog::getOpenFileName(this,
                                                        "Select Model",
                                                        "/home/fuego",
                                                        "Stanford PLY files (*.ply)");
        QString texturePath = filePath;
        texturePath.chop(4);
        texturePath += ".png";

        Viewer->CreateObject()->setTexturePath(texturePath)
                              ->setModelFilePath(filePath)
                ->setScale(QVector3D(1,1,1))
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
    v.setX(sin(testValue) /3);
    v.setY(cos(testValue) /3);
    v.setZ(sin(testValue) /4);

    Viewer->MoveObjectByDir("default", v);
}