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

/*! \file ArchiveIterator.h
\brief Header file containing the definition of the ArchiveIterator class.
*/

#pragma once

#include <qdir.h>
#include <qstring.h>

class QIODevice;

namespace bugless
{

class Archive;
class ArchiveImpl;

//! An ArchiveIterator allows you to iterate over the members of an Archive.
/*! ArchiveIterator defines an iterator class that is used to iterate over
   archive members.

   The following example illustrates how an iterator could be used to 
   extract all the png images from an archive.

   \code
    Archive a( "myzip.zip" );
    a.open( ArchiveImpl::Decompress );
    for ( ArchiveIterator I = a.begin(); I != a.end(); ++I )
    {
        if ( I->fileName().endsWith( "png" ) )
        {
            I->extractFile();
        }
    }
    a.close();
    \endcode

    \sa Archive::find(), Archive::begin(), Archive::end()
*/
class ArchiveIterator
{
public:
    //! Default constructor. 
    inline ArchiveIterator() : m_archive( NULL) {}
    //! Copy constructor.
    inline ArchiveIterator(const ArchiveIterator &o)
                    : m_archive(o.m_archive), m_name(o.m_name) {}
    //! Assignment operator.
    inline ArchiveIterator& operator=(const ArchiveIterator &o)
        { m_archive = o.m_archive; m_name = o.m_name; return *this; }

    //! Operator ->
    /*!
        Allows you to use traditional -> syntax when using Iterators.
    */
    ArchiveIterator* operator->() { return this; }

    //! Operator ==
    /*!
        Returns true if the Iterator is for the same file in the same Archive.
    */
    inline bool operator==(const ArchiveIterator &o) const
                    { return (m_archive==o.m_archive)&&(m_name==o.m_name); }
    //! Operator !=
    /*!
        Returns true if either the Archive or file is different.
    */
    inline bool operator!=(const ArchiveIterator &o) const
                    { return (m_archive!=o.m_archive)&&(m_name!=o.m_name); }

    //! Operator ++
    /*!
        Advances to the next file and returns an Iterator to it.  Returns
        an invalid Iterator (equivalent to Archive::end()) if there are no more
        files)
    */
    ArchiveIterator& operator++();

    //! Get an I/O device which can be used to read/write to an Archive member.
    /*!
        The QIODevice* must be opened before being used and closed when finished
        with.

        \sa Archive::device()
    */
    QIODevice* device();

    //! Extract the file to a target location
    /*!
        \sa Archive::extractFile()
    */
    bool extractFile( const QDir& dir = QDir(), const QString& newName = QString() );
    //! Extract the file to a QIODevice
    /*!
        \sa Archive::extractFile()
    */
    bool extractFile( QIODevice* dev );
    //! Name of the Iterator's file.
    QString fileName() const { return m_name; }

protected:
    friend class ArchiveImpl;

    ArchiveIterator( ArchiveImpl* a );
    void setArchive( ArchiveImpl* a );
    void setName( const QString& n );

private:
    ArchiveImpl* m_archive;
    QString m_name;
};


} // of namespace bugless

