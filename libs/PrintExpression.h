#ifndef QBXDEBUG_PRINTEXPRESSION_H
#define QBXDEBUG_PRINTEXPRESSION_H


template <class DataType>
void __print_watch_expression_header(const DataType *expression);

template <class DataType>
std::vector<DataType> __print_expression(const std::string &indent, const std::string &container_name, std::vector<DataType> container);

template <class DataType>
std::list<DataType> __print_expression(const std::string &indent, const std::string &container_name, std::list<DataType> container);

template <class DataType>
std::set<DataType> __print_expression(const std::string &indent, const std::string &container_name, std::set<DataType> container);

template <class KeyType, class DataType>
std::map<KeyType, DataType> __print_expression(const std::string &indent, const std::string &container_name, std::map<KeyType, DataType> container);

template <class KeyType, class DataType>
std::multimap<KeyType, DataType> __print_expression(const std::string &indent, const std::string &container_name, std::multimap<KeyType, DataType> container);

template <class DataType>
DataType __print_expression(const std::string &indent, const std::string &unit_name, DataType container);

template <class DataType>
std::string __print_expression(const std::string &indent, const std::string &unit_name, std::string container);

template <class DataType>
QString __print_expression(const std::string &indent, const std::string &unit_name, QString container);


template <class DataType>
void __print_watch_expression_header(const DataType *expression)
{
    qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<"Expression: "<<expression;
}

template <class DataType>
std::vector<DataType> __print_expression(const std::string &indent, const std::string &container_name, std::vector<DataType> container)
{
    std::stringstream strm;
    std::string current_unit_name;
    unsigned int current_index;


    if (container_name != "")
        qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<QBXDBG_CONTINUME_MESSAGE_SIGNATURE<<indent.c_str()<<container_name.c_str()<<"=";

    current_index = 0;
    for (typename std::vector<DataType>::iterator current_unit = container.begin(); current_unit!=container.end(); ++current_unit)
    {
        strm<<"["<<current_index<<"]";
        strm>>current_unit_name;
        strm.clear();
        __print_expression(indent+QBXDBG_INDENT, current_unit_name, *current_unit);
        ++current_index;
    }

    return container;
}

template <class DataType>
std::list<DataType> __print_expression(const std::string &indent, const std::string &container_name, std::list<DataType> container)
{
    std::stringstream strm;
    std::string current_unit_name;
    unsigned int current_index;


    if (container_name != "")
        qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<QBXDBG_CONTINUME_MESSAGE_SIGNATURE<<indent.c_str()<<container_name.c_str()<<"=";

    current_index = 0;
    for (typename std::list<DataType>::iterator current_unit = container.begin(); current_unit!=container.end(); ++current_unit)
    {
        strm<<"["<<current_index<<"]";
        strm>>current_unit_name;
        strm.clear();
        __print_expression(indent+QBXDBG_INDENT, current_unit_name, *current_unit);
        ++current_index;
    }

    return container;
}

template <class DataType>
std::set<DataType> __print_expression(const std::string &indent, const std::string &container_name, std::set<DataType> container)
{
    std::stringstream strm;
    std::string current_unit_name;
    unsigned int current_index;


    if (container_name != "")
        qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<QBXDBG_CONTINUME_MESSAGE_SIGNATURE<<indent.c_str()<<container_name.c_str()<<"=";

    current_index = 0;
    for (typename std::set<DataType>::iterator current_unit = container.begin(); current_unit!=container.end(); ++current_unit)
    {
        strm<<"["<<current_index<<"]";
        strm>>current_unit_name;
        strm.clear();
        __print_expression(indent+QBXDBG_INDENT, current_unit_name, *const_cast<DataType*>(&*current_unit));
        ++current_index;
    }

    return container;
}

template <class KeyType, class DataType>
std::map<KeyType, DataType> __print_expression(const std::string &indent, const std::string &container_name, std::map<KeyType, DataType> container)
{
    //std::stringstream strm;
    QString label;
    std::string current_unit_name;


    if (container_name != "")
        qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<QBXDBG_CONTINUME_MESSAGE_SIGNATURE<<indent.c_str()<<container_name.c_str()<<"=";

    for (typename std::map<KeyType, DataType>::iterator current_unit = container.begin(); current_unit!=container.end(); ++current_unit)
    {
        //strm<<"["<<current_unit->first<<"]";
        //strm>>current_unit_name;
        label = label+"["+current_unit->first+"]";
        //strm.clear();
        __print_expression(indent+QBXDBG_INDENT, label.toStdString(), current_unit->second);
        label.clear();
    }

    return container;
}

template <class KeyType, class DataType>
std::multimap<KeyType, DataType> __print_expression(const std::string &indent, const std::string &container_name, std::multimap<KeyType, DataType> container)
{
    std::stringstream strm;
    std::string current_unit_name;


    if (container_name != "")
        qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<QBXDBG_CONTINUME_MESSAGE_SIGNATURE<<indent.c_str()<<container_name.c_str()<<"=";

    for (typename std::multimap<KeyType, DataType>::iterator current_unit = container.begin(); current_unit!=container.end(); ++current_unit)
    {
        strm<<"["<<current_unit->first<<"]";
        strm>>current_unit_name;
        strm.clear();
        __print_expression(indent+QBXDBG_INDENT, current_unit_name, current_unit->second);
    }

    return container;
}

template <class DataType>
DataType __print_expression(const std::string &indent, const std::string &unit_name, DataType container)
{
    qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<QBXDBG_CONTINUME_MESSAGE_SIGNATURE<<indent.c_str()<<unit_name.c_str()<<"="<<container;
    return container;
}

template <class DataType>
std::string __print_expression(const std::string &indent, const std::string &unit_name, std::string container)
{
    qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<QBXDBG_CONTINUME_MESSAGE_SIGNATURE<<indent.c_str()<<unit_name.c_str()<<"="<<QString(container.c_str());
    return container;
}

template <class DataType>
QString __print_expression(const std::string &indent, const std::string &unit_name, QString container)
{
    qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<QBXDBG_CONTINUME_MESSAGE_SIGNATURE<<indent.c_str()<<unit_name.c_str()<<"="<<container;
    return container;
}


#endif
