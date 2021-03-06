#include <QApplication>
#include <QGLFormat>

#include "glwidget.h"

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );

    QGLFormat glFormat;
    glFormat.setVersion( 3, 3 );
    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );

    // Create a GLWidget requesting our format
    GLWidget w( glFormat );
    w.show();

    return a.exec();
}
