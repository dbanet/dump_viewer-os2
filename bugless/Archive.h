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

/*! \file Archive.h
\brief Header file containing the definition of the Archive class.
*/

#pragma once

#include <qglobal.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>

#include "ArchiveIterator.h"

class QFile;

/*! \namespace bugless
    \brief Namespace for bugless projects
*/
namespace bugless
{
class ArchivePrivate;

//! The Archive class provides a convenient interface for working with archive files.
/*! 
 * Archive defines an archive of compressed files.
 *
 * This class provides a way to create and manipulate archives.
 *
 * Instantiate the object, set the file name and then open it. If encryption is
 * to be used then set the password at this point.
 *
 * Then either add files to a new or existing archive or read files from an
 * archive. 
 */
class Archive : public QObject
{
    Q_OBJECT
public:
    //! Type enum
    /*! 
        This enum defines the type of Archive used when creating an Archive.
        \todo Need to think of a more extensible method of enumerating possible
        types that doesn't need modifications to this header (and doesn't 
        involve QString and QStringList).
    */
    enum Type {
               NONE = 0         /*!< Auto detected on open */
#ifdef BUGLESS_ZIP
               ,ZIP = 1         /*!< ZIP Archive.  */
#endif
#ifdef BUGLESS_TAR
               ,TAR = 2         /*!< TAR Archive   */
#endif
               };

    Q_ENUMS(Type)

    //! Creates an invalid archive.
    /*!
        To become valid() specify the Archive name and type.
        \sa setType(), setArchiveName()
    */
    Archive();

    //! Creates an Archive from QIODevice \a device of Type \a type.
    /*!
        \sa setType(), setArchiveName()
    */
    Archive( QIODevice* device, const Type& type = NONE );
    
    //! Creates an Archive from file \a fileName of Type \a type.
    /*!
        \sa setType(), setArchiveName()
    */
    Archive( const QString& fileName, const Type& type = NONE );

    //! The destructor closes the Archive if it is still open.
    virtual ~Archive();

    //! Set the size of the buffer used for reading and writing files to and from the archive.
    /*!
        The buffer size should only be set on an Archive that is not already open.
        Default is 10240
    */
    void setBufferSize( int bufSize );

    //! Set the password to use when using encrypted Archives.
    /*!
        Encryption is only currently supported for ZIP archives.
    */
    void setPassword( const QString& );

    /// Sets the file name of the archive to be opened/created.
    /*!
        \a archiveName will be used to open an existing file or create a 
        new file if it doesn't already exist.

        Example:
        \code
            Archive a;
            a.setArchiveName( "myzip.zip" );
            a.addFile( "foo.txt" );
        \endcode

        Note that \a archiveName can be set either when creating an Archive
        or by using setArchiveName.  The archive name must have been set
        before open() can be called - unless the Archive was created directly
        from a QIODevice*.
    */
    bool setArchiveName( const QString& archiveName );

    //! Sets the Archive type.
    /*!
        Specifies the Archive type to be used.

        If the Type is NONE then the type will be auto-detected when opening archives.

        Auto detection heuristics are based initially on file extension and then on byte sniffing.
        To avoid the overhead of auto detection specify the \a type explicitly.

        Example:

        \code
            Archive a;
            a.setArchiveName( "actuallyazip.qtz" );
            a.setType( Archive::ZIP );
            a.extractAll();
        \endcode
    */
    bool setType( const Type type );
    Type type() const;

    //! Is the Archive valid.
    /*!
        A valid Archive has been successfully created with a supported Type.

        \sa setType(), create()
    */
    bool valid() const;

    //! Opens an archive.
    /*! 
        Opens the archive and prepares it for first use.

        Example:
        \code
        #include "Archive.h"

        using namespace bugless;

        Archive* a = new Archive("my.zip");
        a->open();
        // use it...

        \endcode

        This will create an empty Archive. Files can then be added to this
        archive using the addFile() method.

        Note that if an archive has not been opened then calling addFile(),
        removeFile(), extractFile() etc will automatically open the Archive.

        \sa close()
    */
    bool open();

    //! Closes the archive.
    /*!
        When finished with an archive object use close() to finalize the archive. 
        An archive object that has been closed can be reopened with a new or different file.

        close() is automatically called from the destructor and is only necessary if you want to
        reuse an archive.

        \sa open()
    */
    void close();

    /// Adds a new file to the archive.
    /*! 
        \a fileName is the name of the file to add. It can be a relative or
        absolute path. 
        When the file is extracted from the archive the same path will be 
        used to create the new file.

        \returns true if the file was successfully added

        \sa addFiles(), addFile( const QString&, QIODevice* )
     **/
    bool addFile( const QString &fileName );

