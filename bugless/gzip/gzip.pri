
VPATH += gzip
INCLUDEPATH += gzip

HEADERS += \
    gzip.h

SOURCES += \
    gzip.cpp
               
win32 {
    contains( QMAKE_COMPILER_DEFINES, "_MSC_VER=1500" ) {
        # vsnprint declarations are an issue in VS2008
        DEFINES += NO_vsnprintf
    }
}
