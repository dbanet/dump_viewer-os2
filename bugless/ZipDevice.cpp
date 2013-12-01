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

/*! \file ZipDevice.cpp
\brief Source for ZipDevice class.
*/

#include <zlib.h>

#ifdef QT3
# include <qstring.h>
# include <qstringlist.h>
# include <qiodevice.h>
# include <qfile.h>
# include <qfileinfo.h>
# include <qdatetime.h>
#else
# include <QString>
# include <QStringList>
# include <QIODevice>
# include <QFile>
# include <QFileInfo>
# include <QDateTime>

# include <QDebug>
#endif

#include "ZipDevice.h"
#include "ZipImpl.h"
#include "Archive.h"


namespace bugless
{


ZipDevice::ZipDevice( ZipImpl* a )
    : ArchiveDevice(a)
    , m_unzipFile(NULL)
    , m_zipFile(NULL)
{
}

void ZipDevice::setZipFileHandles(unzFile unzip, zipFile zip)
{
    m_unzipFile = unzip;
    m_zipFile = zip;
}

QString ZipDevice::fileName() const
{
    int err = UNZ_OK;
    unz_file_info fInfo;

    if ( m_unzipFile == NULL )
    {
        qDebug("fileName: file not opened for Decompress!");
        return QString();
    }


    // Get the file info. This contains such things as the file name length.
    if ((err = unzGetCurrentFileInfo(m_unzipFile, &fInfo, NULL, 0, NULL, 0,
                                     NULL, 0)) != UNZ_OK)
    {
        qDebug("fileName: failed to retrieve file info!");
        return QString();
    }

    // Now we have to call into unzip again just to get the file name!
    QByteArray fName, dummy1, dummy2;
    fName.resize(fInfo.size_filename);
    dummy1.resize(fInfo.size_file_extra);
    dummy2.resize(fInfo.size_file_comment);

    if ((err = unzGetCurrentFileInfo(m_unzipFile, NULL,
                        fName.data(), fName.size(), dummy1.data(), dummy1.size(),
                        dummy2.data(), dummy2.size())) != UNZ_OK)
    {
        qDebug("fileName: failed to retrieve file info!");
        return QString();
    }

    // Finally return the file info we want

    QString fileName = QString(fName.data());
#ifdef QT3
    fileName.setLength(fName.size());
#endif

    return fileName;
}


uLong calcCRC( QIODevice* iodev )
{
    const int CRCBUFSZ = 10000;
    uLong crc = crc32(0L, Z_NULL, 0);

    if ( !iodev )
    {
        return crc;
    }

#ifdef QT3
    QByteArray buf(CRCBUFSZ);
#else
    QByteArray buf(CRCBUFSZ, '\0');
#endif
    int nRead = 0;

#ifdef QT3
    iodev->open(IO_ReadOnly);
#else
    iodev->open(QIODevice::ReadOnly);
#endif

    while (true)
    {
#ifdef QT3
        if ((nRead = iodev->readBlock(buf.data(), CRCBUFSZ)) <= 0)
#else
        if ((nRead = iodev->read(buf.data(), CRCBUFSZ)) <= 0)
#endif
            break;

        crc = crc32(crc, (Bytef*)buf.data(), nRead);
    }

    iodev->close();

    return crc;
}

bool ZipDevice::open( ArchiveImpl::Mode mode )
{
    qDebug("ZipDevice::open: ArchiveImpl::Mode %d", mode);

    if (m_mode == mode)
    {
        // We are already in in the correct mode
        return true;
    }

    if ( !m_archiveImpl->ensureInMode( mode ) )
    {
        qDebug( "ZipDevice::open failed to change mode" );
        return false;
    }

    int err;

    switch (mode)
    {
        case ArchiveImpl::Closed:
        {
            qDebug("ZipDevice::open: invalid mode!");
            return false;
        }
        case ArchiveImpl::Create:
        case ArchiveImpl::Add:
        {
            if ( !m_zipFile )
            {
                // Need to be open
                qDebug("ZipDevice::open: open archive for Add or Create first!");
                return false;
            }

            if (m_addFileName.isEmpty())
            {
                // Need to set the file name first
                qDebug("ZipDevice::open: set file name first!");
                return false;
            }

            if ( !m_addFileIODevice && !m_password.isEmpty() )
            {
                // Need to set where the data comes from - not if a textstream
                qWarning("ZipDevice::open: streams and encrypted zip archives not supported");
                return false;
            }

            // This is used to record the file's timestamp
            zip_fileinfo zipfi;
            zipfi.dosDate = 0;
            zipfi.tmz_date.tm_sec = m_addFileTime.time().second();
            zipfi.tmz_date.tm_min = m_addFileTime.time().minute();
            zipfi.tmz_date.tm_hour = m_addFileTime.time().hour();
            zipfi.tmz_date.tm_mday = m_addFileTime.date().day();
            zipfi.tmz_date.tm_mon = m_addFileTime.date().month() - 1;
            zipfi.tmz_date.tm_year = m_addFileTime.date().year();
            zipfi.internal_fa = 0;      // Not sure what these fields are
            zipfi.external_fa = 0;      // for but decryption fails if they
                                        // aren't zero'd.

            uLong crc = 0;
            if ( !m_password.isEmpty() && m_addFileIODevice )
            {
                // This is horrible but we have to read in the entire file
                // to calculate it's crc as this goes into the header (and affects
                // the encryption keys - see crypthead) if the archive is to be encrypted.
                crc = calcCRC(m_addFileIODevice);
            }

#ifdef QT3
            err = zipOpenNewFileInZip3(m_zipFile, m_addFileName.ascii(), &zipfi, NULL, 0,
#else
            err = zipOpenNewFileInZip3(m_zipFile, m_addFileName.toAscii().constData(),
                                       &zipfi, NULL, 0,
#endif
                                       NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION,
                                       0,                      /*int raw*/
                                       15,                     /*int windowBits*/
                                       8,                      /*int memLevel*/
                                       Z_DEFAULT_STRATEGY,     /*int strategy*/
#ifdef QT3
                                       m_password.ascii(),     /*const char* password*/
#else
                                       m_password.isEmpty() ? NULL : m_password.toAscii().constData(),     /*const char* password*/
#endif
                                       crc);                   /*uLong crcForCrypting*/

            if (err != ZIP_OK)
            {
                qDebug("ZipDevice::open: failed open new file in archive %d", err);
                return false;
            }

            m_nTotalWritten = 0;
            break;
        }
        case ArchiveImpl::Decompress:
        {
            if (m_unzipFile == NULL)
            {
                // Need to set the unzip file first
                qDebug("ZipDevice::open: open archive for Decompress first!");
                return false;
            }

            if (!m_password.isEmpty())
            {
#ifdef QT3
                err = unzOpenCurrentFilePassword(m_unzipFile, m_password.ascii());
#else
                err = unzOpenCurrentFilePassword(m_unzipFile, m_password.toAscii().constData());
#endif
            }
            else
            {
                err = unzOpenCurrentFile(m_unzipFile);
            }

            if (err != UNZ_OK)
            {
                qDebug("ZipDevice::open: failed to open first file");
                return false;
            }
            break;
        }

        case ArchiveImpl::Remove:
        default:
            break;
    }

    m_mode = (ArchiveImpl::Mode)mode;

    return true;
}

void ZipDevice::close()
{
    switch (m_mode)
    {
        case ArchiveImpl::Closed:
            return;

        case ArchiveImpl::Create:
        case ArchiveImpl::Add:
            zipCloseFileInZip(m_zipFile);
            break;

        case ArchiveImpl::Decompress:
        case ArchiveImpl::Remove:
        default:
            break;
    }

    m_mode = ArchiveImpl::Closed;
}

#ifdef QT3
Q_ULONG ZipDevice::size() const
#else
qint64 ZipDevice::size() const
#endif
{
    if ( m_mode == ArchiveImpl::Decompress )
    {
        // If reading return the uncompressed size
        unz_file_info info_z;

        if (unzGetCurrentFileInfo(m_unzipFile, &info_z, NULL, 0, NULL, 0,
                NULL, 0) != UNZ_OK)
        {
#ifdef QT3
            return 0;
#else
            return -1;
#endif
        }

        //qDebug("size: %d", (int)info_z.uncompressed_size);

        return info_z.uncompressed_size;
    }

    // If writing return 0.
    return 0;
}


#ifdef QT3
Q_LONG ZipDevice::readBlock(char *data, Q_ULONG maxlen)
#else
qint64 ZipDevice::readData(char *data, qint64 maxlen)
#endif
{
    int nRead = unzReadCurrentFile(m_unzipFile, data, maxlen);
#ifdef QT3
    qDebug("readBlock: read %d %s", nRead, data);
#else
    qDebug() << "readData: read" << nRead << QByteArray( data, nRead );
#endif

    if (nRead < 0)
    {
        // Error
        qDebug("readBlock: file read failed! %d", nRead);
        return -1;
    }

    return nRead;
}


#ifdef QT3
Q_LONG ZipDevice::writeBlock(const char *data, Q_ULONG len)
#else
qint64 ZipDevice::writeData(const char *data, qint64 len)
#endif
{
#ifdef QT3
    qDebug("writeBlock: 0x%x %d", (uint)data, (int)len);
#else
    qDebug() << "writeData:" << data << len;
#endif

    int err = ZIP_OK;
    err = zipWriteInFileInZip(m_zipFile, data, len);

    if (err != ZIP_OK)
    {
        // Error
        qDebug("writeBlock: failed!");
        return -1;
    }

    m_nTotalWritten += len;

    return len;
}

#ifndef QT3
qint64 ZipDevice::pos() const
{
    qDebug("pos:");

    if ( m_mode == ArchiveImpl::Decompress )
    {
        return unztell(m_unzipFile);
    }
    else
    {
        return m_nTotalWritten;
    }
}
#endif


} // of namespace bugless
