
QT       += core gui opengl

TARGET = VR-basic
TEMPLATE = app

SOURCES +=  \
            main.cpp \
            glwidget.cpp

HEADERS  += \
            glwidget.h \
                shader.h \
    mesh.h \
    model.h

OTHER_FILES +=  \
                simple.vert \
                simple.frag

INCLUDEPATH += /Users/cky/Documents/glm

INCLUDEPATH += /usr/local/include/assimp
LIBS += -L/usr/local/lib/ -lassimp

INCLUDEPATH += /opt/local/include/SOIL/
LIBS += -L/opt/local/lib/ -lSOIL

CONFIG += warn_off
