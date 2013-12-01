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

#pragma once

// qt
#include <qstring.h>
#include <qiodevice.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qdatetime.h>
#ifdef QT3
#include <qmemarray.h>
#else
#include <qbytearray.h>
#endif

// local
#include "Archive.h"
#include "ArchiveIterator.h"

// std
#include <memory>

class QIODevice;
class IOCompressorImpl;

namespace bugless
{


class ArchiveDevice;

// A ArchiveImpl archive.
/* ArchiveImpl defines an archive of compressed files.
 *
 * This class provides a way to create and manipulate zip archives.
 *
 * Instantiate the object, set the file name and then open it. If encryption is
 * to be used then set the password at this point.
 *
 * Then either add files to a new or existing archive or read files from an
 * archive. It is not possible to combine reading and writing - the archive
 * must be opened in one mode or the other.
 *
 */
class ArchiveImpl
{
public:
    ArchiveImpl();
    virtual ~ArchiveImpl();

    //! Mode enum. - obsolete
    //! This enum defines the current mode the archive is in.
    /*!
        When adding, removing or extracting files the Archive must be in the appropraite
        mode.  This is done behind the scenes and different Archive implementations
        must implement changeModeTo to do 'the right thing'.

        \sa ensureInMode(), changeModeTo()
    */
    enum Mode {
               Closed,      /*!< The archive is closed. */
               Create,      /*!< Used to create a new archive. Existing contents will be lost */
               Add,         /*!< Used when appending files to an existing archive. */
               Remove,      /*!< Used when removing file from an existing archive  */
               Decompress   /*!< Used to extract files from an existing archive.   */
             };

    //! Set the size of the buffer used during un/decompression.
    /*!
        Should only be set on an Archive thar is not already open.
    */
    void setBufferSize( int bufSize );

    //! Set the password;
    void setPassword( const QString& password );

    // Opens an archive.
    virtual bool open();
    // Closes the archive.
    virtual void close();

    //! Is the archive open.
    bool isOpen() const { return m_open; }

    // Sets the I/O device for the archive.
    bool setIODevice( QIODevice* );

    // Adds a new file to the archive.
    /* fileName is the name of the file to add. The archive must have
     * been opened in Create or Add mode.
     */
    bool addFile( const QString &fileName);
    bool addFile( const QString &fileName, QIODevice*, const QDateTime& lastModified = QDateTime() );

    //! Removes \a fileName from the Archive.  
    /*! 
        Default implementation uses removeFiles
        \returns true if successful.
    */
    virtual bool removeFile( const QString &fileName );
    //! Removes \a fileNames from the Archive.
    /*!
        \returns true if successful. 
    */
    virtual bool removeFiles( const QStringList &fileNames ) = 0;

    //! Removes all files from the Archive.
    /*!
        Does this by closing the Archive and truncating our QIODevice.
    */
    bool removeAll();

    // Extracts the named file from the archive.
    /* The archive must have been opened in Decompress mode.
     */
    bool extractFile( const QString& fileName, QIODevice* dev );
    
    // Extracts the current file from the archive.
    /* The archive must have been opened in Decompress mode.
        \param outFileName if supplied, will be set to the name of the extracted file
     */
    virtual bool extractFile( const QString& fName, const QDir& dir = QDir(), const QString &newName = QString(), QString* outFileName = NULL );

    // Extracts all files from the archive.
    /* The archive must have been opened in Decompress mode.
     */
    bool extractAll(const QDir& dir = QDir());

    // Returns a handle to the archive's zipDevice object.
    /* This allows the archive to be used in combination with QDataStream
     * QTextStream.  
     * \param fName if specified, sets the current file name on the device.
     * if fName is empty it will return the current device.
     */
    virtual ArchiveDevice* device( const QString &fName );

    //! List the files in the Archive.
    QStringList files() const;
    
    // Returns an iterator to the start of the archive.
    ArchiveIterator begin();

    // Returns an iterator to the end of the archive.
    ArchiveIterator end();

    // Returns an iterator to the named file.
    ArchiveIterator find( const QString& fName );

    //! Change mode to \a mode.
    /*!
        \return true if mode succesfuly changed
    */
    bool ensureInMode( const ArchiveImpl::Mode mode );

private:
    // Let's keep this simple
    ArchiveImpl(const ArchiveImpl& o);
    ArchiveImpl& operator=(const ArchiveImpl& o);

protected:
    virtual bool changeModeTo( const ArchiveImpl::Mode mode );

    void init(int bufSize = 10240);

    //! Position ourselves at the first file.
    virtual bool firstFile() = 0;
    //! Advance to the next file.
    virtual bool nextFile() = 0;
    //! Make file \fileName current.
    /*!
        Default implementation iterates through the archive files
        using firstFile and nextFile.
    */
    virtual bool gotoFile(const QString &fileName);

    //! Does the archive contain a file with name \a fileName
    /*!
        Default implementation iterates through the archive files
        using firstFile and nextFile.
    */
    virtual bool containsFile(const QString &fileName) const;

    QIODevice* m_ioDevice;
    QByteArray m_buf;           // Internal buffer
    ArchiveImpl::Mode m_mode;
    bool m_open;
    std::auto_ptr<ArchiveDevice> m_dev;
    QString m_password;

    std::auto_ptr<IOCompressorImpl> m_compressor;

    friend class ArchiveIterator;
};


}  // of namespace bugless

