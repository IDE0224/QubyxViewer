#include "mutex.h"

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
qbx::RecursiveMutex::RecursiveMutex() : QMutex(QMutex::Recursive) { }
#endif
