#-------------------------------------------------
#
# Project created by QtCreator 2013-09-19T22:47:43
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dump_viewer
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    torrent_hash_convert.cpp \
    arpmanetdc/base32.cpp \
    arpmanetdc/util.cpp \
    quazip/quagzipfile.cpp

HEADERS  += mainwindow.h \
    torrent_hash_convert.h \
    arpmanetdc/base32.h \
    arpmanetdc/util.h \
    quazip/quazip_global.h \
    quazip/quagzipfile.h

# bugless
INCLUDEPATH += bugless bugless/tar bugless/gzip bugless/util bugless/zip
DEFINES += BUGLESS_TAR BUGLESS_GZIP BUGLESS_ZIP
!win32:LIBS += -lz
SOURCES += bugless/tar/TarDevice.cpp \
    bugless/tar/Tar.cpp \
    bugless/tar/TarImpl.cpp \
    bugless/ZipDevice.cpp \
    bugless/ArchiveFactory.cpp \
    bugless/ArchiveImpl.cpp \
    bugless/gzip/gzip.cpp \
    bugless/ArchiveDevice.cpp \
    bugless/ArchiveIterator.cpp \
    bugless/util/dirUtils.cpp \
    bugless/util/fileUtils.cpp \
    bugless/ZipImpl.cpp \
    bugless/IOCompressorImpl.cpp \
    bugless/Archive.cpp \
    bugless/zip/qiodeviceapi.cpp \
    bugless/zip/unzip.c \
    bugless/zip/ioapi.c \
    bugless/zip/qzip.c
HEADERS += bugless/tar/TarDevice.h \
    bugless/tar/Tar.h \
    bugless/tar/TarImpl.h \
    bugless/ZipDevice.h \
    bugless/ArchiveFactory.h \
    bugless/ArchiveImpl.h \
    bugless/gzip/gzip.h \
    bugless/ArchiveDevice.h \
    bugless/ArchiveIterator.h \
    bugless/util/dirUtils.h \
    bugless/util/fileUtils.h \
    bugless/ZipImpl.h \
    bugless/IOCompressorImpl.h \
    bugless/Archive.h \
    bugless/zip/qiodeviceapi.h \
    bugless/zip/unzip.h \
    bugless/zip/ioapi.h \
    bugless/zip/qzip.h

FORMS    += mainwindow.ui

win32:RC_FILE = ressource.rc
RESOURCES = ressource.qrc
