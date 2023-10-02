INCLUDEPATH += $$PWD/
			
CONFIG(debug, debug|release) {
    LIBS += $$PWD/../../lib/libProtocolBased.a
    PRE_TARGETDEPS += $$PWD/../../lib/libProtocolBased.a
}

CONFIG(release, debug|release) {
    LIBS += $$PWD/../../lib/libProtocolBase.a
    PRE_TARGETDEPS += $$PWD/../../lib/libProtocolBase.a
}
