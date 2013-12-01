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

#include <map>

#include <qstring.h>

#include "Archive.h"

class QIODevice;

namespace bugless
{

class ArchiveImpl;

//! An abstract interface to be implemented by archive creators.
class ArchiveCreatorInterface
{
public:
    virtual ~ArchiveCreatorInterface() {}
	virtual Archive::Type type() const = 0;
	virtual ArchiveImpl* create() const = 0;
	virtual bool couldBe( QIODevice* dev ) const = 0;;
	virtual QString extension() const = 0;
};

//! An ArchiveFactory.
/*!
    Archive factory is a central point for registering, creating
    and querying Archives by Type.
*/

class ArchiveFactory
{
public:
    static ArchiveFactory* instance();

	//! List of registered types.
    QList<Archive::Type> types();

	//! Create an Archive of Type \a type
	/*!
		Returnds a new Archive if successful, otherwise false.
	*/
    ArchiveImpl* create( Archive::Type type );

    //! Peek at \dev to see if the Archive looks like it is of Type \a type
    bool couldBe( Archive::Type type, QIODevice* dev );

    //! Returns the typical file extension for this type.
    QString extension( Archive::Type type );

	//! Register an ArchiveCreator for Type \a type.
    void registerArchiveType( Archive::Type type, ArchiveCreatorInterface* );

private:
    ArchiveFactory();
    typedef std::map<Archive::Type, ArchiveCreatorInterface*> ArchiveCreators;
    ArchiveCreators m_creators;
};


}  // of namespace bugless