    //! Add multiple files to the archive.
    /*!
        As for addFile(), but allows multiple files to be added at once.

        Equivalent to:
        \code
        foreach( QString file, fileNames )
            a.addFile( file )
        \endcode

        \returns true if all files were successfully added
    */
    bool addFiles( const QStringList &fileNames );

    //! Adds a file to the archive
    bool addFile( QFile& file );

    /// Adds a new file to the archive.
    /*! 
        \param dev I/O device for the data to be archived as file \a fileName
        \param fileName is the file name to use. 
        
        \sa addFile( const QString& )
     */
    bool addFile( const QString& fileName, QIODevice* dev );

    /// Adds a file to the archive.
    /*! 
        \param data data to be archived as file \a fileName
        \param fileName is the file name to use. 
        
        \sa addFile( const QString& )
     */
    bool addFile( const QString& fileName, QByteArray& data );

    /// Removes a single file from the archive.
    /*! 
        \param fileName is the name of the file to remove. 
        
        \returns true if the file was in the archive and could be 
        removed from it.

        If there is more than one file to be removed consider using 
        removeFiles instead
        \sa removeFiles()
    */
    bool removeFile( const QString &fileName );

    /// Remove a set of files.
    /*!
        As above, but more efficient if removing multiple files.

        \returns true if at least one file was removed and there were no other errors.

        \sa removeFile()
    */
    bool removeFiles( const QStringList &fileNames );

    /// Remove all files from the archive
    /*!
        Use this if, for example, you want to clear out an existing 
        archive prior to adding new files.

        \returns true if all the files could be removed
    */
    bool removeAll();

    //! Extracts the named file.
    /*!
        \param fileName the Archive member file to extract
        \param dir directory to extract into
        \param newName name of new file

        The optional \a dir parameter can be used to specify the directory that the 
        files will be extracted into. If it is omitted the files will be extracted to
        the current working directory. Either way the file names and paths used when
        the archive was created will be preserved. So extracting the file source/test.cpp
        to the directory tmp will create the file as tmp/source/test.cpp.

        The optional \a newName parameter can be used to specify the file name to use 
        when the file is extracted. So extracting the file \a source/test.cpp with \a newName 
        set to file.cpp will create the file \a file.cpp. 
        
        Example:
        \code
            Archive a( "myarchive.zip" );
            a.extractFile( "file.txt" );
        \endcode

    */
    bool extractFile( const QString& fileName, const QDir& dir = QDir(), const QString& newName = QString() );

    //! Extracts a file from the archive directly to a QIODevice.
    /*!
        \param fileName the Archive member file to extract
        \param dev the I/O device to write the data into.

        Extracted data will be appended to \a dev.
    */
    bool extractFile( const QString& fileName, QIODevice* dev );

    //! Extracts a file from the archive directly to a QByteArray
    /*!
        \param fileName the archive member file to extract
        \param data the QByteArray device to write the data into.

        Extracted data will be appended to \a data.
    */
    bool extractFile( const QString& fileName, QByteArray& data );

    //! Extracts all files from the archive.
    /*! \param dir location to extract to 
    
        If \a dir is not specified files will be extracted to the
        current working directory .
    */
    bool extractAll( const QDir& dir = QDir() );
    
    //! Returns a QIODevice* handle to the named archive member file.
    /*!
        \param fileName file name of the archive member.

        This allows the archive to be used in combination with QDataStream
        and QTextStream.

        Example:
        \code
        // create a file in the archive and write text to it
        Archive a( "myarchive.zip" );
        QIODevice* dev = a.device( "text1.txt" );
        dev->open(QIODevice::WriteOnly) )
        QTextStream ts(dev);
        ts << "This is me writing text to text1.txt" << endl;
        dev->close();
        \endcode

        \returns a QIODevice* handle to the named file.
    */
    QIODevice* device( const QString& fileName );
    
    //! List of the files in the archive
    /*!
        \returns the full paths of files in the archive.
    */
    QStringList files();
    
    //! Returns an iterator to the start of the archive.
    /*!
        \sa end() 
    */
    ArchiveIterator begin();
    
    //! Returns an iterator to the end of the archive.
    /*!
        \sa begin() 
    */
    ArchiveIterator end();

    //! Returns an iterator to the named file.
    /*!
        \sa begin(), end(), files() 
    */
    ArchiveIterator find( const QString& fileName );

private:
    Q_DISABLE_COPY(Archive)

    friend class ArchiveIterator;
    ArchivePrivate* m_impl;
};


}  // of namespace bugless

