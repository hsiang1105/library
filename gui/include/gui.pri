INCLUDEPATH += $$PWD/
			
CONFIG(debug, debug|release) {
    LIBS += $$PWD/../../lib/libGUId.a
    PRE_TARGETDEPS += $$PWD/../../lib/libGUId.a
}

CONFIG(release, debug|release) {
    LIBS += $$PWD/../../lib/libGUI.a
    PRE_TARGETDEPS += $$PWD/../../lib/libGUI.a
}
