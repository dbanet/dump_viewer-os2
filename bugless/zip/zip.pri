

HEADERS += zip/ioapi.h \
           zip/qzip.h \
           zip/crypt.h \
           zip/unzip.h \
           zip/qiodeviceapi.h

SOURCES += zip/ioapi.c \
           zip/qzip.c \
           zip/unzip.c \
           zip/qiodeviceapi.cpp
           

# win32 does not have zlib by default
# use the version supplied with Qt
# if this is a Qt3 build, zlib is already built in
win32 {
    DEFINES += _CRT_SECURE_NO_WARNINGS

    ZLIB_PATH = $$(QTDIR)/src/3rdparty/zlib
    INCLUDEPATH += $$ZLIB_PATH
    !qt3:SOURCES += $${ZLIB_PATH}/adler32.c \
                    $${ZLIB_PATH}/crc32.c \
                    $${ZLIB_PATH}/infback.c \
                    $${ZLIB_PATH}/inflate.c \
                    $${ZLIB_PATH}/uncompr.c \
                    $${ZLIB_PATH}/compress.c \
                    $${ZLIB_PATH}/deflate.c \
                    $${ZLIB_PATH}/gzio.c \
                    $${ZLIB_PATH}/inffast.c \
                    $${ZLIB_PATH}/inftrees.c \
                    $${ZLIB_PATH}/trees.c \
                    $${ZLIB_PATH}/zutil.c
}
