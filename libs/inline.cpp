#include "inline.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QLineF::IntersectionType qbx::lineFIntersection(const QLineF& line1,
                                           const QLineF& line2,
                                           QPointF* pos){
    return line1.intersects(line2, pos);
#else
QLineF::IntersectType qbx::lineFIntersection(const QLineF& line1,
                                        const QLineF& line2,
                                        QPointF* pos){
    return line1.intersect(line2, pos);
#endif
}

std::list<QString> qbx::toStdList(const QStringList& qlist) {
    return std::list<QString>(qlist.begin(), qlist.end());
}

QStringList qbx::fromStdList(const std::list<QString>& list)
{
    QStringList result;
    result.reserve(static_cast<int>(list.size()));
    for (const auto& s : list)
        result.append(s);
    return result;
}
