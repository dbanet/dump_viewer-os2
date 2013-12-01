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

/*! \file ZipDevice.h
\brief Header file containing the definition of the ZipDevice class.
*/

#pragma once

#include "zip/qzip.h"
#include "zip/unzip.h"

#include "ArchiveDevice.h"

namespace bugless
{

class ZipImpl;

/// A file within a zip archive.
class ZipDevice : public ArchiveDevice
{
public:
    virtual ~ZipDevice() {};

    virtual QString fileName() const;
    virtual bool open( ArchiveImpl::Mode mode );

public:
    // Virtual methods from ArchiveDevice/QIODevice
    virtual void close();

#ifdef QT3
    virtual Q_ULONG size() const;
#else
    virtual qint64 size() const;
#endif

#ifdef QT3
    virtual Q_LONG readBlock( char *data, Q_ULONG maxlen );
#else
    virtual qint64 readData( char *data, qint64 maxlen );
#endif

#ifdef QT3
    virtual Q_LONG writeBlock( const char *data, Q_ULONG len );
#else
    virtual qint64 writeData( const char *data, qint64 len );
#endif

#ifndef QT3
    /// Returns the current position in the file
    virtual qint64 pos() const;
#endif

protected:
    friend class ZipImpl;
    ZipDevice( ZipImpl* a );
    
    /// Give the ZipDevice object the handles created by the zip archive.
    void setZipFileHandles(unzFile unzip, zipFile zip);

private:
    unzFile m_unzipFile;
    zipFile m_zipFile;
};


} // of namespace bugless


