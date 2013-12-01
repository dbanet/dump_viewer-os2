
#pragma once

// qt
#include <qstring.h>

namespace bugless
{
//! Create intermediate directories of a file path
/*!
    \param path structure to be set up
*/
bool createPath( const QString& path );

}