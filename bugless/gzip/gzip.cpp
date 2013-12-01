#include <qglobal.h>
#include <qbytearray.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qbuffer.h>

#include <zlib.h>

#include "gzip/gzip.h"

#if !defined( DEF_MEM_LEVEL )
# if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
# else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
# endif
#endif

namespace {

// gzip format magic identifiers ID1 and ID2
const QChar gz_magic_0 = 0x1f;
const QChar gz_magic_1 = 0x8b;


// modified from zlib/compress.c to use deflateInit2. otherwise unchanged
int mycompress2 (Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    //err = deflateInit(&stream, level);
    // +16 == 'use gzip headers'
    err = deflateInit2(&stream, level,
                       Z_DEFLATED, MAX_WBITS+16, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (err != Z_OK) return err;

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = deflateEnd(&stream);
    return err;
}

// modified from zlib/uncompr.c to use inflateInit2. otherwise unchanged
int myuncompress (Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;

    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    //err = inflateInit(&stream);
    // +16 == 'use gzip headers'
    err = inflateInit2(&stream,MAX_WBITS+16);

    if (err != Z_OK) return err;

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            return Z_DATA_ERROR;
        return err;
    }
    *destLen = stream.total_out;

    err = inflateEnd(&stream);
    return err;
}

QByteArray compress_(const uchar* data, int nbytes, int compressionLevel)
{
    if (nbytes == 0) 
    {
        //return QByteArray(4, '\0');
        return QByteArray();
    }
    if (!data) 
    {
        qWarning("compress: Data is null");
        return QByteArray();
    }
    if (compressionLevel < -1 || compressionLevel > 9)
        compressionLevel = -1;

    // minimum size for compression is uncompressed size + 1% + 12
    ulong len = nbytes + nbytes / 100 + 13;
    QByteArray bazip;
    int res;
    do {
        bazip.resize(len);
        res = mycompress2((uchar*)bazip.data(), &len, (uchar*)data, nbytes, compressionLevel);

        switch (res) {
        case Z_OK:
            bazip.resize(len);
            break;
        case Z_MEM_ERROR:
            qWarning("compress: Z_MEM_ERROR: Not enough memory");
            bazip.resize(0);
            break;
        case Z_BUF_ERROR:
            len *= 2;
            break;
        }
    } while (res == Z_BUF_ERROR);

#ifdef QT_DEBUG
    Q_ASSERT( bazip.at(0) == gz_magic_0 );
    Q_ASSERT( bazip.at(1) == gz_magic_1 );
#endif
    return bazip;
}


QByteArray uncompress_(const uchar* data, int nbytes)
{
    if (!data) 
    {
        qWarning("uncompress: Data is null");
        return QByteArray();
    }
    
    if (nbytes <= 4) 
    {
        //    qWarning("uncompress: Input data is corrupted");
        return QByteArray();
    }

    // size of uncompressed data is stored in last four bytes.  lsb.
    const ulong expectedSize = (data[nbytes-4])        | (data[nbytes-3] << 8) |
                               (data[nbytes-2] <<  16) | (data[nbytes-1] << 24 );


    ulong len = qMax(expectedSize, 1ul);
    QByteArray baunzip;
    int res;
    do {
        baunzip.resize(len);
        res = myuncompress((uchar*)baunzip.data(), &len,
                            (uchar*)data, nbytes);

        switch (res) {
        case Z_OK:
            if ((int)len != baunzip.size())
                baunzip.resize(len);
            break;
        case Z_MEM_ERROR:
            qWarning("uncompress: Z_MEM_ERROR: Not enough memory");
            break;
        case Z_BUF_ERROR:
            // bigger than we thought.  increase buffer size.
            len *= 2;
            break;
        case Z_DATA_ERROR:
            qWarning("uncompress: Z_DATA_ERROR: Input data is corrupted");
            break;
        }
    } while (res == Z_BUF_ERROR);

    if (res != Z_OK)
        baunzip = QByteArray();

    return baunzip;
}

} // end of anonymous namespace

namespace bugless { namespace gzip {

QByteArray compress( const QByteArray& data )
{ 
    return compress_( reinterpret_cast<const uchar *>( data.constData() ), data.size(), -1 );
}


QByteArray uncompress( const QByteArray& data )
{ 
    if ( data.size()>2 && (data.at(0) != gz_magic_0 || data.at(1) != gz_magic_1) )
    {
        qWarning( "gz: data to be uncompressed doesn't look like gzip format" );
    }

    return uncompress_( reinterpret_cast<const uchar*>( data.constData() ), data.size() ); 
}

} } // of namespace bugless::gzip
