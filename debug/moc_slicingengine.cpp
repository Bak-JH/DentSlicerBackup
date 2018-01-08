/****************************************************************************
** Meta object code from reading C++ file 'slicingengine.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../slicingengine.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'slicingengine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SlicingEngine_t {
    QByteArrayData data[15];
    char stringdata0[174];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SlicingEngine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SlicingEngine_t qt_meta_stringdata_SlicingEngine = {
    {
QT_MOC_LITERAL(0, 0, 13), // "SlicingEngine"
QT_MOC_LITERAL(1, 14, 15), // "updateModelInfo"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 13), // "printing_time"
QT_MOC_LITERAL(4, 45, 5), // "layer"
QT_MOC_LITERAL(5, 51, 3), // "xyz"
QT_MOC_LITERAL(6, 55, 6), // "volume"
QT_MOC_LITERAL(7, 62, 14), // "slicingStarted"
QT_MOC_LITERAL(8, 77, 13), // "slicingOutput"
QT_MOC_LITERAL(9, 91, 15), // "slicingFinished"
QT_MOC_LITERAL(10, 107, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(11, 128, 12), // "slicingError"
QT_MOC_LITERAL(12, 141, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(13, 164, 5), // "slice"
QT_MOC_LITERAL(14, 170, 3) // "cfg"

    },
    "SlicingEngine\0updateModelInfo\0\0"
    "printing_time\0layer\0xyz\0volume\0"
    "slicingStarted\0slicingOutput\0"
    "slicingFinished\0QProcess::ExitStatus\0"
    "slicingError\0QProcess::ProcessError\0"
    "slice\0cfg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SlicingEngine[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   53,    2, 0x0a /* Public */,
       8,    0,   54,    2, 0x0a /* Public */,
       9,    2,   55,    2, 0x0a /* Public */,
      11,    1,   60,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
      13,    1,   63,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Float,    3,    4,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 10,    2,    2,
    QMetaType::Void, 0x80000000 | 12,    2,

 // methods: parameters
    QMetaType::Void, QMetaType::QVariantMap,   14,

       0        // eod
};

void SlicingEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SlicingEngine *_t = static_cast<SlicingEngine *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateModelInfo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4]))); break;
        case 1: _t->slicingStarted(); break;
        case 2: _t->slicingOutput(); break;
        case 3: _t->slicingFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 4: _t->slicingError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 5: _t->slice((*reinterpret_cast< QVariantMap(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SlicingEngine::*_t)(int , int , QString , float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SlicingEngine::updateModelInfo)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject SlicingEngine::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SlicingEngine.data,
      qt_meta_data_SlicingEngine,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SlicingEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SlicingEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SlicingEngine.stringdata0))
        return static_cast<void*>(const_cast< SlicingEngine*>(this));
    return QObject::qt_metacast(_clname);
}

int SlicingEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SlicingEngine::updateModelInfo(int _t1, int _t2, QString _t3, float _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
