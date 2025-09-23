#include "reg_exp.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
qbx::RegExp::RegExp(const QString& pattern, Qt::CaseSensitivity cs)
    : QRegExp(pattern, cs)
{
    //
}
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
qbx::RegExp::RegExp(const QString& pattern, Qt::CaseSensitivity cs, PatternSyntax syntax)
    : QRegularExpression((syntax == PatternSyntax::FixedString)
                             ? QRegularExpression::escape(pattern)
                             : pattern)
{
    setPatternOptions(cs == Qt::CaseInsensitive
                          ? QRegularExpression::CaseInsensitiveOption
                          : QRegularExpression::NoPatternOption);
}
#else
qbx::RegExp::RegExp(const QString& pattern, Qt::CaseSensitivity cs, PatternSyntax syntax)
    : QRegExp(pattern, cs, syntax)
{}
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
bool qbx::RegExp::indexIn(const QString& str)
{
    match_ = this->match(str);
    return match_.hasMatch();
}

QString qbx::RegExp::cap(int nth) const
{
    if (match_.hasMatch())
        return match_.captured(nth);
    return QString();
}
#endif


#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
qbx::RegExp::operator QRegularExpression() const { return *this; }
#else
qbx::RegExp::operator QRegExp() const { return *this; }
#endif
