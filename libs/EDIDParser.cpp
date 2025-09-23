/* 
 * File:   EDIDParser.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 16 січня 2012, 14:26
 */

#include <string.h>
#include <algorithm>

#include "EDIDParser.h"

#include <QDebug>
#include "reg_exp.h"

#include "QubyxDebug.h"

EDIDParser::EDIDParser(const unsigned char *EDID, unsigned int EDIDsize, QString OSId) 
                : EDIDsize_(EDIDsize), OSId_(OSId), serial_(""), name_("")
{
    try
    {
        if (EDIDsize)
        {
            EDID_ = new unsigned char[EDIDsize];
            memcpy(EDID_, EDID, EDIDsize*sizeof(unsigned char));
            fillDescriptors();
            edidVer_ = 0;
            if (EDIDsize_>19)
                edidVer_ = EDID_[18]*10+EDID_[19];
//            qDebug() << "EDID version:" << edidVer_/10.;

//            qDebug() << "EDIDsize" << EDIDsize;
//            qDebug() << "edid:";
            $D(EDID_, EDIDsize);
        }
        else
            EDID_ = nullptr;
    }
    catch(...)
    {
        EDID_ = nullptr;
    }
}

EDIDParser::~EDIDParser()
{
    if (EDID_)
        delete[] EDID_;
}

void EDIDParser::fillDescriptors()
{
    qDebug() << "EDIDParser::fillDescriptors()";
    descriptors_.clear();
    
    if (EDIDsize_ > 54)
        for (unsigned i = 54; i < 126 && i + 18 <= EDIDsize_; i += 18)
        {
            descriptors_.push_back(int(i));
//            qDebug() << "descriptor" << i;
        }
}

QString EDIDParser::getEDIDString(unsigned char code)
{    
    QString res = "";
    for (auto itr = descriptors_.begin(); itr != descriptors_.end(); ++itr){
        if (EDID_[*itr] == 0x00 && EDID_[*itr + 1] == 0x00 && EDID_[*itr + 2] == 0x00
           && EDID_[*itr + 3] == code && EDID_[*itr + 4] == 0x00) {
            for (unsigned int j = 0; j < 13 && EDID_[uint(*itr) + 5 + j]; j++){
                QChar c = QChar(EDID_[uint(*itr) + 5 + j]);
//                if (not c.isDigit() && not c.isLetter())
//                    return "";
                res += c;
            }
            break;
        }
    }

    for (auto itr = descriptors_.begin(); itr != descriptors_.end(); ++itr){
        QString t = "";
        for (unsigned int j = 0; j < 13 && EDID_[uint(*itr) + 5 + j]; j++)
            t += QChar(EDID_[uint(*itr) + 5 + j]);
    }

    return res.trimmed();
}

QString EDIDParser::GetEDIDIntervalHex(unsigned startpos, unsigned endpos, bool reverted/*=false*/)
{
     QString res = "";
     std::vector <unsigned char> v;
     for (unsigned i = startpos; i < endpos && i < EDIDsize_; ++i)
         v.push_back(EDID_[i]);
     if (reverted)
         std::reverse(v.begin(), v.end());

     for (auto ch : v)
         res += QString(ch < 16 ? "0" : "") + QString::number(ch, 16);

     return res;
}

QString EDIDParser::getShortManufacturer()
{
    if (EDIDsize_ < 10)
        return "";

    char mf[4];
    mf[0] = (EDID_[8] >> 2) & 0x1F;
    mf[1] = char((EDID_[8] & 0x3) << 3) + (EDID_[9] >> 5);
    mf[2] = EDID_[9] & 0x1F;
    mf[3] = 0;

    for (int i = 0; i < 3; i++)
        mf[i] += 'A'-1;

    return QString::fromStdString(mf);
}

