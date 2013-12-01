#include <qdir.h>

#include "util/dirUtils.h"


namespace bugless
{

// recursive helper for create path
bool _createPath( const QString& path )
{
    if (path.isEmpty())
        return true;

    QDir dir(path);
    if ( dir.exists() )
    {
        return true;
    }

    const QChar separator('/');
    const QString curDir = path.section( separator, 0, 0 );
    const QString restOfPath = path.section( separator, 1 );

    QDir here;
    if ( !here.mkdir(curDir) )
    {
        // Failed to create directory
        return false;
    }

    // Recurse to create the rest of the path
    if ( !QDir::setCurrent(curDir) )
    {
        return false;
    }
    return _createPath(restOfPath);
}

bool createPath( const QString& path )
{

#ifdef QT3
    const QString cwd = QDir().currentDirPath();
    bool result = _createPath( path );
#else
    const QString cwd = QDir().currentPath();
    bool result = _createPath( path );
#endif

    // restore the cwd (it will have been modified by createPath)
    QDir::setCurrent(cwd);

    return result;
}


} // of namespace bugless