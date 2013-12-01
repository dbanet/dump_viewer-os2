/* -*- mode: C++; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil -*- */
/**********************************************************************
**
** Copyright (C) 2007 Bugless Software Development Ltd..  All rights
** reserved.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included
** in the packaging of this file.
**
** Licensees holding valid Bugless Software Development Archive license
** may use this file in accordance with the Bugless Software Development
** Archive License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A
** PARTICULAR PURPOSE.
**
** information about Bugless Software Development License Agreements:
** See http://www.bugless.co.uk/products/archive/licence.html for Archive licensing information.
** See http://www.bugless.co.uk/gpl/ for GPL licensing information.
**
** Contact info@bugless.co.uk if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qstring.h>
#include <qdebug.h>
#include <qbuffer.h>

#include <zlib.h>

#include "util/fileUtils.h"
#include "util/QIODeviceCloser.h"
#include "ArchiveFactory.h"
#include "ZipDevice.h"

#include "ZipImpl.h"


namespace bugless
{

class ZipArchiveCreator : public ArchiveCreatorInterface
{
public:
    ZipArchiveCreator()
    {
        ArchiveFactory::instance()->registerArchiveType( Archive::ZIP, this );
    }

    virtual Archive::Type type() const
    {
        return Archive::ZIP;
    }

    virtual ArchiveImpl* create() const
    {
        return new ZipImpl;
    }
	
    virtual bool couldBe( QIODevice* dev ) const
    {
        QIODeviceCloser io( dev, QIODevice::ReadOnly );
        // peek the first two bytes for the magic values:
        // 0x50, 0x4b,
        char buf[2];
        if ( io->peek(buf, sizeof(buf)) == sizeof(buf) )
        {
             return (buf[0] == 0x50 && buf[1] == 0x4b);
        }

        return false;
    }

    virtual QString extension() const
    {
        return "zip";
    }
};

// global creator object
ZipArchiveCreator g_zipCreator;

ZipImpl::ZipImpl()
    : ArchiveImpl()
    , m_unzipFile(NULL)
    , m_zipFile(NULL)
{
    m_dev.reset( new ZipDevice( this ) );
}

ZipImpl::~ZipImpl()
{
}

bool ZipImpl::changeModeTo( const ArchiveImpl::Mode mode )
{
    // changing mode - close any files 
    if (m_unzipFile != NULL)
    {
        unzClose(m_unzipFile);
        m_unzipFile = NULL;
    }

    if (m_zipFile != NULL)
    {
        zipClose(m_zipFile, NULL);
        m_zipFile = NULL;
    }

    if ( m_dev.get() )
    {
        zipDevice()->setZipFileHandles( NULL, NULL );
    }

    if ( mode == ArchiveImpl::Closed )
    {
        // all done
        m_ioDevice->close();
        return true;
    }

    // Allocate the ZipDevice
    if ( !m_dev.get() )
    {
        m_dev.reset( new ZipDevice( this ) );
    }

    m_dev->setPassword( m_password );

    // Open the Zip archive
    switch (mode)
    {
        case ArchiveImpl::Closed:
        case ArchiveImpl::Remove:
            // Just to keep the compiler quiet
            break;

        case ArchiveImpl::Create:
        case ArchiveImpl::Add:
        {
            m_zipFile = zipOpenWithIODevice( m_ioDevice, APPEND_STATUS_ADDINZIP );
            if ( !m_zipFile )
            {
                m_zipFile = zipOpenWithIODevice( m_ioDevice, APPEND_STATUS_CREATE );
            }
            break;
        }
        case ArchiveImpl::Decompress:
        {
            m_unzipFile = unzOpenWithIODevice( m_ioDevice );
            break;
        }
    }

    // Check to see if opening the archive succeeded
    switch (mode)
    {
        case ArchiveImpl::Remove:
        case ArchiveImpl::Closed:
            // Just to keep the compiler quiet
            break;

        case ArchiveImpl::Create:
        case ArchiveImpl::Add:
            if (m_zipFile == NULL)
            {
                // Error
                qDebug("open: failed to open archive's QIODevice* for add/create" );
                return false;
            }
            break;

        case ArchiveImpl::Decompress:
            if (m_unzipFile == NULL)
            {
                // Error
                qDebug("open: failed to open archive's QIODevice* for decompress" );
                return false;
            }
            break;
    }

    // Set the file handle
    zipDevice()->setZipFileHandles(m_unzipFile, m_zipFile);

    return true;
}


bool ZipImpl::extractFile( const QString& fName, const QDir& dir, const QString& newName, QString* )
{
    QString outFileName;
    if ( !ArchiveImpl::extractFile( fName, dir, newName, &outFileName ) )
    {
        return false;
    }

    // Get the filestamp from the current file
    unz_file_info fInfo;
    if (unzGetCurrentFileInfo (m_unzipFile, &fInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK)
    {
        QDate date( fInfo.tmu_date.tm_year, fInfo.tmu_date.tm_mon, fInfo.tmu_date.tm_mday );
        QTime time( fInfo.tmu_date.tm_hour, fInfo.tmu_date.tm_min, fInfo.tmu_date.tm_sec );

        setFileTime( outFileName, QDateTime( date, time ) );
    }

    return true;
}


bool ZipImpl::firstFile()
{
    //qDebug("firstFile");

    if ( !ensureInMode( ArchiveImpl::Decompress ) )
    {
        qDebug( "Couldn't change mode for firstFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );


    // Position ourselves at the first file in the archive
    const int err = unzGoToFirstFile(m_unzipFile);
    if (err != UNZ_OK)
    {
        qDebug("firstFile: failed to go to first file");
        return false;
    }

    return true;
}

// Move on to the next file in the archive (if any).
bool ZipImpl::nextFile()
{
    //qDebug("nextFile");

    if ( !ensureInMode( ArchiveImpl::Decompress ) )
    {
        qDebug( "Couldn't change mode for nextFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );

    // Go to next file in the archive
    const int err = unzGoToNextFile(m_unzipFile);
    if (err != UNZ_OK)
    {
        qDebug("nextFile: failed to go to next file");
        return false;
    }

    return true;
}

bool ZipImpl::gotoFile(const QString &fileName)
{
#ifdef QT3
    qDebug("gotoFile: %s", fileName.ascii());
#else
    qDebug("gotoFile: %s", qPrintable(fileName));
#endif

    if ( !ensureInMode( ArchiveImpl::Decompress ) )
    {
        qDebug( "Couldn't change mode for gotoFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );

    if (fileName.isEmpty())
    {
        qDebug("gotoFile: fileName is empty!");
        return false;
    }

    if (!m_unzipFile)
    {
        qDebug("gotoFile: no unzip file");
        return false;
    }
#ifdef QT3
    if (unzLocateFile(m_unzipFile, fileName.ascii(), 1, 1) == UNZ_OK)
#else
    if (unzLocateFile(m_unzipFile, fileName.toAscii().constData(), 1, 1) == UNZ_OK)
#endif
    {
        // Found it so now open it so it can be used.
        int err;
        if (!m_password.isEmpty())
        {
#ifdef QT3
            err = unzOpenCurrentFilePassword(m_unzipFile, m_password.ascii());
#else
            err = unzOpenCurrentFilePassword(m_unzipFile, m_password.toAscii().constData());
#endif
        }
        else
        {
            err = unzOpenCurrentFile(m_unzipFile);
        }
        if (err != UNZ_OK)
        {
            qDebug("gotoFile: Failed to open current file");
            return false;
        }

        return true;
    }

    return false;
}


bool ZipImpl::containsFile(const QString &fileName) const
{
#ifdef QT3
    qDebug("containsFile: %s", fileName.ascii());
#else
    qDebug("containsFile: %s", qPrintable(fileName));
#endif
    
    if (fileName.isEmpty())
    {
        qDebug("containsFile: fileName is empty!");
        return false;
    }

    if (m_mode != ArchiveImpl::Decompress )
    {
        if ( !m_ioDevice || m_ioDevice->size()==0 )
        {
            return false;
        }
        bool foundFile = false;
        // store pos and mode as these will need to be restored
        // (note that we clear any Truncate flag as otherwise we'll
        //  wipe any data when we reopen)
        const qint64 pos = m_ioDevice->pos();
        const QIODevice::OpenMode iomode = m_ioDevice->openMode() & ~QIODevice::Truncate;
        ArchiveImpl::Mode restore_mode = m_mode;
        if (restore_mode == ArchiveImpl::Create )
            restore_mode = ArchiveImpl::Add;
        //qDebug() << "pos: " << pos << " Mode: " << mode;

        // need to do non-const things beind the scenes - must close
        // the zipFile so that it can be successfully opened again.
        ZipImpl* non_const_this = const_cast<ZipImpl*>(this);
        // need to close so that any currently open archive is finalised
        if ( !non_const_this->ensureInMode( ArchiveImpl::Closed ) )
        {
            qWarning( "Couldn't close zip archive" );
        }

        unzFile file = unzOpenWithIODevice( m_ioDevice );
        if ( file )
        {
#ifdef QT3
            if ( unzLocateFile(file, fileName.ascii(), 1, 0) == UNZ_OK)
#else
            if ( unzLocateFile(file, fileName.toAscii().constData(), 1, 0) == UNZ_OK)
#endif
            {
                foundFile = true;
            }
            unzClose( file );
        }

        if ( !non_const_this->ensureInMode( restore_mode ) )
        {
            qWarning( "Couldn't reopen zip archive" );
        }

        m_ioDevice->seek(pos);

        // checks
        Q_ASSERT( m_ioDevice->pos() == pos );
        Q_ASSERT( (m_ioDevice->openMode()&~QIODevice::Truncate) == iomode );
        
        return foundFile;
    }

    if ( !m_unzipFile )
    {
        qDebug("containsFile: no unzip file");
        return false;
    }
#ifdef QT3
    if (unzLocateFile(m_unzipFile, fileName.ascii(), 1, 0) == UNZ_OK)
#else
    if (unzLocateFile(m_unzipFile, fileName.toAscii().constData(), 1, 0) == UNZ_OK)
#endif
    {
        return true;
    }

    return false;
}

// based on example code at http://www.winimage.com/zLibDll/del.cpp
/*
    general approach is to use the RAW format to copy files
    across without the overhead of un/compression.

    unzGetGlobalInfo gives us any global comment information
    (we keep this as a courtesy even though Archive API doesn't 
    expose it) and then for each file that isn't to be deleted
    use unzGetCurrentFileInfo to get information about each file.
    We use this information to get what we need to then add the file
    into the 'new' archive using zipOpenNewFileInZip2 and zipWriteInFileInZip
*/
bool ZipImpl::removeFiles( const QStringList& fileNames )
{
    if ( !ensureInMode( ArchiveImpl::Remove ) )
    {
        qDebug( "Couldn't change mode for removeFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Remove );

    // open source file
    unzFile szip = unzOpenWithIODevice( m_ioDevice );
    if ( szip==NULL ) 
    { 
        m_ioDevice->close();
        return false;
    }

    // use a temporary buffer for the destination archive
    QByteArray data;
    QBuffer buffer( &data );
    zipFile dzip = zipOpenWithIODevice( &buffer, APPEND_STATUS_CREATE );
    if ( dzip==NULL ) 
    { 
        unzClose( szip );
        return false;
    }

    // get global info for the comment size
    unz_global_info glob_info;
    if ( unzGetGlobalInfo(szip, &glob_info) != UNZ_OK) 
    { 
        zipClose( dzip, NULL );
        unzClose( szip );
        return false;
    }

    // get any comment
    QByteArray glob_comment;
    if ( glob_info.size_comment > 0 )
    {
        glob_comment.resize( glob_info.size_comment );
        if ( (unsigned int)unzGetGlobalComment(szip, glob_comment.data(), glob_comment.size()) != glob_info.size_comment )
        { 
            zipClose( dzip, NULL); 
            unzClose(szip); 
            return false; 
        }
    }

    // number of files copied from dzip to szip
    int nFilesCopied = 0;
    // number of files not copied
    int nFilesDeleted = 0; 

    int rv = unzGoToFirstFile( szip );
    while (rv == UNZ_OK)
    {
        // get zipped file info
        unz_file_info unzfi;
        if ( unzGetCurrentFileInfo(szip, &unzfi, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK )
        {
            break;
        }
        QByteArray dos_fn;
        dos_fn.resize( unzfi.size_filename );
        if ( unzGetCurrentFileInfo(szip, &unzfi, dos_fn.data(), dos_fn.size(), NULL, 0, NULL, 0) != UNZ_OK )
        {
            break;
        }

        if ( fileNames.contains( QString( dos_fn ) ) )
        {
            // file is being deleted.  don't copy it over.
            nFilesDeleted++;
        }
        else
        {
            // file isn't in the delete list.  copy it over.
            QByteArray file_extra;
            file_extra.resize( unzfi.size_file_extra );
            QByteArray file_comment;
            file_comment.resize( unzfi.size_file_comment );
            if ( unzGetCurrentFileInfo( szip, &unzfi, 
                                        dos_fn.data(),       dos_fn.size(), 
                                        file_extra.data(),   file_extra.size(), 
                                        file_comment.data(), file_comment.size() ) != UNZ_OK)
            {
                break;
            }

            // open file for RAW reading
            int method;
            int level;
            const int raw(1);
            if ( unzOpenCurrentFile2( szip, &method, &level, raw ) != UNZ_OK ) 
            {  
                break;
            }

            // get the size of extrafield
            const int size_local_extra = unzGetLocalExtrafield( szip, NULL, 0 );
            if ( size_local_extra<0 ) 
            {
                break;
            }
            // allocate
            QByteArray local_extra;
            local_extra.resize( size_local_extra );
            if ( unzGetLocalExtrafield( szip, local_extra.data(), local_extra.size() ) < 0 ) 
            {
                break;
            }

            // data buffer for the compressed data.
            QByteArray buf;
            buf.resize( unzfi.compressed_size );
            // read file
            if ( unzReadCurrentFile( szip, buf.data(), buf.size() ) != (int)unzfi.compressed_size )
            {
                break;
            }

            // open destination file
            zip_fileinfo zfi;
            // zip/unzip time structures are different but have exactly the same structure
            // - hence the memcpy
            memcpy( &zfi.tmz_date, &unzfi.tmu_date, sizeof(tm_unz) );
            zfi.dosDate = unzfi.dosDate;
            zfi.internal_fa = unzfi.internal_fa;
            zfi.external_fa = unzfi.external_fa;

            if ( zipOpenNewFileInZip2( dzip, dos_fn, &zfi, 
                                       local_extra.data(), local_extra.size(), 
                                       file_extra.data(),  file_extra.size(), 
                                       file_comment.data(), method, level, raw ) != UNZ_OK ) 
            {
                break;
            }

            // write file
            if ( zipWriteInFileInZip( dzip, buf.data(), buf.size() ) != UNZ_OK ) 
            {
                break;
            }

            if ( zipCloseFileInZipRaw( dzip, unzfi.uncompressed_size, unzfi.crc ) != UNZ_OK ) 
            {
                break;
            }

            if ( unzCloseCurrentFile( szip ) == UNZ_CRCERROR ) 
            {
                break;
            }

            ++nFilesCopied;
        }

        rv = unzGoToNextFile(szip);
    }

    // close our archives - we've got what we came for
    zipClose( dzip, glob_comment.data() );
    unzClose( szip );

    // failure if we didn't delete anything or we weren't able to 
    // check all the files
    if ( (nFilesDeleted==0) || (rv!=UNZ_END_OF_LIST_OF_FILE) )
    {
        return false;
    }

    // if all files were deleted
    if ( nFilesCopied==0 )
    {
        // remove the archive entirely
        data.clear();
    }

    m_ioDevice->open( QIODevice::WriteOnly | QIODevice::Truncate );
    m_ioDevice->write( data );
    m_ioDevice->close();

    return true;
}


}  // of bugless namespace

