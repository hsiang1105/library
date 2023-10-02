#-------------------------------------------------
#
# Project created by QtCreator 2019-01-30T09:36:00
#
#-------------------------------------------------

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += staticlib
TEMPLATE = lib
TARGET = ProtocolBase
DEFINES += DLL_EXPORTS

DESTDIR = $$PWD/../../../../lib/

include($$PWD/src.pri)

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

win32{
    QMAKE_POST_LINK = $$PWD/copy_reference_files.bat
}
