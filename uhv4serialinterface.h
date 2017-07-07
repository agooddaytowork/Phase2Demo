#ifndef UHV4SERIALINTERFACE_H
#define UHV4SERIALINTERFACE_H

#include "windowcommand.h"
#include <QObject>
#include <QtSerialPort>
#include <QTimer>
#include "uhv4receivemessage.h"
#include "uhv4errormessage.h"
#include "config.h"
#include <QMessageBox>

class UHV4SerialInterface: public QObject
{
    Q_OBJECT

public:
    UHV4SerialInterface(QObject *parent = 0);
    ~UHV4SerialInterface();

    bool SerialConfig(const QString &PortName, const int &Baudrate);
    void SerialDisconnect();
    void WriteCommand(int ADDR, QByteArray WIN,  QByteArray DATA);
    void ReadCommand(int ADDR,  QByteArray WIN);

    bool Serialconnected = false;

    /** Global Variable **/
    bool CH1_ON = false;
    bool CH2_ON = false;
    bool CH3_ON = false;
    bool CH4_ON = false;
    bool getDatadisable = false;

    int currentChanel = 0;

    bool UHV4_FLAG_NACK = false;
    int currentPumpaddr = 0;
signals:
    void messageReady(UHV4ReceiveMessage Message);

    /** Signal for dealing with the user interface **/

    void SIG_UHV4_PRESSURE(QByteArray addr, int CH, QByteArray pressure);
    void SIG_UHV4_VOLTAGE(QByteArray addr, int CH, int voltage);
    void SIG_UHV4_CURRENT(QByteArray addr, int CH, QByteArray current);
    void SIG_UHV4_MODEL(QByteArray addr, QByteArray Model);
    void SIG_UHV4_MODEL_SERIAL(QByteArray addr, QByteArray Serial);
    void SIG_UHV4_ACK(int addr);
    void SIG_UHV4_NACK(int addr);
    void SIG_UHV4_ErrorShowed();
    void SIG_UHV4_ErrorReadyShow(UHV4ErrorMessage errorMSG);
    void SIG_readyWrite();
    void SIG_UHV4_SerialError();
public slots:
    void SerialReceive();
    void MessageParser(UHV4ReceiveMessage Message);

    /** Slots for controlling the UHV4 controller **/
    void S_UHV4_HV_ON_OFF(int addr , int CH, bool status);
    void S_UHV4_PROTECT_ON_OFF(int addr, int CH, bool status);
    void S_UHV4_read_Pressure(int addr, int CH);
    void S_UHV4_read_Voltage(int addr, int CH);
    void S_UHV4_read_Current(int addr, int CH);
    void S_UHV4_Send_CheckError_command(int addr);
    void S_UHV4_ErrorParser(int addr, int CH, int Message);

    void setFlagNACK(int currentPumpaddr);

    /** Get DATA **/
    void S_UHV4_getData();

    /** Handle Serial Error*/

    void UHV4_Serial_Error_Handler(QSerialPort::SerialPortError Error);
private:
    QString _PortName;
    int _Baudrate;

    QSerialPort serial;
    QTimer InternalTimer;
    QByteArray ReceiveData;
    bool Busy = false;



};

#endif // UHV4SERIALINTERFACE_H
