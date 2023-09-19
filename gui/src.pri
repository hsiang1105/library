INCLUDEPATH += $$PWD/include/

SOURCES += \
    $$PWD/form_loading_spinner.cpp \
    $$PWD/form_network_addr_edit.cpp \
    $$PWD/form_capacity_bar.cpp \
    $$PWD/form_fading_button.cpp \
    $$PWD/form_collapsed_widget.cpp \
    $$PWD/form_qt_message.cpp \
    $$PWD/form_qt_message_ex.cpp

HEADERS  += \
    $$PWD/include/form_loading_spinner.h \
    $$PWD/include/form_network_addr_edit.h \
    $$PWD/include/form_capacity_bar.h \
    $$PWD/form_fading_button.h \
    $$PWD/include/form_collapsed_widget.h \
    $$PWD/include/form_qt_message.h \
    $$PWD/include/form_qt_message_ex.h

FORMS += \
    $$PWD/form_qt_message.ui \
    $$PWD/form_qt_message_ex.ui

DISTFILES += \
    $$PWD/copy_reference_files.bat \
    $$PWD/include/gui.pri
