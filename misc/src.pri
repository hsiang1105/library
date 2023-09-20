INCLUDEPATH +=  $$PWD/include/   \
                $$PWD/include/form

SOURCES += \
    $$PWD/backdoor.cpp \
    $$PWD/settings.cpp

HEADERS  += \
    $$PWD/include/backdoor.h \
    $$PWD/include/settings.h

DISTFILES += \
    $$PWD/copy_reference_files.bat \
    $$PWD/include/misc.pri
