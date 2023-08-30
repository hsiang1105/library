INCLUDEPATH += $$PWD/include/

SOURCES += \
    $$PWD/security_center.cpp \
    $$PWD/security_center_p.cpp \
    $$PWD/openssl_helper.cpp

HEADERS  += \
    $$PWD/include/security_center.h \
    $$PWD/security_center_p.h \
    $$PWD/openssl_helper.h

FORMS +=

DISTFILES += \
    $$PWD/copy_reference_files.bat \
    $$PWD/include/security.pri
