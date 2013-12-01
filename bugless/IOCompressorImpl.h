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


#include <qiodevice.h>
#include <qbytearray.h>
#include <qbuffer.h>


namespace bugless
{


// An IOCompressor provides access to an underlying compressed QIODevice.
/*!
    Allows you to access compressed data like any other QIODevice.

    In the default implementation, the uncompressed data is buffered to
    reduce compression and decompression stages.

    This is useful when used in conjunction with ArchiveImpl which
    must open/close devices as it switches between reading and writing to
    its underlying QIODevice.
 */
class IOCompressorImpl
{
public:
    IOCompressorImpl( QIODevice* device );
    virtual ~IOCompressorImpl();

    //! Force a write of the buffered data to wrapped QIODevice
    /*!
        A flush should normally be followed closely by a close.
    */
    void flushBuffer();

    QIODevice* buffer() { return &m_buffer; }

protected:

    virtual QByteArray decompress( QByteArray& data );
    virtual QByteArray compress( QByteArray& data );
private:
    IOCompressorImpl(const IOCompressorImpl& o);
    IOCompressorImpl& operator=(const IOCompressorImpl& o);

private:
    QIODevice* m_device;

    QByteArray m_data;
    QBuffer m_buffer;
};


}  // of namespace bugless

