QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/chart_widget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/port.cpp \
    src/wave_generator.cpp \
    libs/qcustomplot/qcustomplot.cpp

HEADERS += \
    src/chart_widget.h \
    src/mainwindow.h \
    src/port.h \
    src/wave_generator.h \
    libs/qcustomplot/qcustomplot.h \

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

RC_ICONS = icons/ico.ico
