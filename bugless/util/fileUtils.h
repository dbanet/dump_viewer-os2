
#pragma once

// qt
#include <qdatetime.h>
#include <qstring.h>

namespace bugless
{
//! Set the creation/access time of a file.
/*!
    \param fileName Name of file 
    \param datetime QDateTime of new time
*/
bool setFileTime( const QString& fileName, const QDateTime& datetime );

}