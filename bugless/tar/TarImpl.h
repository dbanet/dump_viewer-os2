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

#include "ArchiveImpl.h"
#include "TarDevice.h"

namespace bugless
{


//! A Tar archive.
/*! 
    A basic implementation of Tar format archives.  Minimal support
    for all the various Tar options and modes.
*/
class TarImpl : public ArchiveImpl
{
public:
    TarImpl();
    virtual ~TarImpl();

    virtual bool removeFiles( const QStringList &fileNames );
    virtual bool extractFile( const QString& fName, const QDir& dir = QDir(), const QString &newName = QString(), QString* outFileName = NULL );
    
    QIODevice* ioDevice() { return m_ioDevice; }

protected:
    virtual bool changeModeTo( const ArchiveImpl::Mode mode );

private:
    virtual bool firstFile();
    virtual bool nextFile();

    TarDevice* tarDevice() { return static_cast<TarDevice*>(m_dev.get()); }
};

}  // of namespace bugless


