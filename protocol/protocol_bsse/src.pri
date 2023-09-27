INCLUDEPATH +=  $$PWD/ \
                $$PWD/include

SOURCES +=  \
            $$PWD/protocol.cpp \
            $$PWD/protocol_element.cpp \
    $$PWD/protocol_property.cpp \
    $$PWD/abstract_protocol.cpp \
    $$PWD/abstract_message.cpp
       
HEADERS  += \
            $$PWD/protocol_namespace.h \
            $$PWD/include/protocol.h \
            $$PWD/include/protocol_element.h \
    $$PWD/include/protocol_property.h \
    $$PWD/include/abstract_protocol.h \
    $$PWD/include/abstract_message.h
    

DISTFILES +=    \
            $$PWD/copy_reference_files.bat \
            $$PWD/include/protocol_base.pri
