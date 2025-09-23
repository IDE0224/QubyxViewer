#include "QubyxConfiguration.h"

#include <QDomDocument>
#include <QFile>

#include <QDebug>

QString QubyxConfiguration::defaultpath_="";

QubyxConfiguration::QubyxConfiguration(QString path, QString configname, bool withoutext)
{
    Init(path, configname, withoutext);
}

QubyxConfiguration::QubyxConfiguration(QString path, QString configname)
{
    Init(path, configname);
}

QubyxConfiguration::QubyxConfiguration(QString configname)
{
    Init(defaultpath_, configname);
}

QubyxConfiguration::QubyxConfiguration()
{
    opened_ = false;
    doc_ = nullptr;
}

QubyxConfiguration::~QubyxConfiguration()
{
    if (doc_)
        delete doc_;
}

void QubyxConfiguration::SetDefaultPath(QString path)
{
    defaultpath_ = path;
}

void QubyxConfiguration::Init(QString path, QString configname, bool withoutext)
{
    Q_UNUSED(withoutext)
    QString fullpath = path;
    if (fullpath!="" && !fullpath.endsWith('/'))
        fullpath += "/";
    fullpath +=  configname;// + ".xml";/
      
    doc_ = new QDomDocument(configname);

    QFile file(fullpath);
    opened_ = false;
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (doc_->setContent(&file))
    {
        opened_ = true;
    }
    file.close();  
}

void QubyxConfiguration::SetContent(QString xmlcontent)
{
    if (doc_)
        delete doc_;
    
     while(true)
    {
        int find = xmlcontent.indexOf("<",0);
        
        if (xmlcontent[find+1] == '!' || xmlcontent[find+1] == '?')
        {
            xmlcontent = xmlcontent.remove(find, xmlcontent.indexOf(">",find+1) - find+1);
        }
        else
            break;
    }

    doc_ = new QDomDocument("dynamic");
    doc_->setContent(xmlcontent);

    opened_ = true;
}

void QubyxConfiguration::CreateEmpty(QString basetag)
{
    SetContent(QString("<")+basetag+"></"+basetag+">");
}

QString QubyxConfiguration::GetContent()
{
    if (!opened_ || !doc_)
        return "";

    QString content = doc_->toString(-1);
    if (!content.contains("<?xml", Qt::CaseInsensitive))
        content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + content;

    return content;
}

bool QubyxConfiguration::IsOpened()
{
    return opened_;
}

QubyxConfiguration::iterator QubyxConfiguration::GetStartElement()
{
    if (doc_)
        return iterator(doc_->documentElement(), doc_);

    return iterator();
}

QubyxConfiguration::iterator QubyxConfiguration::firstchild(QString tagname)
{
    if (doc_)
        return GetStartElement().firstchild(tagname);

    return iterator();
}

QubyxConfiguration::iterator QubyxConfiguration::firstchild()
{
    return firstchild("");
}

QubyxConfiguration::iterator::iterator(QDomElement node, QDomDocument* doc) : node_(node), tagname_(""), doc_(doc)
{
}

QubyxConfiguration::iterator::iterator(QDomElement node, QString tagname, QDomDocument* doc) : node_(node), tagname_(tagname), doc_(doc)
{
}

QubyxConfiguration::iterator::iterator() : node_(), tagname_(""), doc_(nullptr)
{
}


QubyxConfiguration::iterator::~iterator()
{

}

QString QubyxConfiguration::iterator::name()
{
    return IsNull() ? "" : node_.nodeName();
}

QString QubyxConfiguration::iterator::value()
{
    return IsNull() ? "" :  node_.text().trimmed();
}

void QubyxConfiguration::iterator::setValue(const QString& value)
{
    QDomNodeList list = node_.childNodes();
    for (int i=0;i<list.count();++i)
        if (list.item(i).isText())
        {
            list.item(i).toText().setNodeValue(value);
            return;
        }
    
    QDomNode el = node_.appendChild(doc_->createTextNode(value));
}

