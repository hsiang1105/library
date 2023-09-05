#-------------------------------------------------
#
# Project created by QtCreator 2023-09-04T11:34:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += staticlib
TEMPLATE = lib
TARGET = Misc

DESTDIR = $$PWD/../../../lib/

include($$PWD/src.pri)

TRANSLATIONS =  $$PWD/../../../resource/languages/misc_en.ts

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

win32{
    QMAKE_POST_LINK = $$PWD/copy_reference_files.bat
}


