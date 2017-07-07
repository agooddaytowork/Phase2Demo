#ifndef UHV4RECEIVEMESSAGE_H
#define UHV4RECEIVEMESSAGE_H
#include <QByteArray>

#define CH1 1
#define CH2 2
#define CH3 3
#define CH4 4

#define UHV4_READ_PRESSURE 1
#define UHV4_READ_VOLTAGE 2
#define UHV4_READ_CURRENT 3

struct UHV4ReceiveMessage
{
    QByteArray addr;
    int win;
    QByteArray com;
    QByteArray data;
    QByteArray CRC;
    bool valid;
};

struct UHV4GetDataRequest
{
    int addr;
    int CH;
    int requestType;
};


struct FRUINFO
{
   QString RFID ="";
   QString KTPN ="";
   QString KTSerial="";
   QString LPN="";
   QString TestDate="";
   QString GunOffClose="";
   QString PO="";
   QString ReceivedDate="";
   QString ShippedDate="";
};

struct FRUSTATION
{
    int StationNumber = 0;
    int PumpAddr=0;
    int PumpCH=0;
    int SDCSID = 0;
    int SDCSCH = 0;
    bool HVON = false;
    bool RoughVacOn = false;
    bool StationPageOpen = false;
    bool StationPageInitiated = false;
    FRUINFO aStationInfo;
    int emailcounter=0;

};

struct SDCS
{
    int SDCSID = 0;
    QByteArray RFIDList[6];
    bool RoughVACLine[6];
    int currentPacketNumber = 0;

};

struct StationData
{
    int StationNumber = 0;

};

struct PressureData
{
    double time;
    double pressure;
};

struct CurrentData
{
    double time;
    double current;
};

struct VoltageData
{
    double time;
    int voltage;
};

struct PressureDataStation
{
    int StationNumber = 0;
    QList<PressureData> data;
};

struct CurrentDataStation
{
    int StationNumber = 0;
    QList<CurrentData> data;
};

struct VoltageDataStation
{
    int StationNumber = 0;
    QList<VoltageData> data;
};

#endif // UHV4RECEIVEMESSAGE_H
