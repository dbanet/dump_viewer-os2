
#pragma once

// std
#include <cmath>

// qt
#include <qbytearray.h>

namespace bugless { namespace gzip {

/*!
    Utility functions for compressing and decompressing gz files.

    Based initially on Qt's qCompress and qUncompress functions
    but using zlib functions deflateInit2 and inflateInit2 to
    allow gz style headers.
*/

QByteArray compress( const QByteArray& data );

QByteArray uncompress( const QByteArray& data );

void test();

} } // of namespace bugless::gzip
