#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include "stationform.h"
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <math.h>



void MainWindow::registStation(FRUSTATION aStation)
{
    if(!StationList.isEmpty())
    {
       for(int i = 0; i < StationList.size(); i++) {

            if(StationList[i].StationNumber == aStation.StationNumber)
            {

                  StationList[i] = aStation;

                return;
            }


        }
         StationList.append(aStation);
    }
    else
        StationList.append(aStation);

}

void MainWindow::turnStationOn(FRUSTATION aStation)
{
    // Send command to turn HV-ON
    UHV4INT->S_UHV4_HV_ON_OFF(aStation.PumpAddr,aStation.PumpCH,true);
    setStationStatus(aStation.StationNumber,StationOnIcon);
}

void MainWindow::turnStationOff(FRUSTATION aStation)
{
    // Send command to turn HV-ON
    UHV4INT->S_UHV4_HV_ON_OFF(aStation.PumpAddr,aStation.PumpCH,false);
    setStationStatus(aStation.StationNumber,StationNoIcon);
}

void MainWindow::turnRoughVACOn(FRUSTATION aStation)
{
    SetRoughVACStatus(aStation.StationNumber, StationOnIcon);
}

void MainWindow::turnRoughVACOff(FRUSTATION aStation)
{
    SetRoughVACStatus(aStation.StationNumber, StationNoIcon);
}

void MainWindow::startGetdata()
{
    connect(&getDatatimer,SIGNAL(timeout()),this,SLOT(getData()));
    connect(&getRFIDtimer,SIGNAL(timeout()),this,SLOT(getRFIDData()));
    getDatatimer.start(DATAUPDATEINTERVAL);
    getRFIDtimer.start(RFIDUPDATEINTERVAL);
}


void MainWindow::getData()
{
    getDatatimer.stop();
//    UHV4GetDataRequest aRequest;
//    aRequest.addr = 0;
//    aRequest.CH = ui->PumpCHSpinBox->value();

//    aRequest.requestType = UHV4_READ_PRESSURE;

//    emit requestData(aRequest);

//    aRequest.requestType = UHV4_READ_CURRENT;

//    emit requestData(aRequest);

//    aRequest.requestType = UHV4_READ_VOLTAGE;

//    emit requestData(aRequest);
    QEventLoop loop;
    QTimer  timeOuttimer;
    QObject::connect(UHV4INT, SIGNAL(SIG_readyWrite()), &loop, SLOT(quit()));


    QObject::connect(&timeOuttimer,SIGNAL(timeout()),&loop,SLOT(quit()));
    timeOuttimer.start(GET_DATA_TIMEOUT);

    if(!StationList.isEmpty())
    {
        foreach (FRUSTATION aStation, StationList) {

            //if(aStation.HVON == true && aStation.StationPageOpen == true)
            if(aStation.HVON == true)
            {
                UHV4INT->S_UHV4_read_Pressure(aStation.PumpAddr,aStation.PumpCH);
                loop.exec();
                UHV4INT->S_UHV4_read_Voltage(aStation.PumpAddr,aStation.PumpCH);
                loop.exec();
                UHV4INT->S_UHV4_read_Current(aStation.PumpAddr,aStation.PumpCH);
                loop.exec();

            }

        }
    }

    getDatatimer.start(DATAUPDATEINTERVAL);

}

void MainWindow::getRFIDData()
{
     QEventLoop loop;
     QObject::connect(CANSDCSINT,SIGNAL(SIG_CAN_newDatafromSDCS(SDCS)),&loop,SLOT(quit()));
      QTimer  timeOuttimer;
      QObject::connect(&timeOuttimer,SIGNAL(timeout()),&loop,SLOT(quit()));
      timeOuttimer.start(GET_DATA_TIMEOUT);

      if(!SDCSList.isEmpty())
      {
          qDebug() << "Entered SDCSList check in getData()";

              qDebug() << "Entered SDCSList check LOOP in getData()";
              CANSDCSINT->RequestDatafromSDCS(SDCSList[0]);
              loop.exec();
      }
}

double MainWindow::convertPressureToDouble(QByteArray input)
{
        QByteArray Value;
            QByteArray Value2;
            double output;
            int theExpo;
            double theValue;
            Value.append(input.at(3));
            Value.append(input.at(4));
            Value.append(input.at(5));

            theValue = Value.toDouble();

            Value2.append(input.at(8));
            Value2.append(input.at(9));

            theExpo = Value2.toInt();

            if(input.at(7) == '-')
            {
                output = theValue*pow(10,-(theExpo));
            }

            if(input.at(7) == '+')
            {
                output = theValue*pow(10,(theExpo));
            }

            return output;
}

