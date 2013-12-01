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



#include "zip/qzip.h"
#include "zip/unzip.h"

#include "ArchiveImpl.h"
#include "ZipDevice.h"

namespace bugless
{


// A ZipImpl archive.
/* ZipImpl defines an archive of compressed files.
 *
 * This class provides a way to create and manipulate zip archives.
 *
 * Then either add files to a new or existing archive or read files from an
 * archive. It is not possible to combine reading and writing - the archive
 * must be opened in one mode or the other.
 *
 \todo maintain a local cache of the names of files within the archive.  reduces
 overhead and switching between modes when checking if files are in the archive.
 */
class ZipImpl : public ArchiveImpl
{
public:
    ZipImpl();
    virtual ~ZipImpl();

    virtual bool removeFiles( const QStringList &fileNames );
    virtual bool extractFile( const QString& fName, const QDir& dir = QDir(), const QString &newName = QString(), QString* outFileName = NULL );

protected:
    virtual bool changeModeTo( const ArchiveImpl::Mode mode );

private:
    // only valid if we have an unzFile.  makes first file current.
    virtual bool firstFile();
    // only valid if we have an unzFile.  makes next file current.
    virtual bool nextFile();
    virtual bool gotoFile(const QString &fileName);
    
    virtual bool containsFile(const QString &fileName) const;

    //! unzip file.  valid during Decompress.
    unzFile m_unzipFile;
    //! zip file. valid during Add/Create
    zipFile m_zipFile;

    ZipDevice* zipDevice() { return static_cast<ZipDevice*>(m_dev.get()); }
};


}  // of namespace bugless


