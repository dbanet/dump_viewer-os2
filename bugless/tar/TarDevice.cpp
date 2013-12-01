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

/*! \file TarDevice.cpp
\brief Source for TarDevice class.
*/

#include <zlib.h>

#ifdef QT3
# include <qstring.h>
# include <qstringlist.h>
# include <qiodevice.h>
# include <qfile.h>
# include <qfileinfo.h>
# include <qdatetime.h>
#else
# include <QString>
# include <QStringList>
# include <QIODevice>
# include <QFile>
# include <QFileInfo>
# include <QDateTime>

# include <QDebug>
#endif

#include "TarImpl.h"
#include "TarDevice.h"
#include "Archive.h"


namespace bugless
{


TarDevice::TarDevice( TarImpl* impl )
    : ArchiveDevice( impl )
    , m_tarImpl(impl)
    , m_nTotalRead( 0 )
{
}

bool TarDevice::open( ArchiveImpl::Mode mode )
{
    qDebug("TarDevice::open: ArchiveImpl::Mode %d", mode);

    if ( m_mode == mode )
    {
        // We are already in in the correct mode
        return true;
    }

    if ( !m_archiveImpl->ensureInMode( mode ) )
    {
        qDebug( "TarDevice::open failed to change mode" );
        return false;
    }

    switch (mode)
    {
    case ArchiveImpl::Closed:
    {
        qDebug("TarDevice::open: invalid mode!");
        return false;
    }
    case ArchiveImpl::Create:
    case ArchiveImpl::Add:
    {
        if ( !m_tarImpl )
        {
            // Need to be open
            qDebug("TarDevice::open: open archive for Add or Create first!");
            return false;
        }
        
        if ( m_addFileName.isEmpty() )
        {
            // Need to set the file name first
            qDebug("TarDevice::open: set file name first!");
            return false;
        }
        
        tar::TarHeader info;
        info.setName( m_addFileName );
        
        if ( tar::haveEOFMarker( m_tarImpl->ioDevice() ) )
        {
            info.setPos( m_tarImpl->ioDevice()->size() - 2*tar::BLOCK_SIZE );
        }
        else
        {
            info.setPos( m_tarImpl->ioDevice()->size() );
        }
        info.setTime( m_addFileTime.isValid() ? m_addFileTime : QDateTime::currentDateTime() );
        
        if ( !writeHeader( m_tarImpl->ioDevice(), info ) )
        {
            qDebug("TarDevice::open: failed open new file in archive" );
            return false;
        }
        setHeaderInfo( info );
        m_nTotalWritten = 0;
        break;
    }
    case ArchiveImpl::Decompress:
    {
        if ( m_tarImpl == NULL )
        {
            qDebug("TarDevice::open: open archive for Decompress first!");
            return false;
        }
        
        // our TarHeader should have been set up via parent archive's device()/gotoFile()
        if ( m_headerInfo.name().isEmpty() )
        {
            qDebug("TarDevice::open: set file to Decompress first!");
            return false;
        }
        
        m_nTotalRead = 0;
        break;
    }
    case ArchiveImpl::Remove:
    default:
        break;
    }

    m_mode = (ArchiveImpl::Mode)mode;

    return true;
}

void TarDevice::close()
{
    switch (m_mode)
    {
    case ArchiveImpl::Closed:
        return;
        
    case ArchiveImpl::Create:
    case ArchiveImpl::Add:
    {
        m_headerInfo.setSize( m_nTotalWritten );
        // update the header 
        // \todo lightweight update for size/checksum only
        if ( !tar::writeHeader( m_tarImpl->ioDevice(), m_headerInfo ) )
        {
            qDebug("TarDevice::close: failed to finalise header" );
        }
        // pad out the file to required block size
        if ( !tar::writeBlocks( m_tarImpl->ioDevice(), m_headerInfo ) )
        {
            qDebug("TarDevice::close: failed to finalise data blocks" );
        }
        break;
    }
    case ArchiveImpl::Decompress:
    case ArchiveImpl::Remove:
    default:
        break;
    }

    m_mode = ArchiveImpl::Closed;
}

void TarDevice::setHeaderInfo( tar::TarHeader& info )
{
    m_headerInfo = info;
}

QString TarDevice::fileName() const
{
    return m_headerInfo.name();
}

#ifdef QT3
Q_ULONG TarDevice::size() const
#else
qint64 TarDevice::size() const
#endif
{    
    if ( m_mode == ArchiveImpl::Decompress )
    {
        // \todo invalid header?
        return m_headerInfo.size();
        // if ( !m_headerInfo.valid )
        {
#ifdef QT3
            return 0;
#else
            return -1;
#endif
        }

        //return m_headerInfo.size();
    }

    // If writing return 0.
    return 0;
}


#ifdef QT3
Q_LONG TarDevice::readBlock(char *data, Q_ULONG maxlen)
#else
qint64 TarDevice::readData(char *data, qint64 maxlen)
#endif
{
    const qint64 remaining = m_headerInfo.size() - m_nTotalRead;
    if ( remaining <= 0 )
    {
        // nothing left to read - return 0 to indicate completion
        return 0;
    }

    maxlen = qMin( maxlen, remaining );
    // make sure we're reading from the correct place
    const int pos = m_headerInfo.pos() + tar::HEADER_SIZE + m_nTotalRead;
    m_tarImpl->ioDevice()->seek( pos );
    const int nRead = m_tarImpl->ioDevice()->read( data, maxlen );
#ifdef QT3
    qDebug("readBlock: read %d %s", nRead, data);
#else
    qDebug() << "readData: read:" << nRead << QByteArray( data, nRead );
#endif

    if (nRead < 0)
    {
        // Error
        qDebug("readBlock: file read failed! %d", nRead);
        return -1;
    }

    m_nTotalRead += nRead;

    return nRead;
}


#ifdef QT3
Q_LONG TarDevice::writeBlock(const char *data, Q_ULONG len)
#else
qint64 TarDevice::writeData(const char *data, qint64 len)
#endif
{
#ifdef QT3
    qDebug("writeBlock: 0x%x %d", (uint)data, (int)len);
#else
    qDebug() << "writeData:" << data << len;
#endif

    // make sure we're writing to the correct place
    const int pos = m_headerInfo.pos() + tar::HEADER_SIZE + m_nTotalWritten;
    m_tarImpl->ioDevice()->seek( pos );
    const int nWritten = m_tarImpl->ioDevice()->write( data, len );

    if ( nWritten != len )
    {
        // Error
        qDebug("writeBlock: failed!");
        return -1;
    }

    m_nTotalWritten += len;

    return len;
}

#ifndef QT3
qint64 TarDevice::pos() const
{
    if ( m_mode == ArchiveImpl::Decompress )
    {
        return m_nTotalRead;
    }
    else
    {
        return m_nTotalWritten;
    }
}
#endif


} // of namespace bugless
