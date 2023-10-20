#-------------------------------------------------
#
# Project created by QtCreator 2023-10-16T16:54:06
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += staticlib
TEMPLATE = lib
TARGET = Communication

DESTDIR = $$PWD/../../../lib/

include($$PWD/src.pri)
include($$PWD/external.pri)

TRANSLATIONS =  $$PWD/../../../resource/languages/communication_en.ts

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

win32{
    QMAKE_POST_LINK = $$PWD/copy_reference_files.bat
}
