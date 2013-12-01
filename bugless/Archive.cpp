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

/*! \file Archive.cpp
\brief Source for Archive class.
*/

#include <qbuffer.h>

#include "ArchiveDevice.h"
#include "ArchiveIterator.h"
#include "ArchiveFactory.h"
#include "ArchiveImpl.h"

#include "Archive.h"

namespace bugless
{

// given a file name try and guess the archive type
// \todo update to handle archive.compressor format e.g. tar.gz
Archive::Type fromName( const QString& name )
{
    QList<Archive::Type> types = ArchiveFactory::instance()->types();
    foreach( Archive::Type type, types )
    {
        const QString ext = ArchiveFactory::instance()->extension( type );
        if ( name.endsWith( ext, Qt::CaseInsensitive ) )
        {
            return type;
        }
    }

    return Archive::NONE;
}

Archive::Type fromPeek( QIODevice* device )
{
    if ( device )
    {
        QList<Archive::Type> types = ArchiveFactory::instance()->types();
        foreach( Archive::Type type, types )
        {
            if ( ArchiveFactory::instance()->couldBe( type, device ) )
            {
                return type;
            }
        }
    }

    return Archive::NONE;
}

//! The dark side of Archive
/*!
    Wraps up the actual type specific Archive implementation as well as some
    state that allows us to have a flexible api.

    We store archive name, password, iodevice etc so that we can be 
    initialised before knowing exactly what type we are - i.e. before we
    have a valid ArchiveImpl.
*/
class ArchivePrivate
{
public:
    ArchivePrivate( Archive* parent );

    Archive* m_parent;

    //! Type specific implementation to do the heavy lifting.
    std::auto_ptr<ArchiveImpl> m_impl;

    QString m_archiveName;
    QString m_password;
    //! If all we had was an archive name then we are the creator/owner of the
    //! QFile which is used as our QIODevice
    QFile* m_file;
    //! hold onto ioDevices whose type hasn't been resolved yet
    QIODevice* m_ioDevice;
    Archive::Type m_type;