QString EDIDParser::serial()
{
    if (!EDID_)
        return OSId_;
       
    if (serial_.length())
        return serial_;
    
    if (EDIDsize_>64){
        qDebug() << "EDID serial";
        serial_ = getEDIDString(0xFF);
        qDebug() << "EDID serial 1" << serial_;

        if (serial_ == "" && getShortManufacturer() == "BDS"){
            serial_ = getEDIDString(0x0F);
            qDebug() << "Barco serial" << serial_;
        }

        if (serial_ == "" && EDIDsize_ > 128){
            // try read from another block
            QString serialParse("");
            for (uint i = 128; i < EDIDsize_ - 18; i++){
                if (EDID_[i] == 0x00 && EDID_[i + 1] == 0x00 && EDID_[i + 2] == 0x00 &&
                    EDID_[i + 3] == 0xFF && EDID_[i + 4] == 0x00) {
                    for (unsigned int j = 0; j < 13 && EDID_[uint(i) + 5 + j]; j++)
                        serialParse += QChar(EDID_[uint(i) + 5 + j]);
                    break;
                }
            }
            serial_ = serialParse;
            qDebug() << "EDID serial from block 1 and 2" << serialParse;
        }

        if (serial_ == ""){
            serial_ = GetEDIDIntervalHex(12, 16, true);
            qDebug() << "EDID serial 2" << serial_;
        }

        if (isSerialLooksFaked(serial_)){
            serial_ = getEDIDString(0xFE);
            qDebug() << "EDID serial 3" << serial_;
        }

        if (serial_ == ""){
            serial_ = GetEDIDIntervalHex(12, 16, true);
            qDebug() << "EDID serial 4" << serial_;
        }

        if (isSerialLooksFaked(serial_, true))
            serial_ = OSId_;

        if (serial_.contains("/"))
            serial_ = serial_.right(serial_.length()-serial_.lastIndexOf("/")-1);
        if (serial_.contains("\\"))
            serial_ = serial_.right(serial_.length()-serial_.lastIndexOf("\\")-1);
    }
    else {
        serial_ = OSId_;

        if (EDIDsize_ > 16){
            serial_ = GetEDIDIntervalHex(12, 16, true);
            if (isSerialLooksFaked(serial_))
                serial_ = OSId_;
        }

        if (serial_.contains("/"))
            serial_ = serial_.right(serial_.length() - serial_.lastIndexOf("/")-1);
        if (serial_.contains("\\"))
            serial_ = serial_.right(serial_.length() - serial_.lastIndexOf("\\")-1);
    }

    qDebug() << "EdidParser.cpp Serial edid read: " << serial_;
    return cleanString(serial_);
}

QString EDIDParser::name()
{
    if (!EDID_)
        return (OSId_.replace(".","").replace("\\","").replace("/",""));
       
    if (name_.length())
        return name_;
    
    if (EDIDsize_>64)
    {

        name_ = getEDIDString(0xFC);

        qDebug() << "EDID name 1" << name_;

        if (name_=="")
            name_ = getEDIDString(0xFE);
    }

    if (name_=="")
        name_ = (EDID_[11] < 16 ? "0" : "") + QString::number(EDID_[11], 16) +
                (EDID_[10] < 16 ? "0" : "") + QString::number(EDID_[10], 16);
    
    return cleanString(name_);
}

std::vector<EDIDParser::Resolution> EDIDParser::resolutions()
{
    if (!EDID_)
        return std::vector<Resolution>();
       
    if (!resolutions_.empty())
        return resolutions_;
    
    
    //try to find detailed timing descriptor(s)
    for (std::vector<int>::iterator itr=descriptors_.begin();itr!=descriptors_.end();++itr)
        if (EDID_[*itr]!=0x00 || EDID_[*itr+1]!=0x00)
        {
            Resolution res;
            res.x = int((EDID_[*itr + 4] & uint(0xf0)) << 4) + EDID_[*itr + 2];
            res.y = int((EDID_[*itr + 7] & uint(0xf0)) << 4) + EDID_[*itr + 5];
            qDebug() << "Resolution:" << res.x << res.y;
            resolutions_.push_back(res);
        }
    
    if (!resolutions_.empty())
        return resolutions_;
    
    //standard timing info
    for (unsigned int i=38;i<52 && i+1<EDIDsize_;i+=2)
        if (EDID_[i]!=1 || EDID_[i+1]!=1)
        {
            Resolution res;
            res.x = int(uint(EDID_[i]) + 31) * 8;
            switch(EDID_[i]&0xC0>>6)
            {
            case 0:
                if (edidVer_>=13)
                    res.y = 10*res.x/16;
                else
                    res.y = res.x;
                break;
            case 1:
                res.y = 3*res.x/4;
                break;
            case 2:
                res.y = 4*res.x/5;
                break;
            case 3: 
                res.y = 9*res.x/16;
                break;
            default:
                res.y = res.x;
                break;
            }
            qDebug() << "Resolution:" << res.x << res.y;
            resolutions_.push_back(res);
        }
    
    return resolutions_;
}

