#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include <QString>

namespace OSUtils
{

    bool setFullPermissions(QString path, bool folder=false);

    /**
     * @brief Get command line for opening file by extension
     * @param extension - file extension, without dot (for example "doc");
     * @return full command line path of associated application ("C:\Program Files (x86)\LibreOffice 4\program\\swriter.exe" -o "%1")
     */
    QString getAssociatedApplication(QString extension);

}
#endif // FILEHANDLING_H