    // helper ensures archive has been opened
    bool ensureOpen();
};

ArchivePrivate::ArchivePrivate( Archive* parent )
    : m_parent( parent ),
      m_impl( NULL ),
      m_file( NULL ),
      m_ioDevice( NULL ),
      m_type( Archive::NONE )
{}

bool ArchivePrivate::ensureOpen()
{
    if ( !m_impl.get() || !m_impl->isOpen() )
    {
        return m_parent->open();
    }

    return m_impl.get() && m_impl->isOpen();
}


Archive::Archive(const QString& fileName, const Type& type )
    : QObject( NULL ),
      m_impl( new ArchivePrivate( this ) )
{
    setArchiveName( fileName );
    setType( type );
}

Archive::Archive(QIODevice* device, const Type& type )
    : QObject( NULL ),
      m_impl( new ArchivePrivate( this ) )
{
    m_impl->m_ioDevice = device;
    setType( type );
}

Archive::Archive()
    : QObject( NULL ),
      m_impl( new ArchivePrivate( this ) )
{}

Archive::~Archive()
{
    close();
    delete m_impl;
}

bool Archive::valid() const
{
    return m_impl->m_impl.get();
}

bool Archive::setType( Archive::Type type )
{
    if ( valid() )
    {
        qWarning( "Cannot change Archive type" );
        return false;
    }

    m_impl->m_type = type;

    m_impl->m_impl.reset( ArchiveFactory::instance()->create( m_impl->m_type ) );

    if ( m_impl->m_impl.get() == NULL )
    {
        qWarning( "Archive Type not supported: %i", type );
        return false;
    }

    return true;
}

Archive::Type Archive::type() const
{
    return m_impl->m_type;
}

void Archive::setBufferSize( int bufSize )
{
    // don't need to be open to set buffer size, just valid
    if ( !valid() )
    {
        return;
    }

    return m_impl->m_impl->setBufferSize( bufSize );
}

bool Archive::setArchiveName( const QString& archiveName )
{
    m_impl->m_archiveName = archiveName;

    return true;
}

void Archive::setPassword( const QString& password )
{
    m_impl->m_password = password;
}

bool Archive::open()
{
    if ( valid() && m_impl->m_impl->isOpen() )
    {
        return true;
    }

    if ( !m_impl->m_archiveName.isEmpty() )
    {
        m_impl->m_file = new QFile( m_impl->m_archiveName );
        m_impl->m_ioDevice = m_impl->m_file;
    }

    if ( !valid() )
    {
        // try and determine type
        Type type(NONE);
        
        {
            // peek takes priority
            type = fromPeek( m_impl->m_ioDevice );
            if ( type == NONE )
            {
                type = fromName( m_impl->m_archiveName );
            }
        }

        setType( type );
    }

    if ( !valid() )
    {
        // still not valid.  can't do anymore
        return false;
    }

    if (m_impl->m_ioDevice )
    {
        m_impl->m_impl->setIODevice( m_impl->m_ioDevice );
    }

    m_impl->m_impl->setPassword( m_impl->m_password );

    return m_impl->m_impl->open();
}

void Archive::close()
{
    if ( m_impl->m_impl.get() )
    {
        m_impl->m_impl->close();
        m_impl->m_impl->setIODevice( NULL );
    }

    if ( m_impl->m_file )
    {
        // file should have been closed already
        if ( m_impl->m_file->isOpen() )
        {
            qWarning( "Archive::close - m_file is still open!" );
        }
        delete m_impl->m_file;
        m_impl->m_file = NULL;
    }

    m_impl->m_ioDevice = NULL;

}

bool Archive::addFile(const QString &fileName)
{
#ifdef QT3
    qDebug("addFile: file %s", fileName.ascii());
#else
    qDebug("addFile: file %s", qPrintable(fileName));
#endif

    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    // and its modification time
    QDateTime lastModified;
    if ( QFileInfo(fileName).exists() )
    {
        lastModified = QFileInfo(fileName).lastModified();
    }
    else
    {
        lastModified = QDateTime::currentDateTime();
    }

    // Open input file
    QFile file(fileName);
    return m_impl->m_impl->addFile( fileName, &file, lastModified );
}

bool Archive::addFiles( const QStringList &fileNames )
{
    bool result(true);
    foreach( const QString& file, fileNames )
    {
        result &= addFile( file );
    }

    return result;
}

bool Archive::addFile( QFile& file )
{
#ifdef QT3
    qDebug("addFile: file %s", file.fileName().ascii());
#else
    qDebug("addFile: file %s", qPrintable(file.fileName()));
#endif

    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    // and its modification time
    QDateTime lastModified;
    if ( QFileInfo(file).exists() )
    {
        lastModified = QFileInfo(file).lastModified();
    }
    else
    {
        lastModified = QDateTime::currentDateTime();
    }

    // Open input file
    return m_impl->m_impl->addFile( file.fileName(), &file, lastModified );
}


bool Archive::addFile( const QString &fileName, QIODevice* iodev )
{
    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    return m_impl->m_impl->addFile( fileName, iodev );
}

bool Archive::addFile( const QString& fileName, QByteArray& data )
{
    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    // wrap the data in a buffer for i/o
    QBuffer buffer( &data );
    return m_impl->m_impl->addFile( fileName, &buffer );
}

bool Archive::removeFile( const QString &fileName )
{
    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    return m_impl->m_impl->removeFile( fileName );
}

bool Archive::removeFiles( const QStringList &fileNames )
{
    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    return m_impl->m_impl->removeFiles( fileNames );
}

bool Archive::removeAll()
{
    if ( !valid() )
    {
        return false;
    }

    return m_impl->m_impl->removeAll();
}

bool Archive::extractAll( const QDir& dir )
{
    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    return m_impl->m_impl->extractAll( dir );
}

bool Archive::extractFile( const QString& fName, const QDir& dir, const QString& newName )
{
    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    return m_impl->m_impl->extractFile( fName, dir, newName );
}

bool Archive::extractFile( const QString& fileName, QByteArray& data )
{
    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    // wrap the data in a buffer for i/o
    QBuffer buffer( &data );
    return m_impl->m_impl->extractFile( fileName, &buffer );
}

bool Archive::extractFile( const QString& fileName, QIODevice* dev )
{
    if ( !m_impl->ensureOpen() )
    {
        return false;
    }

    return m_impl->m_impl->extractFile( fileName, dev );
}

QIODevice* Archive::device( const QString &fName )
{
    if ( !m_impl->ensureOpen() )
    {
        return NULL;
    }

    return m_impl->m_impl->device(fName);
}

QStringList Archive::files()
{
    if ( !m_impl->ensureOpen() )
    {
        return QStringList();
    }

    return m_impl->m_impl->files();
}

ArchiveIterator Archive::begin()
{
    if ( !m_impl->ensureOpen() )
    {
        return ArchiveIterator();
    }

    return m_impl->m_impl->begin();
}

ArchiveIterator Archive::end()
{
    if ( !m_impl->ensureOpen() )
    {
        return ArchiveIterator();
    }

    return m_impl->m_impl->end();
}

ArchiveIterator Archive::find( const QString& fName )
{
    if ( !m_impl->ensureOpen() )
    {
        return ArchiveIterator();
    }

    return m_impl->m_impl->find( fName );
}


}  // of namespace bugless
