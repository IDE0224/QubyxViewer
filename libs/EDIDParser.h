/* 
 * File:   EDIDParser.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 16 січня 2012, 14:26
 */

#ifndef EDIDPARSER_H
#define	EDIDPARSER_H

#include <QString>
#include <vector>

class EDIDParser 
{
public:
    struct Resolution
    {
        int x,y;
    };
private:
    unsigned char *EDID_;
    unsigned int EDIDsize_;
    QString OSId_;
    std::vector<int> descriptors_;
    int edidVer_;
    
    QString serial_, name_, manufacturer_;
    std::vector<Resolution> resolutions_, sizes_;
    
    void fillDescriptors();
        
    QString GetEDIDIntervalHex(unsigned startpos, unsigned endpos, bool reverted=false);
    
    QString cleanString(QString s);

public:
    EDIDParser(const unsigned char *EDID, unsigned int EDIDsize, QString OSId);
    virtual ~EDIDParser();

    QString serial();
    QString name();
    QString manufacturer();
    std::vector<Resolution> resolutions();
    std::vector<Resolution> realSizesMm();

    QString getShortManufacturer();
    QString getEDIDString(unsigned char code);


    
    static bool isSerialLooksFaked(QString serial, bool standardFakeOnly=false);
    static QString longManufacturer(QString shortManufacturer);
};

#endif	/* EDIDPARSER_H */

