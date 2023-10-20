INCLUDEPATH += $$PWD/

HEADERS  += \
            $$PWD/protocol.h \
            $$PWD/protocol_element.h \
            $$PWD/protocol_property.h \
            $$PWD/abstract_protocol.h \
            $$PWD/abstract_message.h

CONFIG(debug, debug|release) {
    LIBS += $$PWD/../../lib/libProtocolBased.a
    PRE_TARGETDEPS += $$PWD/../../lib/libProtocolBased.a
}

CONFIG(release, debug|release) {
    LIBS += $$PWD/../../lib/libProtocolBase.a
    PRE_TARGETDEPS += $$PWD/../../lib/libProtocolBase.a
}
