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

#include "IOCompressorImpl.h"

namespace bugless
{


IOCompressorImpl::IOCompressorImpl( QIODevice* device )
    : m_device( device )
{
    Q_ASSERT( !m_buffer.isOpen() );
    // only need to read in the first time we're opened
    m_device->open( QIODevice::ReadOnly );
    QByteArray data = m_device->readAll();
    m_data = decompress( data );
    m_buffer.setBuffer( &m_data );
    m_device->close();
}

IOCompressorImpl::~IOCompressorImpl()
{
}

void IOCompressorImpl::flushBuffer()
{
    m_device->open( QIODevice::WriteOnly|QIODevice::Truncate );
    Q_ASSERT( m_data.size() == m_buffer.size() );
    qDebug( "Size of data: %d", m_data.size() );
    m_device->write( compress( m_data ) );
    m_device->close();
}

QByteArray IOCompressorImpl::decompress( QByteArray& data )
{
    return data;
}

QByteArray IOCompressorImpl::compress( QByteArray& data )
{
    return data;
}

}  // of namespace bugless
