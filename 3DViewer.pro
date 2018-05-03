#-------------------------------------------------
#
# Project created by QtCreator 2018-04-17T08:29:28
#
#-------------------------------------------------

QT       += core gui opengl


LIBS += -lmapcomponents -lgsslink -lqdmapacces -ldl


TARGET = 3DViewer
TEMPLATE = app


SOURCES +=  main.cpp\
            mainwindow.cpp \
            GL/scene.cpp \
            viewer.cpp \
            GL/cameraclass.cpp \
            terrain/terrainhandler.cpp \
            objects/objectshandler.cpp \
            objects/genericobject.cpp \
            objects/objectbuilder.cpp \
            objects/objectcomposer.cpp \
            objects/plyparser.cpp \
            objects/trajectorybuilder.cpp \
            terrain/terraintexturegen.cpp \
            terrain/terrainobject.cpp \
            terrain/mtwparser.cpp \
            terrain/terrainbuilder.cpp

HEADERS  += mainwindow.h \
            GL/scene.h \
            viewer.h \
            GL/cameraclass.h \
            terrain/terrainhandler.h \
            objects/objectshandler.h \
            objects/genericobject.h \
            objects/objectbuilder.h \
            objects/objectcomposer.h \
            objects/plyparser.h \
            objects/trajectorybuilder.h \
            terrain/terraintexturegen.h \
            terrain/terrainobject.h \
            terrain/mtwparser.h \
            terrain/terrainbuilder.h

RESOURCES += \
    rsc.qrc

