#ifndef QBXDATADEF_H
#define	QBXDATADEF_H

#include <map>
#include <QString>

namespace QbxData
{
  enum DataTypes
  {
      DataTypeString = 0,
      DataTypeKeyInteger,
      DataTypeInteger,
      DataTypeDouble
  };
};

typedef std::map<QString, QString> QbxDataMap;
typedef std::map<QString, QbxData::DataTypes> QbxTypeMap;

#endif	/* _QBXDATADEF_H */

