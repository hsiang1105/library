INCLUDEPATH +=  \
                $$PWD/include/

SOURCES +=  \
    $$PWD/comm_connection.cpp \
    $$PWD/comm_parameter.cpp \
    $$PWD/comm_client.cpp \
    $$PWD/communication.cpp \
    $$PWD/comm_recorder.cpp

HEADERS  += \
    $$PWD/comm_connection.h \
    $$PWD/comm_parameter.h \
    $$PWD/comm_client.h \
    $$PWD/communication.h \
    $$PWD/comm_recorder.h

FORMS += 

DISTFILES += \
             $$PWD/copy_reference_files.bat \
             $$PWD/include/communication.pri