void MainWindow::setStationStatus(int StationNumber, QIcon theIcon)
{
    switch(StationNumber)
    {
        case 1:
        ui->StationButton_1->setIcon(theIcon);
        break;
    case 2:
    ui->StationButton_5->setIcon(theIcon);
    break;

    case 3:
    ui->StationButton_3->setIcon(theIcon);
    break;

    case 4:
    ui->StationButton_4->setIcon(theIcon);
    break;

    }
}

void MainWindow::SetRoughVACStatus(int StationNumber, QIcon theIcon)
{

    switch(StationNumber)
    {
        case 1:
        ui->RoughVacButton_1->setIcon(theIcon);
        break;
    case 2:
    ui->RoughVacButton_2->setIcon(theIcon);
    break;

    case 3:
    ui->RoughVacButton_3->setIcon(theIcon);
    break;

    case 4:
    ui->RoughVacButton_4->setIcon(theIcon);
    break;

    }
}
 void MainWindow::updateStationStatusBaseOnPressure(QByteArray addr,int CH,QByteArray Pressure)
 {
        // get the addr
     unsigned char tmpAddr = (int)((unsigned char)addr[0] - 0x80);

        //convert pressure to double
     double tmpPressure = convertPressureToDouble(Pressure);
     // find the station base on the Pump  addr and the Pump CH in StationList

     foreach(FRUSTATION aStation, StationList)
     {
         if(aStation.PumpAddr == tmpAddr && aStation.PumpCH == CH)
         {
               // compare to see if the pressure in Spec, Spec it temparily set by config.h
             if(tmpPressure < PRESSURE_GLOBAL_SPEC_WARNING_LIMIT)
             {
                setStationStatus(aStation.StationNumber, StationOnIcon);
                aStation.emailcounter = 0;
             }
             else if(tmpPressure <= PRESSURE_GLOBAL_SPEC_UPPER_LIMIT && tmpPressure > PRESSURE_GLOBAL_SPEC_WARNING_LIMIT)
             {
                 setStationStatus(aStation.StationNumber, StationWarningIcon);
                 aStation.emailcounter = 0;
             }
             else
             {
                 setStationStatus(aStation.StationNumber, StationoffIcon);
                 if(aStation.emailcounter == MAXSAMPLETOSENDMAIL)
                 {
                     aStation.emailcounter = 0;

                     if(sendEmail == true)
                     {
                         QEventLoop loop;
                         QTimer atimer;
                         connect(&atimer,SIGNAL(timeout()),&loop,SLOT(quit()));
                         connect(this,SIGNAL(SIG_EmailSent()),&loop,SLOT(quit()));

                         emit SIG_sendAlertEmail(aStation.StationNumber, Pressure);
                         atimer.start(EMAILSENDTIMEOUT);
                         loop.exec();
                     }

                 }
                 aStation.emailcounter++;
             }
             break;
         }
     }

 }

 void MainWindow::SLOT_sendAlertEmail(int StationNumber, QByteArray Pressure)
 {
     SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    // We need to set the username (your email address) and the password
    // for smtp authentification.

    smtp.setUser("alertgdc@gmail.com");
    smtp.setPassword("GDC159753");

    // Now we create a MimeMessage object. This will be the email.

    MimeMessage message;

    message.setSender(new EmailAddress("alertgdc@gmail.com", "GDC"));
    message.addRecipient(new EmailAddress("tamtam4321@gmail.com", "Tam Duong"));
    message.addRecipient(new EmailAddress("tuan.q.nguyen@kla-tencor.com", "Tuan Nguyen"));
    message.setSubject("Warning - GDC - FRUs out of spec");

    // Now add some text to the email.
    // First we create a MimeText object.

    MimeText text;
    QString tmpString;
    tmpString.append("Station ");
    tmpString.append(StationNumber);
    tmpString.append(" is out of spec.\n");
    tmpString.append("Current pressure is: ");
    tmpString.append(Pressure);
    tmpString.append("Torr");

    text.setText(tmpString);

    // Now add it to the mail

    message.addPart(&text);

    // Now we can send the mail

    smtp.connectToHost();
    smtp.login();
    smtp.sendMail(message);
    smtp.quit();

    emit SIG_EmailSent();
 }

 void MainWindow::registSDCS(SDCS aSDCS)
 {
    if(!SDCSList.isEmpty())
    {
        for(int i = 0; i < SDCSList.size();i++)
        {
            if(SDCSList[i].SDCSID == aSDCS.SDCSID)
            {
                SDCSList[i].currentPacketNumber = aSDCS.currentPacketNumber;
                SDCSList[i].RFIDList[aSDCS.currentPacketNumber] = aSDCS.RFIDList[aSDCS.currentPacketNumber];
                SDCSList[i].RoughVACLine[aSDCS.currentPacketNumber] = aSDCS.RoughVACLine[aSDCS.currentPacketNumber];

                emit newDatafromSDCS(SDCSList[i]);

                return;
            }
        }
        SDCSList.append(aSDCS);
    }
    else
    {
        SDCSList.append(aSDCS);
    }
 }

 void MainWindow::showNewSDCSonSreen(SDCS aSDCS)
 {
     QMessageBox::information(this,(tr("Info")),tr("found SDCS with ID %1").arg((aSDCS.SDCSID + 1)));
 }

 void MainWindow::saveCurrentData(QByteArray pumpAddr ,int pumpCH ,QByteArray current)
 {
    unsigned char tmpAddr = (int)((unsigned char)pumpAddr[0] - 0x80);
    double current1 = convertPressureToDouble(current);
    double time = QDateTime::currentDateTime().toTime_t();

    CurrentData tmpData;
    tmpData.current = current1;
    tmpData.time = time;

    for(int i = 0; i < StationList.size();i++)
    {
        if(StationList[i].PumpAddr == tmpAddr && StationList[i].PumpCH == pumpCH)
        {
            for(int ii = 0; ii < CurrentDataStationList.size(); ii++)
            {
                if(CurrentDataStationList[ii].StationNumber == StationList[i].StationNumber)
                {
                    if(CurrentDataStationList[ii].data.size()> MAXIMUMDATAPOINT)
                    {
                        CurrentDataStationList[ii].data.removeFirst();
                    }
                    CurrentDataStationList[ii].data.append(tmpData);
                    return;
                }
            }
        }
    }
 }

 void MainWindow::savePressureData(QByteArray pumpAddr ,int pumpCH,QByteArray pressure)
 {  unsigned char tmpAddr = (int)((unsigned char)pumpAddr[0] - 0x80);
     double pressure1 = convertPressureToDouble(pressure);
     double time = QDateTime::currentDateTime().toTime_t();

     PressureData tmpData;
     tmpData.pressure = pressure1;
     tmpData.time = time;

     for(int i = 0; i < StationList.size();i++)
     {
         if(StationList[i].PumpAddr == tmpAddr && StationList[i].PumpCH == pumpCH)
         {
             for(int ii = 0; ii < PressureDataStationList.size(); ii++)
             {
                 if(PressureDataStationList[ii].StationNumber == StationList[i].StationNumber)
                 {
                     if(PressureDataStationList[ii].data.size()> MAXIMUMDATAPOINT)
                     {
                         PressureDataStationList[ii].data.removeFirst();
                     }
                     PressureDataStationList[ii].data.append(tmpData);
                     return;
                 }
             }
         }
     }
 }

 void MainWindow::saveVoltageData(QByteArray pumpAddr ,int pumpCH,int voltage)
 {
     unsigned char tmpAddr = (int)((unsigned char)pumpAddr[0] - 0x80);
          double time = QDateTime::currentDateTime().toTime_t();

          VoltageData tmpData;
          tmpData.voltage = voltage;
          tmpData.time = time;

          for(int i = 0; i < StationList.size();i++)
          {
              if(StationList[i].PumpAddr == tmpAddr && StationList[i].PumpCH == pumpCH)
              {
                  for(int ii = 0; ii < VoltageDataStationList.size(); ii++)
                  {
                      if(VoltageDataStationList[ii].StationNumber == StationList[i].StationNumber)
                      {
                          if(VoltageDataStationList[ii].data.size()> MAXIMUMDATAPOINT)
                          {
                              VoltageDataStationList[ii].data.removeFirst();
                          }
                          VoltageDataStationList[ii].data.append(tmpData);
                          return;
                      }
                  }
              }
          }
 }