void QubyxConfiguration::iterator::setHTMLValue(const QString &value)
{
    QDomImplementation::InvalidDataPolicy defpolicy =  doc_->implementation().invalidDataPolicy();
    doc_->implementation().setInvalidDataPolicy(QDomImplementation::AcceptInvalidChars);
    
    setValue(value);
   /* QDomCharacterData chdata;
    chdata.setData(value);
    node_.appendChild(chdata);*/
    
    doc_->implementation().setInvalidDataPolicy(defpolicy);
}

QbxDataMap QubyxConfiguration::iterator::attributes()
{
    if (!IsNull())
    {
        QDomNamedNodeMap m = node_.attributes();
        QDomAttr attr;
        QbxDataMap map;
        for (unsigned i = 0; i < uint(m.length()); i++)
        {
            attr = m.item(int(i)).toAttr();
            if (!attr.isNull())
                map[attr.name()] = attr.value();
        }
        return map;
    }
    return QbxDataMap();
}

bool QubyxConfiguration::iterator::IsNull()
{
    return node_.isNull();
}

QubyxConfiguration::iterator QubyxConfiguration::iterator::firstchild()
{
    return firstchild("");
}

QubyxConfiguration::iterator QubyxConfiguration::iterator::firstchild(QString tagname)
{
    return iterator(node_.firstChildElement(tagname), tagname, doc_);
}

void QubyxConfiguration::iterator::setattribute(QString name, QString value)
{
    node_.setAttribute(name, value);
}

void QubyxConfiguration::iterator::removeattribute(const QString& name)
{
    node_.removeAttribute(name);
}

QubyxConfiguration::iterator QubyxConfiguration::iterator::addchild(QString tagname)
{
    if (doc_)
        return iterator(node_.appendChild(doc_->createElement(tagname)).toElement(), tagname, doc_);

    return iterator();
}

QubyxConfiguration::iterator QubyxConfiguration::iterator::addchild(QString tagname, QString tagvalue)
{
    if (doc_)
    {
        QDomElement el = doc_->createElement(tagname);
        el.appendChild(doc_->createTextNode(tagvalue));
        return iterator(node_.appendChild(el).toElement(), tagname, doc_);
    }

    return iterator();
}

void QubyxConfiguration::iterator::removeChildsByName(const QString &name)
{
    if (doc_)
    {
        QDomNodeList list = node_.childNodes();
        for (int i=0;i<list.count();++i)
            if (list.item(i).nodeName()==name)
            {
                node_.removeChild(list.item(i));
                i--;
            }
    }
}

void QubyxConfiguration::iterator::removeChilds(const QString &name, std::vector<std::pair<QString, QString> > &attributes)
{
    if (doc_)
    {
        QDomNodeList list = node_.childNodes();
        for (int i=0;i<list.count();++i)
            if (list.item(i).nodeName()==name)
            {
                bool res = true;
                
                for (std::vector<std::pair<QString, QString> >::iterator attIt = attributes.begin(); attIt != attributes.end(); ++attIt)
                        if (list.item(i).attributes().namedItem(attIt->first).nodeValue() != attIt->second)
                        {
                            res = false;
                            break;
                        }
                    
                if (res)
                {
                    node_.removeChild(list.item(i));
                    i--;
                }
            }
    }
}

QString QubyxConfiguration::iterator::operator*()
{
    if (IsNull())
        return "";
    return this->value();
}

QubyxConfiguration::iterator& QubyxConfiguration::iterator::operator++()
{
    if (IsNull())
        return *this;
    node_ = node_.nextSiblingElement(tagname_);
    return *this;
}

QubyxConfiguration::iterator QubyxConfiguration::iterator::operator++(int s)
{
    Q_UNUSED(s)
    if (IsNull())
        return *this;
    iterator it = *this;
    node_ = node_.nextSiblingElement(tagname_);
    return it;
}



