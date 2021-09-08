INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

#-------------------------------------------------

HEADERS += \
    $$PWD/qcustomplot.h

SOURCES += \
    $$PWD/qcustomplot.cpp

