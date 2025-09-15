/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include <QObject>
#include <QSettings>

class FileReader : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QString readFile(const QString& fileName);
    Q_INVOKABLE QString fileType(const QString& fileName);
    Q_INVOKABLE QStringList getListOfImageFiles(const QString& fileName);

    Q_INVOKABLE QString fragmentShader();

    Q_INVOKABLE QString lastProfilePath();
    Q_INVOKABLE QString lastProfilePathDir();
    Q_INVOKABLE void setLastProfilePath(QString profilePath);

    Q_INVOKABLE QString lastMediaPath();
    Q_INVOKABLE QString lastMediaPathDir();
    Q_INVOKABLE void setLastMediaPath(QString filePath);

private:
    QSettings settings_;
    QString fragmentShader_;
};

