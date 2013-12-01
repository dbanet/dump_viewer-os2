/****************************************************************************
** Meta object code from reading C++ file 'Archive.h'
**
** Created: Sun Dec 1 06:25:26 2013
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../bugless/Archive.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Archive.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_bugless__Archive[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, flags, count, data
      17, 0x0,    3,   18,

 // enum data: key, value
      22, uint(bugless::Archive::NONE),
      27, uint(bugless::Archive::ZIP),
      31, uint(bugless::Archive::TAR),

       0        // eod
};

static const char qt_meta_stringdata_bugless__Archive[] = {
    "bugless::Archive\0Type\0NONE\0ZIP\0TAR\0"
};

const QMetaObject bugless::Archive::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_bugless__Archive,
      qt_meta_data_bugless__Archive, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &bugless::Archive::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *bugless::Archive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *bugless::Archive::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_bugless__Archive))
        return static_cast<void*>(const_cast< Archive*>(this));
    return QObject::qt_metacast(_clname);
}

int bugless::Archive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
