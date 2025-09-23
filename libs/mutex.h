#ifndef LIBS_QUBYX_FIX_MUTEX_H
#define LIBS_QUBYX_FIX_MUTEX_H

#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include "core.h"

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#include <QRecursiveMutex>
#endif

namespace qbx {
    class Mutex;
    class RecursiveMutex;
}

class qbx::Mutex : public QMutex
{
public:
    using QMutex::QMutex;
};

class qbx::RecursiveMutex : public
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
                    QRecursiveMutex
#else
                    QMutex
#endif
{
public:
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    using QRecursiveMutex::QRecursiveMutex;
#else
    RecursiveMutex();
#endif
};

#endif // LIBS_QUBYX_FIX_MUTEX_H
