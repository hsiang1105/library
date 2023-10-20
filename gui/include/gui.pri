INCLUDEPATH += $$PWD/

HEADERS  += \
    $$PWD/form_loading_spinner.h \
    $$PWD/form_network_addr_edit.h \
    $$PWD/form_capacity_bar.h \
    $$PWD/form_collapsed_widget.h \
    $$PWD/form_qt_message.h \
    $$PWD/form_qt_message_ex.h

CONFIG(debug, debug|release) {
    LIBS += $$PWD/../../lib/libGUId.a
    PRE_TARGETDEPS += $$PWD/../../lib/libGUId.a
}

CONFIG(release, debug|release) {
    LIBS += $$PWD/../../lib/libGUI.a
    PRE_TARGETDEPS += $$PWD/../../lib/libGUI.a
}