std::vector<EDIDParser::Resolution> EDIDParser::realSizesMm()
{
    if (!EDID_)
        return std::vector<Resolution>();
    
    if (!sizes_.empty())
        return sizes_;
    
    
    //try to find detailed timing descriptor(s)
    for (std::vector<int>::iterator itr=descriptors_.begin();itr!=descriptors_.end();++itr)
        if (EDID_[*itr] !=0x00 || EDID_[*itr + 1] != 0x00) {
            Resolution res;
            res.x = int(((uint32_t(EDID_[*itr + 14]) & unsigned(0xf0)) << 4) + EDID_[*itr + 12]);
            res.y = int(((uint32_t(EDID_[*itr + 14]) & unsigned(0x0f)) << 8) + EDID_[*itr + 13]);
            qDebug() << "Real size:" << res.x << res.y;
            sizes_.push_back(res);
        }
    
    if (!sizes_.empty())
        return sizes_;
    
    //standard edid size info
    if (EDIDsize_>22)
    {
        Resolution res;
        res.x = int(EDID_[21] * 10);
        res.y = int(EDID_[22] * 10);
        qDebug() << "Real size:" << res.x << res.y;
        sizes_.push_back(res);
    }
    
    return sizes_;
}

QString EDIDParser::manufacturer()
{
    if (!EDID_)
        return "";
       
    if (manufacturer_.length())
        return manufacturer_;
    
    manufacturer_ = getShortManufacturer();
    qDebug() << "%% Manufacturer:" << manufacturer_;
    
    manufacturer_ = longManufacturer(manufacturer_);

    qDebug() << "%% Manufacturer:" << manufacturer_;
    
    return manufacturer_;
}

bool EDIDParser::isSerialLooksFaked(QString serial, bool standardFakeOnly)
{
    if (serial.isEmpty())
        return true;
    
    static std::vector<QString> fakes{"01010101", "01000000", "00000000", "00000001"};

    if (standardFakeOnly)
        return serial==fakes[0];

    if (std::find(fakes.begin(), fakes.end(), serial)!=fakes.end())
        return true;
    
    return false;
}

