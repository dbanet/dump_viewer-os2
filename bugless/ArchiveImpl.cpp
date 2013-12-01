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
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>

#include "util/dirUtils.h"
#include "IOCompressorImpl.h"
#include "ArchiveDevice.h"

#include "ArchiveImpl.h"

namespace bugless
{

ArchiveImpl::ArchiveImpl()
    : m_ioDevice( NULL )
    , m_mode(ArchiveImpl::Closed)
    , m_open( false )
    , m_dev(NULL)
    , m_password(QString::null)
{
    setBufferSize( 10240 );
}

ArchiveImpl::~ArchiveImpl()
{
    close();
}

void ArchiveImpl::setBufferSize( int bufSize )
{
    if ( m_mode != ArchiveImpl::Closed )
    {
        qWarning( "can't set buffer size on an open archive" );
        return;
    }

    m_buf.resize(bufSize);
}

void ArchiveImpl::setPassword( const QString& password )
{
    m_password = password;
}

bool ArchiveImpl::ensureInMode( const ArchiveImpl::Mode mode )
{
    if ( mode == m_mode )
    {
        return true;
    }

    QString modeAsString( "UNKNOWN" );

    switch( mode )
    {
    case ArchiveImpl::Closed:
        modeAsString = "CLOSED";
        break;
    case ArchiveImpl::Create:
        modeAsString =  "CREATE";
        break;
    case ArchiveImpl::Add:
        modeAsString =  "ADD";
        break;
    case ArchiveImpl::Remove:
        modeAsString =  "REMOVE";
        break;
    case ArchiveImpl::Decompress:
        modeAsString =  "DECOMPRESSS";
        break;
    default:
        break;
    };

    qDebug( "Change mode to %s,", qPrintable( modeAsString ) );

    if ( mode != ArchiveImpl::Closed )
    {
        if ( !m_open )
        {
            qDebug( "Not open. opening..." );
            open();
        }
    }

    if ( !changeModeTo( mode ) )
    {
        return false;
    }

    m_mode = mode;

    return true;
}

bool ArchiveImpl::changeModeTo( const ArchiveImpl::Mode )
{
    return true;
}

bool ArchiveImpl::open()
{
    //qDebug("ArchiveImpl::open");

    // Check for re-open
    if ( m_open )
    {
        // The archive is already open
        return true;
    }
    m_open = true;

    // Need to have something to open!
    if ( !m_ioDevice )
    {
        qDebug("open: need to assign archive io device name before opening!");
        return false;
    }

    return true;
}

void ArchiveImpl::close()
{
    if ( !m_open )
    {
        // We are already closed
        return;
    }
    m_open = false;

    // Close the current device if it exists
    if ( m_dev.get() )
    {
        m_dev->close();
        m_dev.reset();
    }

    ensureInMode( ArchiveImpl::Closed );

    // last of all, close our IODevice
    // (but hold onto it - we may be opened again)
    if ( m_ioDevice )
    {
        if ( m_compressor.get() )
        {
            m_compressor->flushBuffer();
        }
        m_ioDevice->close();
    }
}

bool ArchiveImpl::setIODevice( QIODevice* device )
{
    if ( m_open )
    {
        // We're already open!
        qDebug("setIODevice: close archive first before setting IO device!");
        return false;
    }

    // internally access to the main IO device may be via a 2nd compression
    // stage.  the compressor exposes itself as a QIODevice so it is all
    // transparent as far as we are concerned.
    if ( device )
    {
        m_compressor.reset( new IOCompressorImpl(device) );
        m_ioDevice = m_compressor->buffer();
    }
    else
    {
        m_ioDevice = NULL;
    }

    return m_ioDevice!=NULL;
}

bool ArchiveImpl::addFile( const QString &fileName, QIODevice* iodev, const QDateTime& lastModified )
{
    if ( !iodev )
    {
        qDebug( "addFile - cannot add NULL file" );
        return false;
    }

    if ( !ensureInMode( ArchiveImpl::Add ) )
    {
        qDebug( "Couldn't change mode for addFile" );
        return false;
    }

    // check to see if there is already a file of that name
    if ( containsFile( fileName ) )
    {
        qWarning( "File already exists in the archive!");
        return false;
    }

    m_dev->setAddFileName( fileName );
    m_dev->setAddFileTime( lastModified.isValid() ? lastModified : QDateTime::currentDateTime() );
    m_dev->setAddFileIODevice( iodev );

    // Now create a new file in the archive
    if ( !m_dev->open(m_mode) )
    {
        qDebug("addFile: failed open device");
        m_dev->setAddFileName( QString() );
        m_dev->setAddFileTime( QDateTime() );
        m_dev->setAddFileIODevice( NULL );

        return false;
    }

    // device is set up.  now open the file to be added
    // and archive it
#ifdef QT3
    if (!iodev->open(IO_ReadOnly))
#else
    if (!iodev->open(QIODevice::ReadOnly))
#endif
    {
        qDebug("addFile: failed to open file!");
        m_dev->close();
        m_dev->setAddFileName( QString() );
        m_dev->setAddFileTime( QDateTime() );
        m_dev->setAddFileIODevice( NULL );

        return false;
    }

    int nRead = 0;
#ifdef QT3
    unsigned int srcSize = iodev->size();
    unsigned int toBeRead = srcSize > m_buf.size() ? m_buf.size() : srcSize;
    unsigned int totalRead = 0;
#else
    int srcSize = iodev->size();
    int toBeRead = srcSize > m_buf.size() ? m_buf.size() : srcSize;
    int totalRead = 0;
#endif

    do
    {
        // Read in from file
#ifdef QT3
        nRead = iodev->readBlock(m_buf.data(), toBeRead);
#else
        nRead = iodev->read(m_buf.data(), toBeRead);
#endif
        totalRead += nRead;
        toBeRead = (srcSize - totalRead) > m_buf.size() ? m_buf.size() : (srcSize - totalRead);

        // Write out to the archive
#ifdef QT3
        if (m_dev->writeBlock(m_buf.data(), nRead) != nRead)
#else
        if (m_dev->writeData(m_buf.data(), nRead) != nRead)
#endif
        {
            // Error!
            // Archive may now be in an inconsistent state.
            qDebug("addFile: failed writing to archive.");
            break;
        }
    } while ((nRead > 0) && (totalRead < srcSize));

    // Close the input file
    iodev->close();

    // Close the new file in the archive
    m_dev->close();

    m_dev->setAddFileName( QString() );
    m_dev->setAddFileTime( QDateTime() );
    m_dev->setAddFileIODevice( NULL );

    return true;
}

bool ArchiveImpl::removeFile( const QString &fileName )
{
    return removeFiles( QStringList() << fileName );
}

bool ArchiveImpl::removeAll()
{
    if ( !ensureInMode( ArchiveImpl::Closed ) )
    {
        qDebug( "Couldn't change mode for extractAll" );
        return false;
    }

    if ( !m_ioDevice )
    {
        return false;
    }

    if ( !m_ioDevice->open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    { 
        return false;
    }


    m_ioDevice->close();
    return true;
}

bool ArchiveImpl::extractAll( const QDir& dir )
{
    //qDebug("extractAll");

    if ( !ensureInMode( ArchiveImpl::Decompress ) )
    {
        qDebug( "Couldn't change mode for extractAll" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );

    ArchiveIterator I = begin();
    ArchiveIterator E = end();

    if ( I == E )
    {
        // Error
        qDebug("extractAll: failed to go to first file");
        return false;
    }

    for ( ; I!=E; ++I )
    {
        // Decompress the file
#ifdef QT3
    qDebug("extractFile: file %s", I->name().ascii());
#else
    qDebug("extractFile: file %s", qPrintable(I->fileName()));
#endif
        if ( !I->extractFile( dir ) )
        {
#ifdef QT3
    qWarning("extractFile: failed %s", I->name().ascii());
#else
    qWarning("extractFile: failed %s", qPrintable(I->fileName()));
#endif
            return false;
        }
    }

    return true;
}

bool ArchiveImpl::extractFile( const QString& fName, const QDir& dir, const QString& newName, QString* outFileName )
{
    //qDebug("extractFile");

    if ( !ensureInMode( ArchiveImpl::Decompress ) )
    {
        qDebug( "Couldn't change mode for extractFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );

    // check file exists
    if ( !gotoFile( fName ) )
    {
        qDebug("extractFile: file is not a member of this archive");
        return false;
    }

    QDir outDir = dir;;
    QString fileName = newName.isEmpty() ? fName : newName;

    if (fileName.isEmpty())
    {
        if ( outFileName )
        {
            *outFileName = QString();
        }
        qDebug("extractFile: failed to get file name for output file.");
        return false;
    }

    // Deal with the output directory and file name
    if ( dir.path().isEmpty() )
    {
#ifdef QT3
        outDir = QDir::currentDir();
#else
        outDir = QDir::current();
#endif
    }

    fileName = outDir.filePath( fileName );

    // Open the output file
    QFile outFile(fileName);
    QFileInfo fi(fileName);

    // set up the folder structure needed to ensure the outFilepath
    // is valid
#ifdef QT3
    createPath(fi.dirPath());
#else
    createPath(fi.path());
#endif

    if ( outFileName )
    {
        *outFileName = fileName;
    }

    return ArchiveImpl::extractFile( fName, &outFile );
}

bool ArchiveImpl::extractFile( const QString& fName, QIODevice* outDev )
{
    if ( !ensureInMode( ArchiveImpl::Decompress ) )
    {
        qDebug( "Couldn't change mode for extractFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );

    // Open the device in decompress mode
    if ( !m_dev->open(ArchiveImpl::Decompress) )
    {
        qDebug("extractFile: failed to open current file");
        return false;
    }

    if ( fName.isEmpty() )
    {
        qWarning( "ExtractFile - no file name specified" );
        return false;
    }

    if ( !gotoFile(fName) )
    {
#ifdef QT3
        qDebug("extractFile: failed to find file %s", fName.ascii());
#else
        qDebug("extract: failed to find file %s", qPrintable( fName ) );
#endif
        m_dev->close();
        return false;
    }

#ifdef QT3
    if (!outDev->open(IO_WriteOnly))
#else
    if ( !outDev->open(QIODevice::WriteOnly) )
#endif
    {
        qDebug("extractFile: failed to open output device");
        m_dev->close();
        return false;
    }

    while (true)
    {
        // Expand the current file in the archive
#ifdef QT3
        int nRead = m_dev->readBlock(m_buf.data(), m_buf.size());
#else
        int nRead = m_dev->readData(m_buf.data(), m_buf.size());
#endif
        //qDebug("extractFile: read %d %s", nRead, m_buf.data());

        if (nRead == 0)
        {
            // We're done
            break;
        }

        if (nRead < 0)
        {
            // Error
            qDebug("extractFile: device read failed! %d", nRead);
            m_dev->close();
            return false;
        }

        // Write the data to the output file.
#ifdef QT3
        if ( outDev->writeBlock(m_buf.data(), nRead) == -1)
#else
        if ( outDev->write(m_buf.data(), nRead) == -1)
#endif
        {
            qDebug("extractFile: failed to write to device!");
            m_dev->close();
            return false;
        }
    }

    // Close the output file
    outDev->close();

    // Close the device
    m_dev->close();

    return true;
}


ArchiveDevice* ArchiveImpl::device( const QString &fName )
{
    if ( fName.isEmpty() )
    {
        qWarning( "device: file name not specified" );
        return NULL;
    }

    if ( !m_open )
    {
        qWarning( "device: archive not opened" );
        return NULL;
    }

    if ( containsFile( fName ) )
    {
        // if file name is specified try and make it current.  this sets the 
        // archive to the correct location
        if ( !gotoFile(fName) )
        {
    #ifdef QT3
            qDebug("device: failed to goto file %s", fName.ascii());
    #else
            qDebug("device: failed to goto file %s", qPrintable( fName ) );
    #endif
        }
    }
    else
    {
        // file not already in archive.  
        if ( m_dev.get() )
        {
            m_dev->setAddFileName( fName );
        }
    }
    
    return m_dev.get();
}

QStringList ArchiveImpl::files() const
{
    //qDebug("listFiles");

    // \todo get rid of this - make firstFile non-const or find alternative
    ArchiveImpl* non_const_this = const_cast<ArchiveImpl*>(this);

    if ( !non_const_this->ensureInMode( ArchiveImpl::Decompress ) )
    {
        qDebug( "Couldn't change mode for extractFile" );
        return QStringList();
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );



    if ( !non_const_this->firstFile() )
    {
        // The archive is empty
        return QStringList();
    }

    QStringList names;
    while(true)
    {
        // Get the file name
        names << m_dev->fileName();

        // Move on to the next file
        if ( !non_const_this->nextFile() )
        {
            // We're out of files
            break;
        }
    }

    return names;
}


bool ArchiveImpl::containsFile(const QString &fileName) const
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

    // store previous mode so that we can restore it
    const ArchiveImpl::Mode restore_mode = m_mode;

    // \todo et rid of this - make firstFile non-const or finnd alternative
    ArchiveImpl* non_const_this = const_cast<ArchiveImpl*>(this);

    if ( !non_const_this->firstFile() )
    {
        // The archive is empty
        non_const_this->ensureInMode( restore_mode );
        return false;
    }

    while(true)
    {
        // Get the file name
        if( m_dev->fileName() == fileName )
        {
            non_const_this->ensureInMode( restore_mode );
            return true;
        }

        // Move on to the next file
        if ( !non_const_this->nextFile() )
        {
            // We're out of files
            break;
        }
    }

    non_const_this->ensureInMode( restore_mode );
    return false;
}

bool ArchiveImpl::gotoFile(const QString &fileName)
{
#ifdef QT3
    qDebug("gotoFile: %s", fileName.ascii());
#else
    qDebug("gotoFile: %s", qPrintable(fileName));
#endif

    if ( fileName.isEmpty() )
    {
        qDebug("gotoFile: fileName is empty!");
        return false;
    }
    
    if ( !firstFile() )
    {
        // The archive is empty
        return false;
    }

    while(true)
    {
        // Get the file name
        if ( m_dev->fileName() == fileName )
        {
            return true;
        }

        // Move on to the next file
        if ( !nextFile() )
        {
            // We're out of files
            break;
        }
    }

    return false;}


ArchiveIterator ArchiveImpl::begin()
{
    ArchiveIterator I(this);

    if ( firstFile() )
    {
        I.setName( m_dev->fileName() );
    }

    return I;
}

ArchiveIterator ArchiveImpl::end()
{
    ArchiveIterator I(NULL);

    // A null string marks the end of the archive.
    I.setName(QString(""));

    return I;
}

ArchiveIterator ArchiveImpl::find( const QString& fName )
{ 
    ArchiveIterator I = begin();
    ArchiveIterator E = end();
    for ( ; I != E; ++I)
    {
        if (I.fileName() == fName)
        {
            break;
        }
    }

    return I;
}


}  // of namsepace bugless
