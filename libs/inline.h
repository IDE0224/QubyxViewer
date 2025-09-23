#ifndef LIBS_QUBYX_FIX_INLINE_H
#define LIBS_QUBYX_FIX_INLINE_H

#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QtCore/QLineF>

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#include <QRecursiveMutex>
#endif

namespace qbx {
    std::list <QString> toStdList(const QStringList& qlist);
    QStringList fromStdList(const std::list<QString>& list);

    template <typename T>
    QVector <T> fromStdVector(const std::vector <T>& v) {
    #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        return QVector <T>(v.begin(), v.end());
    #else
        QVector <T> result;
        result.reserve (static_cast <int> (v.size()));
        for (const T& item : v)
            result.append(item);
        return result;
    #endif
    }

    template <typename T>
    QSet <T> to_set(QList <T> list){
        QSet <T> ret;
        for (const auto& it : list)
            ret.insert(it);

        return ret;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QLineF::IntersectionType lineFIntersection(const QLineF& line1,
                                               const QLineF& line2,
                                               QPointF* pos);
#else
    QLineF::IntersectType lineFIntersection(const QLineF& line1,
                                            const QLineF& line2,
                                            QPointF* pos);
#endif
}

#endif // LIBS_QUBYX_FIX_INLINE_H
