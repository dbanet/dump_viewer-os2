
#pragma once

// std
#include <cmath>

// qt
#include <qiodevice.h>
#include <qdatetime.h>

namespace bugless
{
namespace tar
{

// Tar Header information

/* from POSIX 1003.1-1990.
  
field       size           offset   type      'default'     notes                       
name        [100];         0        char                    file name
mode        [8];           100      octal                   nine bits specifying file permissions and three bits to specify the Set UID, Set GID, and Save Text (sticky) mode
uid         [8];           108      octal       0           id of user owner
gid         [8];           116      octal       0           id of group owner
size        [12];          124      octal                   size of file in bytes
mtime       [12];          136      octal                   time last modified, seconds since January 1, 1970, 00:00 
checksum    [8];           148      octal                   checksum of this header (checksum is calculated as if this field all spaces)
typeflag;   [1]            156      octal       '0'         type of file archived
linkname    [100];         157      char        -           name of linked to file (if symbolic link)
magic       [6];           257      char        -           "ustar " - prefix name with prefix/
version     [2]            263      octal       '00'
uname       [32];          265      char        -           user name, used in preference to uid if 'magic'
gname       [32];          297      char        -           group name, used in preference to uid if 'magic'
devmajor    [8];           329      octal       -
devminor    [8];           337      octal       -
prefix      [155];         345      octal       -           prefix for name if 'magic'
                           500

The name, linkname, magic, uname, and gname are null-terminated 
character strings. All other fields are zero-filled octal numbers
in ASCII. Each numeric field of width w contains w minus 1 digits, and a null.

Checksum has field width 8.  6 bytes of zero-filled octal followed by nul then sp
*/

const int HEADER_SIZE = 512;

// files get padded up to nearest integer blocks
const int BLOCK_SIZE = 512;

//! basic header information
struct TarHeader
{
    TarHeader() : m_pos(0), m_size(0) {} 
    TarHeader( QString _name ) : m_name(_name), m_pos(0), m_size(0), m_time( QDateTime::currentDateTime() ) {} 
    
    QString name() const { return m_name; }
    void setName( QString s ) { m_name = s; }

    int pos() const { return m_pos; }
    void setPos( int pos ) { m_pos = pos; }

    //! size of contained data
    int size() const { return m_size; }
    void setSize( int size ) { m_size = size; }

    QDateTime time() const { return m_time; }
    void setTime( QDateTime t ) { m_time = t; }

    //! size of data block
    /*!
        data size rounded up to multiple of BLOCK_SIZE
    */
    int blockSize() const
    {
        double s(m_size);
        return ceil( s/BLOCK_SIZE) * BLOCK_SIZE;
    }

    //! total size of tar entry
    /*!
        header + data block
    */
    int totalSize() const
    {
        return HEADER_SIZE + blockSize();
    }

    //! expected start pos of next header
    /*!
        \todo handle long names / links?
    */
    int nextHeaderPos() const
    {
        return m_pos + totalSize();
    }
private:
    QString m_name;
    int m_pos;  //! Position in the tar archive.
    int m_size;    //! Size of the data that comes after the header.
    QDateTime m_time;

};


//! Test a Tar archive for a valid checksum in its header block;
/*!
    \param tar QIODevice for the Tar archive.
    \param pos start pos of the header block
*/
bool validChecksum( QIODevice* tar, int pos = 0 );


//! Read a tar header.
/*!
    \returns trus if successful
*/
bool readHeader( QIODevice* dev, TarHeader& info );

//! Write out a header.
/*!
    \returns trus if successful
*/
bool writeHeader( QIODevice* dev, const TarHeader& info );

//! Pad out file to n*BLOCK_SIZE
/*!
    \returns trus if successful
*/
bool writeBlocks( QIODevice* dev, const TarHeader& info );

//! Add the end of file marker.
/*!
    At the end of the archive file there are two 512-byte blocks
    filled with binary zeros as an end-of-file marker
*/
bool addEOFMarker( QIODevice* dev );

//! Is there an end-of-file marker
bool haveEOFMarker( QIODevice* dev );


} // of namespace tar


} // of namespace bugless