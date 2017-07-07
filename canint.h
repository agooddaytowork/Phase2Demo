#ifndef CANINT_H
#define CANINT_H
#include <QCanBus>
#include <QCanBusFrame>
#include <QObject>
#include "uhv4receivemessage.h"
#include "config.h"

class CANINT: public QObject
{
    Q_OBJECT
public:
     CANINT(QObject *parent = 0);
     void init();
     bool SDCSfound = false;
     QCanBusDevice *device;
public slots:
    void ProcessReceiveFrame();
    void ScanforAvailableSDCS();
    void RequestDatafromSDCS(SDCS);
    void CANsend(int ID, QByteArray payload);
signals:
    void SIG_CAN_FoundNewSDCS(SDCS);
    void SIG_CAN_newDatafromSDCS(SDCS);


};

#endif // CANINT_H
