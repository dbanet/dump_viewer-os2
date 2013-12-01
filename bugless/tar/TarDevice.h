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

/*! \file TarDevice.h
\brief Header file containing the definition of the TarDevice class.
*/

#pragma once

#include "Tar.h"

#include "ArchiveDevice.h"

namespace bugless
{

class TarImpl;

/// A file within a tar archive.
/*!
    Tar file members consist of a 512 byte header
    followed by the file data.

    Header information is read on opening for Decompress
    and written when closing a file that has been opened
    for Add.
*/
class TarDevice : public ArchiveDevice
{
public:
    virtual ~TarDevice() {};

    virtual bool open( ArchiveImpl::Mode mode );
    virtual void close();

    virtual QString fileName() const;

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
    friend class TarImpl;
    //! Set the header info for this tar file.
    /*!
        This effectively makes this device refer to 
        a new file in the archive.
    */
    void setHeaderInfo( tar::TarHeader& );
    const tar::TarHeader& headerInfo() const { return m_headerInfo; }

    TarDevice( TarImpl* );

private:    
    TarImpl* m_tarImpl;

    // track bytes read
    qint64 m_nTotalRead;
    
    //! Data from/for the header.
    tar::TarHeader m_headerInfo;
};


} // of namespace bugless


