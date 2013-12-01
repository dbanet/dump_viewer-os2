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
#include <qpointer.h>


#include "util/fileUtils.h"
#include "util/QIODeviceCloser.h"
#include "ArchiveFactory.h"
#include "ArchiveIterator.h"
#include "TarDevice.h"

#include "TarImpl.h"


namespace bugless
{

class TarArchiveCreator : public ArchiveCreatorInterface
{
public:
    TarArchiveCreator()
    {
        ArchiveFactory::instance()->registerArchiveType( Archive::TAR, this );
    }

	virtual Archive::Type type() const
    {
        return Archive::TAR;
    }

	virtual ArchiveImpl* create() const
    {
        return new TarImpl;
    }
	
    virtual bool couldBe( QIODevice* dev ) const
    {
        QIODeviceCloser io( dev, QIODevice::ReadOnly );
        // tarballs start with a 512 byte header that includes a 
        // checcksum.
        return tar::validChecksum( dev, 0 );
    }

	virtual QString extension() const
    {
        return "tar";
    }
};

// global creator object
TarArchiveCreator g_tarCreator;


TarImpl::TarImpl()
    : ArchiveImpl()
{
    m_dev.reset( new TarDevice(this) );
}

TarImpl::~TarImpl()
{
}

bool TarImpl::changeModeTo( const ArchiveImpl::Mode mode )
{
    if ( mode == ArchiveImpl::Closed )
    {
        if ( m_mode == ArchiveImpl::Decompress )
        {
            m_ioDevice->close();
            // were open for decompress.  eof marker should be unchanged
            return true;
        }

        if ( !tar::haveEOFMarker( m_ioDevice ) )
        {
            tar::addEOFMarker( m_ioDevice );
        }

        // all done
        m_ioDevice->close();
        return true;
    }


    // Allocate the TarDevice
    if ( !m_dev.get() )
    {
        m_dev.reset( new TarDevice(this) );
    }

    // Open the archive
    switch (mode)
    {
        case ArchiveImpl::Closed:
        {   // Just to keep the compiler quiet
            break;
	    }
        case ArchiveImpl::Remove:
	    {   
            if ( !m_ioDevice->open( QIODevice::ReadWrite ) )
            {
                qDebug( "Failed to open the Archive for remove" );
                return false;
            }
            break;
	    }
        case ArchiveImpl::Create:
        {
            // creating new.  clear any existing contents
            if ( !m_ioDevice->open( QIODevice::ReadWrite | QIODevice::Truncate ) )
            {
                qDebug( "Failed to open the Archive for create" );
                return false;
            }
            break;
        }
        case ArchiveImpl::Add:
        {
            if ( m_ioDevice->isOpen() )
            {
                if ( m_ioDevice->isWritable() )
                {
                    // already writable
                    return true;
                }
                // close to open again
                m_ioDevice->close();

            }
            if ( !m_ioDevice->open( QIODevice::ReadWrite ) )
            {
                qDebug( "Failed to open the Archive for add" );
                return false;
            }
            
            // need to remove any trailing optional \0 block.
            // these will be restored when the archive is closed again
            /*
            // don't need to remove them - will get overwritten by
            // any neewly added file
            if ( !tar::trimFinalBlocks( m_ioDevice ) )
            {
                qDebug( "tar - failed to trim final blocks" );
                return false;
            }*/

            if ( !tar::validChecksum( m_ioDevice, 0 ) )
            {
                m_ioDevice->close();

                qDebug( "Archive isn't a valid Tar archive - can't open for Add - opening for Create instead" );
                if ( !m_ioDevice->open( QIODevice::ReadWrite ) )
                {
                    qDebug( "Failed to open the Archive for create" );
                    return false;
                }
            }
            break;
        }
        case ArchiveImpl::Decompress:
        {
            if ( m_ioDevice->isOpen() )
            {
                // already open
                return true;
            }

            if ( !m_ioDevice->open( QIODevice::ReadOnly ) )
            {
                // can fail if the file doesn't exist - not been opened for add
                // yet - still checking if archive contains file before opening it
                qDebug( "Failed to open the Archive for decompress" );
                return false;
            }

            if ( !tar::validChecksum( m_ioDevice, 0 ) )
            {
                qDebug( "Archive isn't a valid Tar archive - can't open for Decompress" );
                m_ioDevice->close();
                return false;
            }

            break;
        }
    }

    return true;
}


bool TarImpl::extractFile( const QString& fName, const QDir& dir, const QString& newName, QString* )
{
    QString outFileName;
    if ( !ArchiveImpl::extractFile( fName, dir, newName, &outFileName ) )
    {
        return false;
    }

    // set the file modifiaction time based on header info
    setFileTime( outFileName, tarDevice()->headerInfo().time() );

    return true;
}

bool TarImpl::firstFile()
{
    //qDebug("firstFile");

    if ( !ensureInMode( ArchiveImpl::Decompress ) )
    {
        // this can happen when, for example, the archive is new/empty - nothing to open
        qDebug( "Couldn't change mode for firstFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );

    // Position ourselves at the first file in the archive
    tar::TarHeader info;
    info.setPos( 0 );
    if ( !tar::readHeader( m_ioDevice, info ) )
    {
        return false;
    }

    // make current
    tarDevice()->setHeaderInfo( info );

    return true;
}

bool TarImpl::nextFile()
{
    //qDebug("nextFile");

    if ( !ensureInMode( ArchiveImpl::Decompress ) )
    {
        qDebug( "Couldn't change mode for nextFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Decompress );

    // next file's start position will be current file's
    // start position + Header Size + file size.
    const tar::TarHeader& curInfo = tarDevice()->headerInfo();
    if ( curInfo.size() == 0 )
    { 
        return false;
    }

    tar::TarHeader info;
    info.setPos( curInfo.nextHeaderPos() );

    // check that there is a valid header at the next block
    if ( !tar::validChecksum( m_ioDevice, info.pos() ) )
    {
        qDebug( "TarImpl::nexFile - next file header is not valid" );
        return false;
    }

    // valid header.  read it.
    if ( !tar::readHeader( m_ioDevice, info ) )
    {
        return false;
    }

    // make current
    tarDevice()->setHeaderInfo( info );

    return true;
}

bool TarImpl::removeFiles( const QStringList& _files )
{
    if ( !ensureInMode( ArchiveImpl::Remove ) )
    {
        qDebug( "Couldn't change mode for removeFile" );
        return false;
    }
    Q_ASSERT( m_mode == ArchiveImpl::Remove );

    // need to cut out the sections which apply to
    // files which need are to be removed.  no
    // api on QIODevice to do this, so read into a 
    // QByteArray, modify it, and write it back.
    QIODeviceCloser ioDevice( m_ioDevice, QIODevice::ReadOnly, m_ioDevice->openMode() );

    // first gather the positions and sizes of any files to be removed
    std::vector< std::pair<int,int> > removals;

    if ( !firstFile() )
    {
        // The archive is empty
        return false;
    }

    unsigned int toremove(0);
    QStringList files( _files );
    while( true )
    {
        const int index = files.indexOf( m_dev->fileName() );
        if ( index != -1 )
        {
            const tar::TarHeader& curInfo = tarDevice()->headerInfo();
            // start position
            const int pos = curInfo.pos();
            // size of entry (header+data+block padding)
            const int size = curInfo.totalSize();
            removals.push_back( std::pair< int, int >(pos, size ) );
            toremove += size;

            files.removeAt( index );
        }

        // stop if there's nothing left to find
        if ( files.isEmpty() )
        {
            break;
        }

        // Move on to the next file
        if ( !nextFile() )
        {
            // We're out of files
            break;
        }
    }

    if ( removals.empty() )
    {
        qWarning( "No matching files were found to be removed" );
        return false;
    }
    
    ioDevice->reset();
    QByteArray data = ioDevice->readAll();
    ioDevice->close();

    // \todo better to copy the files we want to keep into a new QByteArray or
    // cut out the files we don't want? could compare size of archive with amount to be
    // removed

    // track how much has been removed as this modifies the start pos for 
    // each removal block
    unsigned int removed(0);
    std::vector< std::pair<int,int> >:: const_iterator I = removals.begin();
    std::vector< std::pair<int,int> >:: const_iterator E = removals.end();
    for ( ; I!=E; ++I )
    {
        data.remove( I->first - removed, I->second );
        removed += I->second;
    }
    Q_ASSERT( toremove == removed );

    ioDevice->open( QIODevice::WriteOnly | QIODevice::Truncate );
    ioDevice->write( data );

    return true;
}


}  // of bugless namespace

