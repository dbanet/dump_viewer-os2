TEMPLATE    = lib
LANGUAGE    = C++

QT -= gui

CONFIG -= shared
CONFIG += qt warn_on staticlib
#CONFIG += debug


DESTDIR = ../lib

CONFIG += zip
CONFIG += tar
CONFIG += gzip

HEADERS += ../include/Archive.h \
           ../include/ArchiveIterator.h \
           ArchiveDevice.h \
           ArchiveImpl.h \
           ArchiveFactory.h \
           IOCompressorImpl.h

SOURCES += Archive.cpp \
           ArchiveImpl.cpp \
           ArchiveDevice.cpp \
           ArchiveIterator.cpp \
           ArchiveFactory.cpp \
           IOCompressorImpl.cpp
           
message( "Bugless::Archive" )

debug {
    message( " - debug lib build" )
} else {
    message( " - release lib build" )
}

zip {
    message( " - adding ZIP support" )
    DEFINES += BUGLESS_ZIP
    HEADERS += ZipImpl.h \
               ZipDevice.h
    SOURCES += ZipImpl.cpp \
               ZipDevice.cpp

    include( zip/zip.pri )
}           

tar {
    message( " - adding TAR support" )
    DEFINES += BUGLESS_TAR
    include( tar/tar.pri )
}

gzip {
    message( " - adding GZIP support" )
    DEFINES += BUGLESS_GZIP
    include( gzip/gzip.pri )
}

include( util/util.pri )
           
INCLUDEPATH += . \
               ../include

TARGET = archive

debug {
    OBJECTS_DIR = debug
    DEFINES += DEBUG
}

release {
    OBJECTS_DIR = release
}

win32-msvc* {
  # Warning level 3, treat warnings as error
  QMAKE_CXXFLAGS_WARN_ON = /W3 /WX
  
  # but allow some
  #QMAKE_CXXFLAGS += /wd4309  # truncation of const 
  #QMAKE_CXXFLAGS += /wd4512  # qobject assignement operator couldnoe be generated 
  #QMAKE_CXXFLAGS += /wd4127  # qlist conditional expression is constant
  #QMAKE_CXXFLAGS += /wd4131  # uses old-style declarator
  #QMAKE_CXXFLAGS += /wd4244  # conversion from 'unsigned int' to 'Bytef', possible loss of data
  QMAKE_CXXFLAGS += /wd4996  # This function or variable may be unsafe, consider using xxx_s instead
  
}

unix|win32-g++ {
  QMAKE_CXXFLAGS += -Wall -Werror
}

