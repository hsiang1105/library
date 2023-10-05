#-------------------------------------------------
#
# Project created by QtCreator 2023-10-02T09:00:00
#
#-------------------------------------------------

QT += core

DEFINES += DLL_EXPORTS

TEMPLATE = lib
TARGET   = prot_fatak_ascii
DESTDIR  = $$PWD/bin/

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

include($$PWD/../../../../include/protocol_base/protocol_base.pri)
include($$PWD/src.pri)
win32{
    QMAKE_POST_LINK += $$PWD/copy_reference_files.bat
}
