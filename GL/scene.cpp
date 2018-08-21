#include "scene.h"
#include "qmath.h"
#include <QDebug>
#include <QTimer>

#define MOVE_STEP 5000
#define SENSITIVITY 3


Scene::Scene(QWidget *parent) : QGLWidget(parent)
{
    this->makeCurrent();

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()) );
    timer->start(40);

    this->setMouseTracking(true);

    UserInput.mouseMoveDelta = QPointF();
    UserInput.mouseMovePrev = QPointF();
    UserInput.mouseWheelDelta = 0;

    wireFrameMode = false;
}


void Scene::initializeGL()
{
    initializeGLFunctions(this->context());

    glViewport(0, 0, this->width(), this->height());

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    SetPerspective(50, 3, 10000, ( (float)(this->width()) / (float)(this->height()) ));

    glClearColor(0.5, 0.5, 0.5, 1);
}

void Scene::resizeGL(int w, int h)
{
    glViewport(0,0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    SetPerspective(50, 300, 50000, ((float)(this->width()) / (float)(this->height())));
}

void Scene::SetPerspective(float fov, float Znear, float Zfar, float aspect)
{
    float radians_fov,
          tangent,
          VPlane,
          HPlane;

    radians_fov = tan( fov * 3.1415 / 180);
    radians_fov = 2.0 * atan( tan( radians_fov * 0.5 ) / aspect ) ;
    tangent = tan(radians_fov / 2.0);
    VPlane = Znear * tangent;
    HPlane = VPlane * aspect;

    glFrustum(-HPlane, HPlane, VPlane, -VPlane, Znear, Zfar);
}

void Scene::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-90, 0, 0, 1);

    glScalef(0.1, 0.1, 0.1);

    if (camera != NULL ) {
        camera->Process();
    }

    emit RenderSignal();

    swapBuffers();
    fetchGLerrors();
    handleInputs();
}


void Scene::renderTestAxis(float m)
{
    glScalef(m,m,m);

    glBegin(GL_LINES);

        qglColor(Qt::red);      // X
        glVertex3f(0,0,0);
        glVertex3f(1,0,0);

        qglColor(Qt::green);    // Y
        glVertex3f(0,0,0);
        glVertex3f(0,1,0);

        qglColor(Qt::blue);     // Z
        glVertex3f(0,0,0);
        glVertex3f(0,0,1);

    glEnd();
}

void Scene::setCamera(CameraClass *newCamera)
{
    camera = newCamera;
    emit NewCameraPos(camera->GetCameraPos());
}

CameraClass* Scene::getCamera()
{
    return camera;
}

void Scene::handleInputs()
{
    if ( !UserInput.keyMap.value(Qt::Key_F1, 1) ) {
        wireFrameMode = !wireFrameMode;
        if (wireFrameMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        return;
    }

    float forward = UserInput.keyMap.value(Qt::Key_Up, 0) - UserInput.keyMap.value(Qt::Key_Down, 0);
    float strafe = UserInput.keyMap.value(Qt::Key_Right, 0) - UserInput.keyMap.value(Qt::Key_Left, 0);
    float up = UserInput.keyMap.value(Qt::Key_PageUp, 0) - UserInput.keyMap.value(Qt::Key_PageDown, 0);

    camera->Move(QVector3D(forward, strafe, up) * MOVE_STEP);
    camera->ProcessMouseInput(UserInput.mouseMoveDelta * SENSITIVITY);
    camera->ProcessMouseWheelInput(UserInput.mouseWheelDelta);

    UserInput.mouseWheelDelta = 0;
    UserInput.mouseMoveDelta = QPointF(0,0);
}


void Scene::fetchGLerrors()
{
    GLenum error = glGetError();
    if (error != 0) {
        qDebug() << "OpenGL Error:" << error;
    }
}
void Scene::keyPressEvent(QKeyEvent *e)
{
    UserInput.keyMap.insert(e->key(), 1);
}
void Scene::keyReleaseEvent(QKeyEvent *e)
{
    if ( e->key() == Qt::Key_F1 ) {
        wireFrameMode = !wireFrameMode;
        if (wireFrameMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        return;
    }

    UserInput.keyMap.remove(e->key());
}
void Scene::wheelEvent(QWheelEvent *e)
{
    if (e->delta() > 0) {
        UserInput.mouseWheelDelta = 1;
    } else {
        UserInput.mouseWheelDelta = -1;
    }
}
void Scene::mouseMoveEvent(QMouseEvent *e)
{
    QPointF newMouseMove = QPointF(e->x(), e->y());

    if (e->buttons() & Qt::LeftButton) {
        UserInput.mouseMoveDelta = UserInput.mouseMovePrev- newMouseMove;
    }

    UserInput.mouseMovePrev = newMouseMove;
}
