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

// \todo find a way to register archive types without explicitly 
// pulling in the headers.  tried usinng global creator objects 
// the relevant Impl source files but these symbols are loaded unless
// you explicitly call them.

#ifdef BUGLESS_ZIP
#include "ZipImpl.h"
#endif
#ifdef BUGLESS_TAR
#include "TarImpl.h"
#endif

#include "ArchiveFactory.h"


namespace bugless
{

ArchiveFactory::ArchiveFactory()
{
    // force dynamic global initialisation 
#ifdef BUGLESS_ZIP
    ZipImpl zip;
#endif
#ifdef BUGLESS_TAR
    TarImpl tar;
#endif
}

ArchiveFactory* ArchiveFactory::instance()
{
    static ArchiveFactory factory;
    return &factory;
}

void ArchiveFactory::registerArchiveType( Archive::Type type, ArchiveCreatorInterface* creator )
{
    m_creators.insert( std::pair<Archive::Type,ArchiveCreatorInterface*>(type,creator) );
}

ArchiveImpl* ArchiveFactory::create( Archive::Type type )
{
    if ( m_creators.count( type ) == 0 )
    {
        return NULL;
    }

    return m_creators[type]->create();
}

bool ArchiveFactory::couldBe( Archive::Type type, QIODevice* dev )
{
    if ( m_creators.count( type ) == 0 )
    {
        return NULL;
    }

    return m_creators[type]->couldBe( dev );
}

QString ArchiveFactory::extension( Archive::Type type )
{
    if ( m_creators.count( type ) == 0 )
    {
        return NULL;
    }

    return m_creators[type]->extension();
}

QList<Archive::Type> ArchiveFactory::types()
{
    QList<Archive::Type> types;
    ArchiveCreators::const_iterator I = m_creators.begin();
    ArchiveCreators::const_iterator E = m_creators.end();
    for ( ; I!=E; ++I )
    {
        types.push_back( I->second->type() );
    }

    return types;
}

}  // of namsepace bugless
