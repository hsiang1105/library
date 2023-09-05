INCLUDEPATH +=  $$PWD/ \
                $$PWD/form
			
CONFIG(debug, debug|release) {
    LIBS += $$PWD/../../lib/libMiscd.a
    PRE_TARGETDEPS += $$PWD/../../lib/libMiscd.a
}

CONFIG(release, debug|release) {
    LIBS += $$PWD/../../lib/libMisc.a
    PRE_TARGETDEPS += $$PWD/../../lib/libMisc.a
}
