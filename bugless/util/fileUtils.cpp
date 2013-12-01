#include <qglobal.h>

#ifdef Q_OS_UNIX
#include <unistd.h>
#include <utime.h>
#endif
#ifdef Q_OS_WIN32
#define USEWIN32IOAPI
#include <windows.h>
#endif // Q_OS_WIN32

#include "util/fileUtils.h"


#ifdef Q_OS_WIN32

QDateTime fileTimeToQDateTime(const FILETIME* time)
{
    QDateTime ret;
    SYSTEMTIME sTime, lTime;
    FileTimeToSystemTime(time, &sTime);
    SystemTimeToTzSpecificLocalTime(0 ,&sTime, &lTime);
    ret.setDate(QDate(lTime.wYear, lTime.wMonth, lTime.wDay));
    ret.setTime(QTime(lTime.wHour, lTime.wMinute, lTime.wSecond, lTime.wMilliseconds));
    return ret;
}

SYSTEMTIME QDateTimeToSystemTime( const QDateTime& datetime )
{
    SYSTEMTIME systime;
    systime.wYear         = datetime.date().year();
    systime.wMonth        = datetime.date().month();
    systime.wDayOfWeek    = datetime.date().dayOfWeek();
    systime.wDay          = datetime.date().day();
    systime.wHour         = datetime.time().hour();
    systime.wMinute       = datetime.time().minute();
    systime.wSecond       = datetime.time().second();
    systime.wMilliseconds = datetime.time().msec();;

    return systime;
}

FILETIME QDateTimeToFileTime( const QDateTime& datetime )
{
    // create a systemtime struct initially - they're easier
    // to deal with than FILETIME
    SYSTEMTIME systime = QDateTimeToSystemTime( datetime );

    FILETIME ftm, local;
    SystemTimeToFileTime( &systime, &local );
    LocalFileTimeToFileTime( &local, &ftm );

    return ftm;
}

#endif // Q_OS_WIN32

#ifdef Q_OS_UNIX
struct utimbuf QDateTimeToUt( const QDateTime& datetime )
{
    struct utimbuf ut;
    struct tm newdate;

    newdate.tm_sec  = datetime.time().second();
    newdate.tm_min  = datetime.time().minute();
    newdate.tm_hour = datetime.time().hour();
    newdate.tm_mday = datetime.date().day();
    newdate.tm_mon  = datetime.date().month();
    newdate.tm_year = datetime.date().year()-1900;

    newdate.tm_isdst = -1;

    ut.actime = ut.modtime = mktime(&newdate);

    return ut;
}
#endif // Q_OS_UNIX

namespace bugless
{

bool setFileTime( const QString& fileName, const QDateTime& datetime )
{
    // Set the last modification time of the output file
#ifdef Q_OS_WIN32

#ifdef UNICODE
#ifdef QT3
    LPCTSTR fName = (LPCTSTR)fileName.ucs2();
#else
    LPCTSTR fName = (LPCTSTR)fileName.utf16();
#endif
#else
    LPCTSTR fName = (LPCTSTR)fileName.local8Bit();
#endif

    HANDLE hFile = CreateFile(fName, GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        FILETIME ftCreate, ftLastAcc, ftLastWrite;
        GetFileTime(hFile,&ftCreate, &ftLastAcc, &ftLastWrite);

        //DosDateTimeToFileTime((WORD)(dosdate>>16), (WORD)dosdate, &ftLocal);
        //LocalFileTimeToFileTime(&ftLocal, &ftm);
        const FILETIME ftm = QDateTimeToFileTime( datetime );

        SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);
        CloseHandle(hFile);
    }
    else
    {
        // If we fail then fail silently as not getting the correct
        // timestamp is not the end of the world.
    }
#else // Q_OS_WIN32
#ifdef Q_OS_UNIX
    struct utimbuf ut = QDateTimeToUt( datetime );
    struct tm newdate;

    newdate.tm_sec  = datetime.time().second();
    newdate.tm_min  = datetime.time().minute();
    newdate.tm_hour = datetime.time().hour();
    newdate.tm_mday = datetime.date().day();
    newdate.tm_mon  = datetime.date().month();
    newdate.tm_year = datetime.date().year()-1900;

    newdate.tm_isdst = -1;

    ut.actime = ut.modtime = mktime(&newdate);
#ifdef QT3
    utime(fileName.ascii(), &ut);
#else
    utime(fileName.toAscii().constData(), &ut);
#endif
#endif // Q_OS_UNIX
#endif // Q_OS_WIN32

    return true;
}

} // of namespace bugless