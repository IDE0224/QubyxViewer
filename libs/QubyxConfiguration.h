#ifndef QUBYXCONFIGURATION_H
#define	QUBYXCONFIGURATION_H

#include <QString>
#include <QtXml/QDomElement>
#include "QbxDataDef.h"

#include <vector>

class QDomDocument;

class QubyxConfiguration
{
    QDomDocument* doc_;
    bool opened_;

    static QString defaultpath_;
    void Init(QString path, QString configname, bool withoutext=true);

public:
    class iterator;

    static void SetDefaultPath(QString path);

    QubyxConfiguration(QString path, QString configname, bool withoutext);
    QubyxConfiguration(QString path, QString configname);
    QubyxConfiguration(QString configname);
    QubyxConfiguration();
    virtual ~QubyxConfiguration();
    bool IsOpened();

    void SetContent(QString xmlcontent);
    void CreateEmpty(QString basetag);
    QString GetContent();

    iterator GetStartElement();
    iterator firstchild(QString tagname);
    iterator firstchild();
};

class QubyxConfiguration::iterator
{
    QDomElement node_;
    QString tagname_;
    QDomDocument* doc_;
    iterator(QDomElement node, QDomDocument* doc);
    iterator(QDomElement node, QString tagname, QDomDocument* doc_);
public:
    iterator();
    ~iterator();
    QString name();
    QString value();
    void setValue(const QString &value);
    void setHTMLValue(const QString &value);
    QbxDataMap attributes();
    bool IsNull();

    iterator firstchild();
    iterator firstchild(QString tagname);

    void setattribute(QString name, QString value);
    void removeattribute(const QString &name);
    iterator addchild(QString tagname);
    iterator addchild(QString tagname, QString tagvalue);
    void removeChildsByName(const QString &name);
    void removeChilds(const QString &name, std::vector<std::pair<QString, QString> > &attributes);

    QString operator*();
    iterator& operator++();
    iterator operator++(int);

    friend class QubyxConfiguration;
};

#endif	/* _QUBYXCONFIGURATION_H */

