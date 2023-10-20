QT += network serialport

INCLUDEPATH += $$PWD/
			
CONFIG(debug, debug|release) {
    LIBS += $$PWD/../../lib/libGUId.a
    PRE_TARGETDEPS += $$PWD/../../lib/libCommunicationd.a
}

CONFIG(release, debug|release) {
    LIBS += $$PWD/../../lib/libGUI.a
    PRE_TARGETDEPS += $$PWD/../../lib/libCommunication.a
}
