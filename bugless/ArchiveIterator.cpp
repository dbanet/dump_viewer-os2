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
** See http://www.bugless.co.uk/archive/licence/ for Archive licensing information.
** See http://www.bugless.co.uk/gpl/ for GPL licensing information.
**
** Contact info@bugless.co.uk if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

/*! \file ArchiveIterator.cpp
\brief Source for ArchiveIterator class.
*/

#include "ArchiveIterator.h"
#include "Archive.h"
#include "ArchiveImpl.h"
#include "ArchiveDevice.h"

namespace bugless 
{

ArchiveIterator::ArchiveIterator( ArchiveImpl* a ) 
    : m_archive(a) 
{}
    
void ArchiveIterator::setArchive( ArchiveImpl* a ) 
{ 
    m_archive = a; 
}

void ArchiveIterator::setName( const QString& n ) 
{ 
    m_name = n; 
}
    
ArchiveIterator& ArchiveIterator::operator++()
{
    if ( m_archive->nextFile() )
    {
        m_name = m_archive->m_dev->fileName();
    }
    else
    {
        // We've hit the last file in the archive so mark that with a null 
        // string.
        m_name = QString("");
    }

    return *this;
}

QIODevice* ArchiveIterator::device()
{
    return m_archive->device( m_name );
}

bool ArchiveIterator::extractFile( const QDir& dir, const QString& newName )
{
    return m_archive->extractFile( m_name, dir, newName );
}

bool ArchiveIterator::extractFile( QIODevice* dev )
{
    return m_archive->extractFile( m_name, dev );
}


} // of namespace bugless

