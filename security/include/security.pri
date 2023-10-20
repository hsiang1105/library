INCLUDEPATH += $$PWD/

HEADERS  += \
            $$PWD/security_center.h

CONFIG(debug, debug|release) {
    LIBS += $$PWD/../../lib/libSecurityd.a
    PRE_TARGETDEPS += $$PWD/../../lib/libSecurityd.a
}

CONFIG(release, debug|release) {
    LIBS += $$PWD/../../lib/libSecurity.a
    PRE_TARGETDEPS += $$PWD/../../lib/libSecurity.a
}

include($$PWD/../../external/OpenSSL/OpenSSL.pri)
