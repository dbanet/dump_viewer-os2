
#include "tar/Tar.h"

// qt
#include <qglobal.h>
#include <qbytearray.h>

// std
#include <cstdio>

namespace bugless
{

namespace tar
{

const int CHECK_SUM_OFFSET = 148;
const int CHECK_SUM_SIZE = 8;
const char* CHECK_SUM_SPACES = "        ";  // 8 spaces - not nul terminated
const char* USTAR = "ustar ";  // ustar+sp
const char* NORMAL_TYPE_FLAG = "0";
const char* NUL = "\0";
const char* SP = " ";
const char* VERSION = "00";
// extension
const char* LONG_LINK = "././@LongLink";

void dumpOctalString( const QString& header )
{
    printf( "  '" );
    for ( int i=0; i<header.size(); ++i )
    {
#ifdef QT3
        printf( "%c", header.at(i) );
#else
        printf( "%c", header.at(i).toAscii() );
#endif
    }
    printf( "'\n" );
}


void dumpHeader( const QByteArray& header )
{
    Q_ASSERT( header.size() == HEADER_SIZE );
    for ( int i=0; i<HEADER_SIZE; ++i )
    {
        printf( "%c", header.at(i) );
        if ( (i+1)%64 == 0 )
            printf( "\n" );
        else if ( (i+1)%8 == 0 )
            printf( "-" );
    }
    printf( "\n" );
}


void dumpHeader( const TarHeader& header )
{
    qDebug( "Header info: " );
    qDebug( "Name:    %s", qPrintable( header.name() ) );
    qDebug( "Size:    %d", header.size() );
    qDebug( "M. Time: %s", qPrintable( header.time().toString() ) );
}

// calculates checksum of first 512 bytes.
// treates bytes [148-155] as spaces
unsigned int getTarHeaderChecksum( QByteArray& header )
{
    unsigned int result(0);
    for ( int i=0; i< header.size(); ++i )
    {
        if ( i < CHECK_SUM_OFFSET || i >= (CHECK_SUM_OFFSET+CHECK_SUM_SIZE) )
            result += header[i];
        else 
            result += ' ';
    }

    return result;
}

bool validChecksum( QIODevice* tar, int pos )
{
    // peek the contents of teh file at the given pos
    // sum the first 512 bytes.  8 bytes starting at offset 512
    // are treated as though they were spaces
    tar->seek( pos );
    QByteArray peek = tar->peek( HEADER_SIZE ); 
    if ( peek.size() != HEADER_SIZE )
    {
        return false;
    }

    bool ok;
    // byte-->string-->int ensures that various ways libraries write
    // checksums is handled (some do 6-sp-nul instead of 6-nul-sp)
    const int checksum1 = QString( peek.mid( CHECK_SUM_OFFSET, CHECK_SUM_SIZE ) ).toInt( &ok, 8 );
    const int checksum2 = getTarHeaderChecksum( peek );

    return ok&&(checksum1 == checksum2);
}

unsigned int toInt( const QString& str )
{
    bool ok;
    unsigned int num = str.toInt( &ok, 8 );
    return num;
}

QString toOctalString6( unsigned int num )
{
    //qDebug( "ToOctalString12: %d", num );
    //QString result = QString( "%1" ).arg( num, 6, 8, QChar( '0' ) );
    //dumpOctalString( result );

    return QString( "%1" ).arg( num, 6, 8, QChar( '0' ) );
}

QString toOctalString8( unsigned int num )
{
    //qDebug( "ToOctalString12: %d", num );
    //QString result = QString( "%1" ).arg( num, 7, 8, QChar( '0' ) );
    //dumpOctalString( result );

    return QString( "%1" ).arg( num, 7, 8, QChar( '0' ) );

}

QString toOctalString12( unsigned int num )
{
    //qDebug( "ToOctalString12: %d", num );
    //QString result = QString( "%1" ).arg( num, 11, 8, QChar( '0' ) );
    //dumpOctalString( result );

    return QString( "%1" ).arg( num, 11, 8, QChar( '0' ) );
}

// assumes dev is open for write.
// pos is start position of header. 
// after writing QIODevice's pos will be += 512
bool writeHeader( QIODevice* dev, const TarHeader& info )
{
    Q_ASSERT( dev->isWritable() );

    Q_ASSERT( !info.name().isEmpty() );
    Q_ASSERT( info.time().isValid() );
    // build up a QByteArray containing the header data and write that in
    // one go to the QIODevice.

    // maximum of 100 chars for name
    // \todo support long link 'L' type flags
    QString name = info.name();
    if ( name.size() > 100 )
    {
        qWarning( "truncating file name to 100 characters" );
        name = name.left( 100 );
    }

    QByteArray header( HEADER_SIZE, '\0' );

    // header start pos
    const int pos = info.pos();
    // offset into header
    int offset = 0;

    //name        [100];         0        char                    file name
    header.replace( offset, name.size(), name.toAscii().data() );
    offset += 100;
    
    //mode        [8];           100      octal                   nine bits specifying file permissions and three bits to specify the Set UID, Set GID, and Save Text (sticky) mode
    header.replace( offset, 8-1, toOctalString8( 0777 ).toAscii().data() );
    offset += 8;
    
    //uid         [8];           108      octal       0           id of user owner
    header.replace( offset, 8-1, toOctalString8( 0 ).toAscii().data() );
    //header.replace( offset, 8-1, QString(info.userId).toAscii() );
    offset += 8;
    
    //gid         [8];           116      octal       0           id of group owner
    header.replace( offset, 8-1, toOctalString8( 0 ).toAscii().data() );
    //header.replace( offset, 8-1, QString(info.groupId).toAscii() );
    offset += 8;
    
    //size        [12];          124      octal                   size of file in bytes
    header.replace( offset, 12-1, toOctalString12( info.size() ).toAscii().data() );
    offset += 12;
    
    //mtime       [12];          136      octal                   time last modified, seconds since January 1, 1970, 00:00 
    header.replace( offset, 12-1, toOctalString12( info.time().toTime_t() ).toAscii().data() );
    offset += 12;

    //checksum    [8];           148      octal                   checksum of this header (checksum is calculated as if this field all spaces)
    Q_ASSERT( offset == CHECK_SUM_OFFSET );
    header.replace( offset, CHECK_SUM_SIZE, CHECK_SUM_SPACES );
    offset += CHECK_SUM_SIZE;
    
    //typeflag;   [1]            156      octal       '0'         type of file archived
    header.replace( offset, 1, NORMAL_TYPE_FLAG );
    offset += 1;
    
    //linkname    [100];         157      char        -           name of linked to file (if symbolic link)
    offset += 100;
    
    //magic       [6];           257      char        -           "ustar" - prefix name with prefix/
    //header.replace( offset, 6, USTAR );
    offset += 6;
    
    //version     [2]            263      octal   
    header.replace( offset, 2, VERSION );
    offset += 2;
    
    //uname       [32];          265      char        -           user name, used in preference to uid if 'magic'
    //header.replace( offset, 4, info.userName.toAscii() );
    offset += 32;
    
    //gname       [32];          297      char        -           group name, used in preference to uid if 'magic'
    //header.replace( offset, 4, info.groupName.toAscii() );
    offset += 32;
    
    //devmajor    [8];           329      octal       -
    offset += 8;
    
    //devminor    [8];           337      octal       -
    offset += 8;
    
    //prefix      [155];         345      octal       -          prefix for name if 'magic'
    offset += 155;
    
    offset += 12;

    Q_ASSERT( offset == HEADER_SIZE );
    // * checksum
    const unsigned int checksum = getTarHeaderChecksum( header );
    header.replace( CHECK_SUM_OFFSET, 6, toOctalString6( checksum ).toAscii().data() );
    qstrncpy( header.data()+CHECK_SUM_OFFSET+6, NUL, 1 );
    header.replace( CHECK_SUM_OFFSET+7, 1, SP );
    //dumpHeader( header );

    dev->seek( pos );
    if ( dev->write( header.data(), HEADER_SIZE ) != HEADER_SIZE )
    {
        return false;
    }

    // extra checks
#ifdef DEBUG
    {
        Q_ASSERT( dev->pos() == pos+HEADER_SIZE );
        // read the header back
        dev->seek( pos );
        QByteArray test = dev->read( HEADER_SIZE );
        Q_ASSERT( test == header );
    }
#endif

    return true;
}

// assumes dev is oen for read
bool readHeader( QIODevice* dev, TarHeader& info )
{
    Q_ASSERT( dev->isReadable() );
    // read out the segments of interest: 
    
    // start pos of header
    const int pos = info.pos();

    if ( dev->size() < pos+HEADER_SIZE )
    {
        return false;
    }

    // offset into header
    int offset(0);
    
    //name        [100];         0        char                    file name
    dev->seek( pos+offset );
    info.setName( QString( dev->read( 100 ) ) );
    offset += 100;
    
    //mode        [8];           100      octal                   nine bits specifying file permissions and three bits to specify the Set UID, Set GID, and Save Text (sticky) mode
    //QByteArray modeData = dev->read( 8 );
    offset += 8;
    
    //uid         [8];           108      octal       0           id of user owner
    //QByteArray uidData = dev->read( 8 );
    offset += 8;
    
    //gid         [8];           116      octal       0           id of group owner
    //QByteArray gidData = dev->read( 8 );
    offset += 8;
    
    //size        [12];          124      octal                   size of file in bytes
    dev->seek( pos+offset );
    info.setSize( toInt( QString( dev->read( 12 ) ) ) );
    offset += 12;
    
    //mtime       [12];          136      octal                   time last modified, seconds since January 1, 1970, 00:00 
    info.setTime( QDateTime::fromTime_t( toInt( QString( dev->read( 12 ) ) ) ) );
    offset += 12;

    //checksum    [8];           148      octal                   checksum of this header (checksum is calculated as if this field all spaces)
    Q_ASSERT( offset == CHECK_SUM_OFFSET );
    offset += CHECK_SUM_SIZE;
    
    //typeflag;   [1]            156      octal       '0'         type of file archived
    char typeFlag; 
    dev->getChar( &typeFlag );
    offset += 1;
    
    //linkname    [100];         157      char        -           name of linked to file (if symbolic link)
    offset += 100;
    
    //magic       [6];           257      char        -           "ustar" - prefix name with prefix/
    dev->seek( pos+offset );
    QByteArray magicData = dev->read( 6 );
    offset += 6;
    
    //version     [2]            263      octal   
    offset += 2;
    
    //uname       [32];          265      char        -           user name, used in preference to uid if 'magic'
    offset += 32;
    
    //gname       [32];          297      char        -           group name, used in preference to uid if 'magic'
    offset += 32;
    
    //devmajor    [8];           329      octal       -
    offset += 8;
    
    //devminor    [8];           337      octal       -
    offset += 8;
    
    //prefix      [155];         345      octal       -          prefix for name if 'magic'
    dev->seek( pos+offset );
    const QString prefix = QString( dev->read( 155 ) );
    offset += 155;
    
    offset += 12;

    Q_ASSERT( offset == HEADER_SIZE );

    // \todo handle type flags, magic and prefix
    // long names by using the name "././@LongLink"
    // typeFlag of 'L' indicates that the next file has a long file name
    // this files data section contains the file name
    // and the actual file data is in the next file

    // 

    return true;
}

bool writeBlocks( QIODevice* dev, const TarHeader& info )
{
    Q_ASSERT( dev->isWritable() );

    // build up a QByteArray containing the block padding and write that in
    // one go to the QIODevice.
    
    const int paddingSize = info.blockSize()-info.size();
    if ( paddingSize <= 0 )
    {
        return true;
    }
    QByteArray padding( paddingSize, '\0' );

    const int pos = info.pos() + HEADER_SIZE + info.size();
    dev->seek( pos );
    if ( dev->write( padding.data(), paddingSize ) != paddingSize )
    {
        return false;
    }

    return true;
}

bool addEOFMarker( QIODevice* dev )
{
    Q_ASSERT( dev->isWritable() );
    if ( !dev->seek( dev->size() ) )
    {
        return false;
    }
    QByteArray endBlock( 2*BLOCK_SIZE, '\0' );
    if ( dev->write( endBlock ) != 2*BLOCK_SIZE )
    {
        return false;
    }

    return true;
}


bool haveEOFMarker( QIODevice* dev )
{
    Q_ASSERT( dev->isReadable() );
    Q_ASSERT( dev->isWritable() );

    // see if final 1024 bytes are \0
    if ( dev->size() < 2*BLOCK_SIZE )
    {
        return false;
    }

    if ( (dev->size() % BLOCK_SIZE) != 0 )
    {
        return false;
    }

    if ( !dev->seek( dev->size() - 2*BLOCK_SIZE ) )
    {
        return false;
    }

    QByteArray end = dev->peek( 2*BLOCK_SIZE );
    if ( end == QByteArray( 2*BLOCK_SIZE, '\0' ) )
    {
        return true;
    }

    return false;
}


} // of namespace tar

} // of namespace bugless
