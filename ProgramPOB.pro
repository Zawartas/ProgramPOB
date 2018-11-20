#-------------------------------------------------
#
# Project created by QtCreator 2018-11-17T10:18:53
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProgramPOB
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialoguiparameters.cpp \
    histogram.cpp

HEADERS += \
        mainwindow.h \
    dialoguiparameters.h \
    histogram.h

FORMS += \
    dialoguiparameters.ui \
    histogram.ui \
    mainwindow.ui

INCLUDEPATH += C:\opencv-build\install\include

LIBS += C:\opencv-build\bin\libopencv_core400.dll
LIBS += C:\opencv-build\bin\libopencv_highgui400.dll
LIBS += C:\opencv-build\bin\libopencv_imgcodecs400.dll
LIBS += C:\opencv-build\bin\libopencv_imgproc400.dll
LIBS += C:\opencv-build\bin\libopencv_features2d400.dll
LIBS += C:\opencv-build\bin\libopencv_calib3d400.dll
LIBS += C:\opencv-build\bin\libopencv_ml400.dll #library for CSV import

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
