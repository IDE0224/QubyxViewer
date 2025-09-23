#ifndef LIBS_QUBYX_FIX_REGEXP_H
#define LIBS_QUBYX_FIX_REGEXP_H

#include <QtCore/QDateTime>
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#include <QtCore/QRegularExpression>
#else
#include <QRegExp>
#endif

#include "core.h"

namespace qbx { class RegExp; }

class qbx::RegExp : public
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
                    QRegularExpression
#else
                    QRegExp
#endif
{
public:
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    using QRegularExpression::QRegularExpression;
#else
    // using QRegExp::QRegExp;
    explicit RegExp(const QString &pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive);
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    enum class PatternSyntax {
        RegExp = 0,
        FixedString
    };
#else
    using PatternSyntax = QRegExp::PatternSyntax;
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    RegExp(const QString& pattern, Qt::CaseSensitivity cs,
           PatternSyntax syntax = PatternSyntax::FixedString);
#else
    RegExp(const QString& pattern, Qt::CaseSensitivity cs, PatternSyntax syntax);
#endif


#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    bool indexIn(const QString& str);
    QString cap(int nth = 0) const;
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    operator QRegularExpression() const;
#else
    operator QRegExp() const;
#endif

private:
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    QRegularExpressionMatch match_;
#endif
};

#endif // LIBS_QUBYX_FIX_REGEXP_H
