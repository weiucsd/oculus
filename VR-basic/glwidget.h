#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

#include <QGLBuffer>
#include <QGLShaderProgram>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget( const QGLFormat& format, QWidget* parent = 0 );

protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    virtual void paintGL();
    void draw();

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

signals:
    // signaling rotation from mouse movement
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

private:
    float scale = 1;
    int xRot = 0;
    int yRot = 0;
    int zRot = 0;
    float deltaX = 0;
    float deltaY = 0;
    QPoint lastPos;
};

#endif // GLWIDGET_H
