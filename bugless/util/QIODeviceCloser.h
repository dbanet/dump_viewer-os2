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

namespace bugless
{

//! An automatic QIODeviceCloser
class QIODeviceCloser
{
public:
    QIODeviceCloser( QIODevice* io, QIODevice::OpenMode openMode, QIODevice::OpenMode restoreMode = QIODevice::NotOpen )
        : m_io( io )
        , m_restoreMode( restoreMode )
    {
        m_io->open( openMode );
    }
    
    ~QIODeviceCloser()
    {
        m_io->close();
        if ( m_restoreMode != QIODevice::NotOpen )
        {
            m_io->open( m_restoreMode );
        }
    }

    QIODevice* operator->() { return m_io; }
private:
    QIODevice* m_io;
    QIODevice::OpenMode m_restoreMode;
};

} // of namespace bugless