QString EDIDParser::longManufacturer(QString shortManufacturer)
{
    static std::map<QString, QString> mfs;
    if (mfs.empty())
    {
        mfs["ABP"] = "AdvanSys";
        mfs["ACC"] = "Accton";
        mfs["ACU"] = "Acculogic";
        mfs["ADI"] = "ADI MicroScan";
        mfs["ADP"] = "Adaptec";
        mfs["ADV"] = "AMD";
        mfs["AEI"] = "Actiontec";
        mfs["ALI"] = "ALI";
        mfs["AMI"] = "AMI Enterprise";
        mfs["ANS"] = "Ansel Communications";
        mfs["AOC"] = "AOC SPECTRUM";
        mfs["APA"] = "Adaptec";
        mfs["APC"] = "APC";
        mfs["API"] = "AcerView";
        mfs["APP"] = "Apple";
        mfs["ARC"] = "Alta";
        mfs["ASC"] = "AdvanSys";
        mfs["AST"] = "ASTVision";
        mfs["ATI"] = "ATI";
        mfs["ATK"] = "Allied";
        mfs["AZT"] = "Aztech";
        mfs["BDS"] = "Barco";
        mfs["BRI"] = "Boca";
        mfs["BUS"] = "BusLogic";
        mfs["CON"] = "CONTEC";
        mfs["CPL"] = "Compal";
        mfs["CPQ"] = "Compaq";
        mfs["CPX"] = "Compex";
        mfs["CRS"] = "Cisco";
        mfs["CRX"] = "Cyrix";
        mfs["CSC"] = "Crystal";
        mfs["CSI"] = "Cabletron Systems";
        mfs["CTL"] = "Creative Labs";
        mfs["DBK"] = "Databook";
        mfs["DEL"] = "Dell";
        mfs["DLK"] = "D-Link";
        mfs["DWE"] = "Daewoo";
        mfs["DXP"] = "DataExpert";
        mfs["EIZ"] = "EIZO";
        mfs["ELS"] = "ELSA";
        mfs["EXI"] = "Exide";
        mfs["FDC"] = "Future Domain";
        mfs["FPS"] = "Fiskars";
        mfs["GSM"] = "LG";
        mfs["HAY"] = "Hayes";
        mfs["HEI"] = "HYUNDAI";
        mfs["HSL"] = "Hansol";
        mfs["HTC"] = "Hitachi";
        mfs["HWP"] = "HP";
        mfs["IBM"] = "IBM";
        mfs["ICL"] = "Fujitsu";
        mfs["ICU"] = "Intel";
        mfs["INI"] = "Initio";
        mfs["INT"] = "Intel";
        mfs["IVM"] = "Iiyama";
        mfs["KTC"] = "Kingston";
        mfs["KTM"] = "Kostec";
        mfs["LGI"] = "Logitech";
        mfs["LNE"] = "Linksys";
        mfs["MDG"] = "Madge";
        mfs["MDY"] = "Microdyne";
        mfs["MEI"] = "Panasonic";
        mfs["MEL"] = "Mitsubishi";
        mfs["MIR"] = "miro";
        mfs["MON"] = "Daewoo";
        mfs["MOT"] = "Motorola";
        mfs["MTX"] = "Matrox";
        mfs["NAN"] = "NANAO";
        mfs["NEC"] = "NEC";
        mfs["NIC"] = "Tulip";
        mfs["NMX"] = "NeoMagic";
        mfs["NOK"] = "NOKIA";
        mfs["NSC"] = "National Semiconductor";
        mfs["NVL"] = "Novell";
        mfs["OLC"] = "Olicom";
        mfs["PGS"] = "Princeton";
        mfs["PHL"] = "Philips";
        mfs["ROK"] = "Rockwell";
        mfs["RTL"] = "Realtek";
        mfs["SAM"] = "Samsung";
        mfs["SKD"] = "SysKonnect";
        mfs["SML"] = "Smile";
        mfs["SNY"] = "Sony";
        mfs["SRC"] = "ShamRock";
        mfs["SSC"] = "Sierra";
        mfs["SUP"] = "Supra";
        mfs["SVE"] = "SVEC";
        mfs["TAT"] = "Tatung";
        mfs["TCI"] = "Tulip";
        mfs["TCJ"] = "Teac";
        mfs["TCM"] = "3Com";
        mfs["TCO"] = "Thomas-Conrad";
        mfs["TEX"] = "Texas Instruments";
        mfs["TOS"] = "Toshiba";
        mfs["USC"] = "UltraStor";
        mfs["USR"] = "USR Sportster";
        mfs["VSC"] = "ViewSonic";
        mfs["WDC"] = "Western Digital";
        mfs["WTC"] = "WEN";
        mfs["WYS"] = "WYSE";
        mfs["YMH"] = "Yamaha";
        mfs["ZDS"] = "Zenith";
        mfs["WKC"] = "WIDE";
        mfs["KTM"] = "Kostec";
    }

    if (mfs.find(shortManufacturer)!=mfs.end())
        return mfs[shortManufacturer];

    return shortManufacturer;
}

QString EDIDParser::cleanString(QString s)
{
    return s.remove(qbx::RegExp(QString::fromUtf8("[^a-zA-Z -_0-9]")));
}
