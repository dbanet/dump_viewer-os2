/*
   based on ioapi.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API
*/

#include "zlib.h"
#include "qiodeviceapi.h"

// qt
#include <qiodevice.h>
#include <qfile.h>


// open the file \a filename and return a \a QIODevice*
voidpf ZCALLBACK fopen_qiodevice_func( voidpf io, const char* filename, int mode )
{
    QIODevice::OpenMode omf( QIODevice::NotOpen );
    if ( (mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ )
    {
        omf = QIODevice::ReadOnly;
    }
    else if ( mode & ZLIB_FILEFUNC_MODE_EXISTING )
    {
        omf = QIODevice::ReadWrite;
    }
    else if ( mode & ZLIB_FILEFUNC_MODE_CREATE )
    {
        omf = QIODevice::ReadWrite | QIODevice::Truncate;
    }

    QIODevice* file( NULL );

    if ( io )
    {
        file = static_cast<QIODevice*>(io);
    }
    if ( !file && filename )
    {
        file = new QFile( filename );
    }

    if ( omf != QIODevice::NotOpen )
    {
        file->open( omf );
    }

    return file;
}

/// reads at most \a size bytes from \a stream into \a buffer
uLong ZCALLBACK fread_qiodevice_func( voidpf, voidpf stream, void* buf, uLong size )
{
    uLong ret(0);
    ret = static_cast<QIODevice*>(stream)->read( (char*)buf, size );

    return ret;
}

/// writes at most \a size bytes from \a buffer into \a stream
uLong ZCALLBACK fwrite_qiodevice_func( voidpf, voidpf stream, const void* buf, uLong size )
{
    uLong ret(0);
    ret = static_cast<QIODevice*>(stream)->write( (char*)buf, size );

    return ret;
}

/// current position in \a stream
long ZCALLBACK ftell_qiodevice_func( voidpf, voidpf stream )
{
    return static_cast<QIODevice*>(stream)->pos();
}

/// set the file position in \a stream to \a offset relative to \a origin
long ZCALLBACK fseek_qiodevice_func( voidpf, voidpf stream, uLong offset, int origin )
{
    QIODevice* d = static_cast<QIODevice*>( stream );

    // QIODevice seek method just takes an absolute offset
    long absOffset(0);
    switch ( origin )
    {
        case ZLIB_FILEFUNC_SEEK_SET:
        {
            d->reset();
            absOffset = offset;
            break;
        }
        case ZLIB_FILEFUNC_SEEK_END:
        {
            d->reset();
            absOffset = d->size() - 1 - offset;
            break;
        }
        case ZLIB_FILEFUNC_SEEK_CUR:
        {
            absOffset = offset;
            break;
        }
        default:
        {
            return -1;
        }
    }

    if ( !d->seek( absOffset ) )
    {
        return -1;
    }

    return 0;
}

int ZCALLBACK fclose_qiodevice_func( voidpf, voidpf stream )
{
    static_cast<QIODevice*>(stream)->close();

    return 0;
}

int ZCALLBACK ferror_qiodevice_func( voidpf, voidpf )
{
    // no qt equivalent
    return 0;
}

void qiodevice_fill_fopen_filefunc( zlib_filefunc_def* pzlib_filefunc_def, voidpf stream )
{
    pzlib_filefunc_def->zopen_file  =  fopen_qiodevice_func;
    pzlib_filefunc_def->zread_file  =  fread_qiodevice_func;
    pzlib_filefunc_def->zwrite_file = fwrite_qiodevice_func;
    pzlib_filefunc_def->ztell_file  =  ftell_qiodevice_func;
    pzlib_filefunc_def->zseek_file  =  fseek_qiodevice_func;
    pzlib_filefunc_def->zclose_file = fclose_qiodevice_func;
    pzlib_filefunc_def->zerror_file = ferror_qiodevice_func;
    pzlib_filefunc_def->opaque      = stream;
}


