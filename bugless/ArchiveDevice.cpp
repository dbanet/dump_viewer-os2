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

/*! \file ArchiveDevice.cpp
\brief Source for ArchiveDevice class.
*/

#include <qstring.h>
#include <qstringlist.h>
#include <qiodevice.h>
#include <qfile.h>
#include <zlib.h>

#include <qfileinfo.h>
#include <qdatetime.h>

#include "ArchiveDevice.h"
#include "ArchiveImpl.h"
#include "Archive.h"


namespace bugless
{

ArchiveDevice::ArchiveDevice( ArchiveImpl* a )
    : m_addFileName(QString::null)
    , m_addFileIODevice( NULL )
    , m_password(QString())
    , m_nTotalWritten( 0 )
    , m_archiveImpl( a )
{

}

void ArchiveDevice::setPassword(const QString &pass) 
{ 
    m_password = pass;
}

void ArchiveDevice::setAddFileName(const QString &fileName) 
{ 
    m_addFileName = fileName;
}

void ArchiveDevice::setAddFileTime( const QDateTime& lastModified )
{
    m_addFileTime = lastModified;
}

void ArchiveDevice::setAddFileIODevice( QIODevice* iodev )
{
    m_addFileIODevice = iodev;
}

#ifdef QT3
bool ArchiveDevice::open(int mode)
#else
bool ArchiveDevice::open(OpenMode mode)
#endif
{
    bool ret = false;
#ifdef QT3
    qDebug("ArchiveDevice::open: mode %d", mode);
#else
    qDebug("ArchiveDevice::open:");
#endif

#ifdef QT3
    if (mode & IO_Raw)
#else
    if (mode & QIODevice::Unbuffered)
#endif
    {
        // We can't support raw mode
        return false;
    }

#ifdef QT3
    if (mode & IO_Translate)
#else
    if (mode & QIODevice::Text)
#endif
    {
        // Doesn't make any sense for us.
        return false;
    }

#ifdef QT3
    if (mode & IO_ReadOnly)
#else
    if (mode & QIODevice::ReadOnly)
#endif
    {
        ret = open(ArchiveImpl::Decompress);

#ifndef QT3
        if (ret)
        {
            setOpenMode(mode);
        }
#endif

        return ret;
    }

#ifdef QT3
    if ((mode & IO_WriteOnly) || (mode & IO_ReadWrite) || (mode & IO_Append))
#else
    if ((mode & QIODevice::WriteOnly) || (mode & QIODevice::ReadWrite) ||
        (mode & QIODevice::Append))
#endif
    {
        // Archives are either opened for read OR for write, not both. So for
#ifdef QT3
        // IO_ReadWrite we assume we are being opened for write.
        if (mode & IO_Truncate)
#else
        // QIODevice::ReadWrite we assume we are being opened for write.
        if (mode & QIODevice::Truncate)
#endif
        {
            ret = open(ArchiveImpl::Create);
        }
        else
        {
            ret = open(ArchiveImpl::Add);
        }

#ifndef QT3
        if (ret)
        {
            setOpenMode(mode);
        }
#endif

        return ret;
    }

    return false;
}


#ifdef QT3
void ArchiveDevice::flush()
{
    // This has no effect as the underlying implementation is not buffered.
}
#endif


#ifdef QT3
Q_LONG ArchiveDevice::readLine(char *data, Q_ULONG maxlen) 
#else
qint64 ArchiveDevice::readLine(char *data, qint64 maxlen) 
#endif
{
	qDebug("readLine: up to %d", (int)maxlen);
	uint nRead = 0;
	bool finished = false;

	while (nRead < maxlen && !finished)
	{
#ifdef QT3
		char c = getch();
#else
		char c = getChar();
#endif
	
		if (c == -1)
		{
			// EOF
			break;
		}
	
		if ('\n' == c)
		{
			// Found the end of line
			finished = true;
		}

		data[nRead] = c;
		nRead++;
	}

	if (nRead < maxlen)
	{
		// Stick a terminating null in if there is space.
		data[nRead + 1] = '\0';
	}

	return (int)nRead;
}

#ifdef QT3
int ArchiveDevice::getch()
#else
int ArchiveDevice::getChar()
#endif
{
    char c = '\0';

#ifdef QT3
    int nRead = readBlock(&c, 1);
#else
    int nRead = readData(&c, 1);
#endif

    if (nRead == 1)
    {
        qDebug("getch: %c", c);
        return c;
    }
    else
        return -1;
}

#ifdef QT3
int ArchiveDevice::putch(int ch)
#else
int ArchiveDevice::putChar(int ch)
#endif
{
    char c = ch;
    qDebug("putch: %c", c);

#ifdef QT3
    int nWritten = writeBlock(&c, 1);
#else
    int nWritten = writeData(&c, 1);
#endif

    if (nWritten == 1)
        return ch;
    else
        return -1;
}

#ifdef QT3
int ArchiveDevice::ungetch(int ch)
{
    qDebug("ungetch:");
    // To prevent compiler warnings
    ch = ch;

    return -1;
}
#endif


} // of namespace bugless
