#include "canint.h"
#include <QDebug>
#include <QEventLoop>
#include <QObject>

CANINT::CANINT(QObject *parent)
{

}

void CANINT::init()
{
    foreach (const QByteArray &backend, QCanBus::instance()->plugins()) {
        if(backend == "socketcan")
        {
            qDebug() << "Support socket can";
            break;
        }
    }


    device = QCanBus::instance()->createDevice("socketcan", QStringLiteral("can0"));
     device->setConfigurationParameter(QCanBusDevice::LoopbackKey,QVariant("false"));


    if(!device)
    {
        qDebug() <<"Device cannot be created";
    }
    if(!device->connectDevice())
    {
        qDebug() << "Connection error" << device->errorString();
    }
    else
    {
        connect(device,SIGNAL(framesReceived()), this,SLOT(ProcessReceiveFrame()));
    }

    // write a CAN FAME To see what happen hihi
//    QCanBusFrame frame;
//    frame.setFrameId(8);
//    QByteArray payload("A36E");
//    frame.setPayload(payload);
//    device->writeFrame(frame);

}

void CANINT::ProcessReceiveFrame()
{
    if(!device)
    {
        qDebug() <<"Device cannot be created";
        return;
    }

    const QCanBusFrame frame = device->readFrame();
    const qint8 dataLength = frame.payload().size();
    const qint32 id = frame.frameId();
    const QByteArray Payload = frame.payload();

    if(frame.frameType() != QCanBusFrame::ErrorFrame)
    {
#if _DEBUG

    qDebug() << "_____________________";
    qDebug()<<"ID: " << QString::number(id,16);
    qDebug()<< "dataLength: "  << dataLength;
      qDebug()<<"PayLoad: " << Payload;

#endif

    SDCS aSDCS;

    // check for presence repsonse from SDCS
    if((id & 0x0f) == 0x0f)
    {

        aSDCS.SDCSID = id>>4;
       // = id >> 4;
       emit SIG_CAN_FoundNewSDCS(aSDCS);

       SDCSfound = true;

#if _DEBUG
    qDebug() << "Enter Check for presence response from SDCS";
    qDebug() << "SDCS ID: " << aSDCS.SDCSID;
#endif

    }
    else
    {
#if _DEBUG
    qDebug() << "_____________________";
    qDebug() << "Enter 2nd part of CAN";
#endif
       aSDCS.SDCSID = id>>4;
       int PacketNumber = (id & 0x0f);

#if _DEBUG
    qDebug() << "PacketNumber" << PacketNumber;
#endif
           if(PacketNumber <=5)
           {
            aSDCS.currentPacketNumber = PacketNumber;
            aSDCS.RFIDList[PacketNumber].append(Payload[0]);
            aSDCS.RFIDList[PacketNumber].append(Payload[1]);
            aSDCS.RFIDList[PacketNumber].append(Payload[2]);
            aSDCS.RFIDList[PacketNumber].append(Payload[3]);
             emit SIG_CAN_newDatafromSDCS(aSDCS);


#if _DEBUG
    qDebug() << "Enter Check for data response from SDCS";
    qDebug() << "SDCS ID: " << aSDCS.SDCSID;
    qDebug() << "RFID: " << aSDCS.RFIDList[PacketNumber];
#endif
           }

    }

    }
}

void CANINT::CANsend(int ID, QByteArray payload)
{

    QCanBusFrame aFrame;
    aFrame.setFrameId(ID);
    aFrame.setPayload(payload);
    device->writeFrame(aFrame);
}

void CANINT::ScanforAvailableSDCS()
{
    // send request with ID 0xf0, dump 1 byte data length
     CANsend(0xf0,".");
}

void CANINT::RequestDatafromSDCS(SDCS aSDCS)
{
    //QEventLoop loop;
    //connect(device,SIGNAL(framesWritten(qint64)),&loop,SLOT(quit()));
    // send request with ID 0bxxxx1111 , xxxx = ID of SDCS
    qint32 id = ((aSDCS.SDCSID <<4) | 7);
    qDebug() << "CAN request Data, ID: " << id;
    CANsend(id,".");
    //loop.exec();


}
