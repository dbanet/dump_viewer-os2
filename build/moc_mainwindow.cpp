/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Sun Dec 1 06:25:23 2013
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      38,   11,   11,   11, 0x0a,
      62,   11,   11,   11, 0x0a,
      97,   11,   11,   11, 0x0a,
     125,   11,   11,   11, 0x0a,
     151,   11,   11,   11, 0x0a,
     186,  179,   11,   11, 0x0a,
     225,  218,   11,   11, 0x0a,
     257,   11,   11,   11, 0x0a,
     300,  296,   11,   11, 0x0a,
     323,  317,   11,   11, 0x08,
     349,   11,   11,   11, 0x08,
     364,   11,   11,   11, 0x08,
     381,  378,   11,   11, 0x08,
     410,  402,   11,   11, 0x08,
     434,   11,   11,   11, 0x08,
     443,   11,   11,   11, 0x08,
     485,   11,   11,   11, 0x08,
     522,   11,   11,   11, 0x08,
     556,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_searchButton_clicked()\0"
    "on_stopButton_clicked()\0"
    "on_patternLineEdit_returnPressed()\0"
    "on_unpackAction_triggered()\0"
    "on_exitAction_triggered()\0"
    "on_selectAction_triggered()\0base32\0"
    "on_base32Action_triggered(bool)\0cp1251\0"
    "on_cp1251Action_triggered(bool)\0"
    "on_selectDescriptionAction_triggered()\0"
    "url\0openUrl(QString)\0lines\0"
    "addLines(QStringList_ptr)\0startFilling()\0"
    "stopFilling()\0id\0showDescription(int)\0"
    "current\0rowChanged(QModelIndex)\0"
    "search()\0on_action_copy_rutracker_link_triggered()\0"
    "on_action_open_rutracker_triggered()\0"
    "on_action_copy_magnet_triggered()\0"
    "on_action_open_magnet_triggered()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_searchButton_clicked(); break;
        case 1: on_stopButton_clicked(); break;
        case 2: on_patternLineEdit_returnPressed(); break;
        case 3: on_unpackAction_triggered(); break;
        case 4: on_exitAction_triggered(); break;
        case 5: on_selectAction_triggered(); break;
        case 6: on_base32Action_triggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: on_cp1251Action_triggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: on_selectDescriptionAction_triggered(); break;
        case 9: openUrl((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: addLines((*reinterpret_cast< QStringList_ptr(*)>(_a[1]))); break;
        case 11: startFilling(); break;
        case 12: stopFilling(); break;
        case 13: showDescription((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: rowChanged((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        case 15: search(); break;
        case 16: on_action_copy_rutracker_link_triggered(); break;
        case 17: on_action_open_rutracker_triggered(); break;
        case 18: on_action_copy_magnet_triggered(); break;
        case 19: on_action_open_magnet_triggered(); break;
        default: ;
        }
        _id -= 20;
    }
    return _id;
}
static const uint qt_meta_data_SearchingThread[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   17,   16,   16, 0x05,
      49,   16,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_SearchingThread[] = {
    "SearchingThread\0\0lines\0newLines(QStringList_ptr)\0"
    "stopFilling()\0"
};

const QMetaObject SearchingThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SearchingThread,
      qt_meta_data_SearchingThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SearchingThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SearchingThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SearchingThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SearchingThread))
        return static_cast<void*>(const_cast< SearchingThread*>(this));
    if (!strcmp(_clname, "QRunnable"))
        return static_cast< QRunnable*>(const_cast< SearchingThread*>(this));
    return QObject::qt_metacast(_clname);
}

int SearchingThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newLines((*reinterpret_cast< QStringList_ptr(*)>(_a[1]))); break;
        case 1: stopFilling(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void SearchingThread::newLines(QStringList_ptr _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SearchingThread::stopFilling()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
