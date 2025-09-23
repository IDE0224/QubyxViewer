#ifndef LIBS_QUBYX_FIX_CORE_H
#define LIBS_QUBYX_FIX_CORE_H

#include <QtCore/QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#include <QStringView>
#else
#include <QStringRef>
#endif

#include "inline.h"

namespace qbx {
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    using StringView = QStringView;
#else
    using StringView = QStringRef;
#endif

    int rand(int v = std::numeric_limits <int>::max());
};

#endif // LIBS_QUBYX_FIX_CORE_H
