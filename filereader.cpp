/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "filereader.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMimeDatabase>
#include <QDebug>

const QString LAST_PROFILE_PATH = "paths/profile";
const QString LAST_MEDIAFILE_PATH = "paths/mediafile";

QString FileReader::readFile(const QString& fileName)
{
    QString content;
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        content = stream.readAll();
    }

    return content;
}

QString FileReader::fileType(const QString& fileName)
{
    QString fName = fileName;
    fName = fName.replace("file:///", "");

    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(fName);

    qDebug() << "mime type:" << mime.name();

    if (mime.name().contains("video", Qt::CaseInsensitive))
        return "video";

    else if (mime.name().contains("image", Qt::CaseInsensitive))
        return "image";

    else
        return "unknown";
}

QStringList FileReader::getListOfImageFiles(const QString& fileName)
{
    qDebug() << "getListOfImageFiles" << fileName;

    QString fName = fileName;
    fName = fName.replace("file:///", "");

    QDir dir(fName);

    qDebug() << "absolute path" << dir.absolutePath();
    dir.cdUp();
    qDebug() << "absolute path2" << dir.absolutePath();

    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);

    QStringList filters;
    filters << "*.png" << "*.tif" << "*.tiff" << "*.bmp" << "*.jpg" << "*.jpeg";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    QStringList res;

    for (int i = 0; i < list.size(); i++)
    {
        QFileInfo fileInfo = list.at(i);
        res.push_back(fileInfo.absoluteFilePath());
    }

    // place selected image file at begin on list
    for (int i = 0; i < res.size(); i++)
    {
        if (!res.first().compare(fName, Qt::CaseInsensitive))
            break;

        res.push_back(res.first());
        res.pop_front();
    }

    for (int i = 0; i < res.size(); i++)
        res[i] = "file:///" + res[i];

    for (int i = 0; i < res.size(); i++)
        qDebug() << res[i];

    return res;
}

QString FileReader::fragmentShader()
{
    if (fragmentShader_.isEmpty())
        fragmentShader_ = readFile(":qml/3dlut.frag");

    return fragmentShader_;
}

QString FileReader::lastProfilePath()
{
    if (settings_.contains(LAST_PROFILE_PATH))
        return settings_.value(LAST_PROFILE_PATH).toString();

    else
        return "";
}

QString FileReader::lastProfilePathDir()
{
    QString path = lastProfilePath();
    QDir dir(path);
    dir.cdUp();

    qDebug() << "profile path" << dir.absolutePath();

    return dir.absolutePath();
}

void FileReader::setLastProfilePath(QString profilePath)
{
    QString path = profilePath;
    path = path.replace("file:///", "");

    settings_.setValue(LAST_PROFILE_PATH, path);
}

QString FileReader::lastMediaPath()
{
    if (settings_.contains(LAST_MEDIAFILE_PATH))
        return settings_.value(LAST_MEDIAFILE_PATH).toString();

    else
        return "";
}

QString FileReader::lastMediaPathDir()
{
    QString path = lastMediaPath();
    QDir dir(path);
    dir.cdUp();

    qDebug() << "media path" << dir.absolutePath();

    return dir.absolutePath();
}

void FileReader::setLastMediaPath(QString filePath)
{
    QString path = filePath;
    path = path.replace("file:///", "");

    settings_.setValue(LAST_MEDIAFILE_PATH, path);
}
