TEMPLATE = app

QT += qml quick widgets serialport

SOURCES += main.cpp \
    qgcui.cpp \
    qgccore.cpp \
    serialthread.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

INCLUDEPATH += libs\mavlink\include\mavlink\v1.0

HEADERS += \
    qgcui.h \
    qgccore.h \
    serialthread.h
