#include "glwidget.h"

#include <QCoreApplication>
#include <QKeyEvent>

#include <iostream>
#include <shader.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <scene.h>
#include <postprocess.h>
#include <Importer.hpp>

#include "Shader.h"
#include "Model.h"

#include <SOIL.h>

using namespace std;

GLWidget::GLWidget( const QGLFormat& format, QWidget* parent )
    : QGLWidget( format, parent )
{
}

void GLWidget::initializeGL()
{
}

void GLWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void GLWidget::draw()
{
    Model ourModel("/Users/cky/Google/lab/VR-basic/iPhone5S/iPhone\ 5S.obj");
//    Model ourModel("/Users/cky/Google/lab/VR-basic/nanosuit/nanosuit.obj");

//    int width,height;
//    unsigned char* image = SOIL_load_image("/Users/cky/Google/lab/VR-basic/iPhone5S/Body1.png", &width, &height, 0, SOIL_LOAD_RGB);
//    unsigned char* image = SOIL_load_image("/Users/cky/Google/lab/VR-basic/nanosuit/hand_showroom_spec.png", &width, &height, 0, SOIL_LOAD_RGB);
//    cout<<*image<<endl;

    Shader shader("/Users/cky/Google/lab/VR-basic/simple.vert", "/Users/cky/Google/lab/VR-basic/simple.frag");

    shader.Use();

    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(deltaX, deltaY, 0.0f));
    trans = glm::rotate(trans, float(xRot)/128, glm::vec3(1.0, 0.0, 0.0));
    trans = glm::rotate(trans, float(yRot)/128, glm::vec3(0.0, 1.0, 0.0));
    trans = glm::rotate(trans, float(zRot)/128, glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(scale, scale, scale));
    GLuint transformLoc = glGetUniformLocation(shader.Program, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glm::mat4 model;
//    model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));	// It's a bit too big for our scene, so scale it down
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

    ourModel.Draw(shader);

}

void GLWidget::paintGL()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    draw();
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        default:
            QGLWidget::keyPressEvent(event);
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta();

    if (numDegrees>0) scale = scale * 1.2;
    else scale = scale * 0.8;
    updateGL();

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    }
    else if (event->buttons() & Qt::RightButton) {
        deltaX += float(dx)/300;
        deltaY -= float(dy)/300;
        updateGL();
    }

    lastPos = event->pos();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}
