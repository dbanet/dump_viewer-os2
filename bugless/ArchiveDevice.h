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

/*! \file ArchiveDevice.h
\brief Header file containing the definition of the ArchiveDevice class.
*/

#pragma once

#include <qdatetime.h>

#include "ArchiveImpl.h"

namespace bugless
{
/// A file within an archive.
/*! ArchiveDevice subclasses QIODevice so that it can be used in combination with
 * QDataStream and QTextStream.
 *
 * These objects should not be created in isolation, rather Archive::device()
 * should be used to return a pointer to the ArchiveDevice created within the
 * archive.
 *
 */
class ArchiveDevice : public QIODevice
{
public:
    virtual ~ArchiveDevice() {};

    // Main functionality 
    // Setters and getters
    /// Returns the name of the current file. const?
    virtual QString fileName() const = 0;

    void setPassword( const QString& password );

    // \todo group these together
    /// Sets the name of the file to be read from or written.
    void setAddFileName(const QString &fileName);

    /// Set the last modified time of the current file.
    void setAddFileTime( const QDateTime& lastModified );

    //! Set the iod evice that is providing the data for Add.
    void setAddFileIODevice( QIODevice* iodev );

    /// Opens a file within an archive.
    /*! \a mode argument specifies how to open the file. mode is one of
     * Create, Add or Decompress.
     * calls open( OpenMode )
     **/
    virtual bool open( ArchiveImpl::Mode mode) = 0;

public:
    // Virtual methods from QIODevice
    /// Opens a file within an archive.
    /*! \a mode argument specifies how to open the file. mode is one of the
     * standard QIODevice mode flags - IO_ReadOnly, IO_WriteOnly, IO_ReadWrite,
     * IO_Append or IO_Truncate. IO_Raw and IO_Translate are not supported.
     **/
#ifdef QT3
    virtual bool open( int mode );
#else
    virtual bool open( OpenMode mode );
#endif

    /// Closes the current file.
    virtual void close() = 0;
#ifdef QT3
    /// Flushes output to the current file.
    virtual void flush();
#endif

    /// Returns the uncompressed size of the file if reading from the archive.
#ifdef QT3
    virtual Q_ULONG size() const = 0;
#else
    virtual qint64 size() const = 0;
#endif

    /// Reads a block of data from the current file.
    /*! \a data points to the buffer used to hold the data read from the
     * file.
     * \a maxlen is the size of the data buffer in bytes.
     **/
#ifdef QT3
    virtual Q_LONG readBlock( char *data, Q_ULONG maxlen ) = 0;
#else
    virtual qint64 readData( char *data, qint64 maxlen ) = 0;
#endif
    /// Writes a block of data to the current file.
    /*! \a data points to the buffer containing data to be written to the
     * file.
     * \a len is the length of data in the buffer in bytes.
     **/
#ifdef QT3
    virtual Q_LONG writeBlock( const char *data, Q_ULONG len ) = 0;
#else
    virtual qint64 writeData( const char *data, qint64 len ) = 0;
#endif
    /// Reads a line of data from the current file.
    /*! \a data points to the buffer used to hold the data read from the
     * file.
     * \a maxlen is the size of the data buffer in bytes.
     **/
#ifdef QT3
    virtual Q_LONG readLine( char *data, Q_ULONG maxlen );
#else
    virtual qint64 readLine( char *data, qint64 maxlen );
#endif

    /// Reads a single character from the current file.
#ifdef QT3
    virtual int getch();
#else
    int getChar();
#endif
    /// Writes a single character to the current file.
    /*! \a ch character to be written to the current file in the archvie.
     **/
#ifdef QT3
    virtual int putch(int ch);
#else
    int putChar(int ch);
#endif
#ifdef QT3
    /// ungetch is not supported and will always return -1
    virtual int ungetch(int ch);
#endif

#ifndef QT3
    /// Only sequential access is supported i.e. always returns true.
    virtual bool isSequential() {return true;}
#endif

#ifndef QT3
    /// Returns the current position in the file
    virtual qint64 pos() const = 0;
#endif

    /// Returns true if the current read position is at the end of the
    /// file. This always returns true as this is a sequential device.
    virtual bool atEnd() const {return true;}

protected:
    ArchiveDevice(ArchiveImpl* a);
    // Don't think we'll ever want to copy or assign
    ArchiveDevice(const ArchiveDevice& o);
    ArchiveDevice& operator=(const ArchiveDevice& o);

    // \todo group these into a struct?
    //! Name of the file to be added.
    /*! 
        This is just the name to identify it in the archive.
    */
    QString m_addFileName;

    //! Last modification time of the file to be added.
    /*!
        \sa setFileTime
    */
    QDateTime m_addFileTime;

    //! I/O device to read data from for Add.
    /*!
        \sa setFileIODevice
    */
    QIODevice* m_addFileIODevice;

    ArchiveImpl::Mode m_mode;             // Mode that the archive was opened in
    QString m_password;
    qint64 m_nTotalWritten;
    friend class ArchiveImpl;
    ArchiveImpl* m_archiveImpl;
};


} // of namespace bugless

