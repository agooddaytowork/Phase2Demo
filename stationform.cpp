#include "stationform.h"
#include "ui_stationform.h"
#include "config.h"
#include <QTime>
#include <math.h>
#include <QMessageBox>

StationForm::StationForm(QWidget *parent, int Station) :
    QWidget(parent),
    ui(new Ui::StationForm)
{
    // Set White Background
    QPixmap pixmap(":/images/test2.png");
    QPalette aPal;
    aPal.setBrush(QPalette::Background,pixmap);
    this->setPalette(aPal);

    // Initialize
    StationNumber = Station;

//    aPressureDataStation.StationNumber = Station;
//    aVoltageDataStation.StationNumber = Station;
//    aCurrentDataStation.StationNumber = Station;

    aStation.StationPageOpen = true;

    ui->setupUi(this);

    // Set Station NUmber TO GUI
    QString A;
    A.append("STATION ");
    A.append(QString::number(StationNumber));
    ui->stationlabel->setText(A);

    // Set Text for ROUGH VAC BUTTON
    ui->HVOFFButton->setText("ROUGH\nVAC ON" );

    //    if((aStation.SDCSID == aSDCS.SDCSID) && (aSDCS.currentPacketNumber == (StationNumber - 1)))
    //    {
    //        QString ba = aSDCS.RFIDList[aSDCS.currentPacketNumber].toHex();
    //        ui->RFIDLineEdit->setText(ba);
    //    }
    InitGraph();


}

StationForm::~StationForm()
{
    delete ui;
}

void StationForm::on_pushButton_clicked()
{
    aStation.StationPageOpen = false;
    updateStationInfo();
    emit SIG_SF_StationInfo(aStation);
//    emit SIG_SF_saveCurrentData(aCurrentDataStation);
//    emit SIG_SF_savePressureData(aPressureDataStation);
//    emit SIG_SF_saveVoltageData(aVoltageDataStation);
    this->close();

}

void StationForm::InitGraph()
{
    QFont font;
       font.setStyleStrategy(QFont::NoAntialias);

       ui->StationGraph->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
       // Init for Graph Channel 1

        ui->StationGraph->setNotAntialiasedElements(QCP::aeAll);
        ui->StationGraph->xAxis->setTickLabelFont(font);
        ui->StationGraph->yAxis->setTickLabelFont(font);
        ui->StationGraph->yAxis2->setTickLabelFont(font);
        ui->StationGraph->legend->setFont(font);

        QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
        timeTicker->setDateTimeFormat(" h:m:s\n MM.d.yy");
   //     QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
   //     timeTicker->setTimeFormat("%h:%m:%s");
        timeTicker->setTickCount(TIMEAXIS_TICKCOUNT);

        ui->StationGraph->xAxis->setTicker(timeTicker);

        ui->StationGraph->yAxis->grid()->setSubGridVisible(true);
        ui->StationGraph->xAxis->grid()->setSubGridVisible(true);
        ui->StationGraph->yAxis->setScaleType(QCPAxis::stLogarithmic);
        //ui->StationGraph->yAxis2->setScaleType(QCPAxis::stLogarithmic);
        QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
        QSharedPointer<QCPAxisTicker> VolTageTicker (new QCPAxisTicker);
        ui->StationGraph->yAxis->setTicker(logTicker);
        ui->StationGraph->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
        ui->StationGraph->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
        ui->StationGraph->yAxis->setRange(PRESSURE_LOWER, PRESSURE_UPPER);

        ui->StationGraph->yAxis2->setTicker(VolTageTicker);
        ui->StationGraph->yAxis2->setRange(VOLTAGE_LOWER,VOLTAGE_UPPER);
        ui->StationGraph->yAxis2->setVisible(true);


      /** ADD GRAPH TO EACH CHANEL **/

      QPen pen;
      pen.setColor(QColor(255, 170, 100));
      pen.setWidth(3);
      //pen.setStyle(Qt::DotLine);

      QPen pen1;
      pen1.setColor(QColor(20, 100, 0));
      pen1.setWidth(3);

      QPen pen2;
      pen2.setColor(QColor(20, 255, 0));
      pen2.setWidth(3);



      ui->StationGraph->addGraph(ui->StationGraph->xAxis,ui->StationGraph->yAxis);
      ui->StationGraph->graph(0)->setPen(pen);
      ui->StationGraph->graph(0)->setName("P");

      ui->StationGraph->addGraph(ui->StationGraph->xAxis,ui->StationGraph->yAxis2);
      ui->StationGraph->graph(1)->setPen(pen1);
      ui->StationGraph->graph(1)->setName("V");

      ui->StationGraph->addGraph(ui->StationGraph->xAxis,ui->StationGraph->yAxis);
      ui->StationGraph->graph(2)->setPen(pen2);
      ui->StationGraph->graph(2)->setName("I");


      ui->StationGraph->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
      ui->StationGraph->legend->setVisible(true);


      ui->StationGraph->yAxis->setLabel("FRU Chamber Pressure (Torr) / Pump Current (A)");
      ui->StationGraph->yAxis2->setLabel("Pump Voltage (V)");
      ui->StationGraph->xAxis->setLabel("Time (h:m:s - MM.d.yy)");



//      QLinearGradient plotGradient;
//      plotGradient.setStart(0, 0);
//      plotGradient.setFinalStop(0, 350);
//      plotGradient.setColorAt(0, QColor(225, 225, 225));
//      plotGradient.setColorAt(1, QColor(175, 175, 175));
//       ui->StationGraph->setBackground(plotGradient);
       ui->StationGraph->yAxis->setTickLabelFont(QFont(QFont().family(),11));
       ui->StationGraph->yAxis2->setTickLabelFont(QFont(QFont().family(),11));
       ui->StationGraph->xAxis->setTickLabelFont(QFont(QFont().family(),9));
}

void StationForm::plotCurrent(QByteArray addr, int CH, QByteArray current)
{
    unsigned char tmpAddr = (int)((unsigned char)addr[0] - 0x80);

    if(aStation.PumpAddr == tmpAddr && aStation.PumpCH == CH)
    {
        double key = QDateTime::currentDateTime().toTime_t();
           double current1 = convertToDouble(current);

           ui->StationGraph->graph(2) ->addData(key,current1);
           ui->StationGraph->replot();

//           // Add data to list
//           CurrentData tmpCurrentData;
//           tmpCurrentData.time = key;
//           tmpCurrentData.current = current1;

//           if(aCurrentDataStation.data.size() > MAXIMUMDATAPOINT)
//           {
//               aCurrentDataStation.data.removeFirst();
//           }
//           aCurrentDataStation.data.append(tmpCurrentData);

    }


}

void StationForm::plotPressure(QByteArray addr, int CH, QByteArray pressure)
{
    unsigned char tmpAddr = (int)((unsigned char)addr[0] - 0x80);

     if(aStation.PumpAddr == tmpAddr && aStation.PumpCH == CH)
     {
         double key = QDateTime::currentDateTime().toTime_t();
         double pressure1 = convertToDouble(pressure);

         ui->StationGraph->graph(0)->addData(key,pressure1);
         ui->StationGraph->replot();

//         // Add data to list

//         PressureData tmpPressureData;
//         tmpPressureData.time = key;
//         tmpPressureData.pressure = pressure1;
//         if(aPressureDataStation.data.size() > MAXIMUMDATAPOINT)
//         {
//             aPressureDataStation.data.removeFirst();
//         }
//            aPressureDataStation.data.append(tmpPressureData);

     }

}

void StationForm::plotVoltage(QByteArray addr, int CH, int voltage)
{
    static int AutoRangeFactor = 10;
    static int counter = 1;
    unsigned char tmpAddr = (int)((unsigned char)addr[0] - 0x80);

     if(aStation.PumpAddr == tmpAddr && aStation.PumpCH == CH)
     {
         double key = QDateTime::currentDateTime().toTime_t();
         ui->StationGraph->graph(1)->addData(key, voltage);
         //ui->StationGraph->xAxis->setRange(key,AutoRangeFactor*DATASAMPLETIME,Qt::AlignRight);

         ui->StationGraph->xAxis->setRange(key,250,Qt::AlignRight);
         ui->StationGraph->replot();

//         // Auto Range
//         if(counter == 5)
//         {
//             if(AutoRangeFactor <= TIMEAXIS_AUTO_RANGE)
//             AutoRangeFactor = AutoRangeFactor + 5;
//             counter =0;
//         }
//         counter++;

//         // Add Data to list
//         VoltageData tmpVoltageData;
//         tmpVoltageData.time = key;
//         tmpVoltageData.voltage = voltage;

//         if(aVoltageDataStation.data.size() > MAXIMUMDATAPOINT)
//         {
//             aVoltageDataStation.data.removeFirst();
//         }
//         aVoltageDataStation.data.append(tmpVoltageData);



     }
}

double StationForm::convertToDouble(QByteArray input)
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
        } // Set Station NUmber TO GUI
        QString A;
        A.append("Station ");
        A.append(QString::number(StationNumber));
        ui->stationlabel->setText(A);


        if(input.at(7) == '+')
        {
            output = theValue*pow(10,(theExpo));
        }

        return output;
}



void StationForm::on_HVONButton_clicked()
{

     updateStationInfo();
    if(aStation.HVON == false)
    {
        aStation.HVON = true;
        ui->HVONButton->setText("HV OFF");
        emit SIG_SF_StationInfo(aStation);
        emit SIG_SF_turnHVON(aStation);
        return;
    }
    else
    {
        aStation.HVON = false;
        ui->HVONButton->setText("HV ON");
        emit SIG_SF_StationInfo(aStation);
        emit SIG_SF_turnHVOFF(aStation);
    }


}

void StationForm::on_HVOFFButton_clicked()
{
    updateStationInfo();
   if(aStation.RoughVacOn == false)
   {
       aStation.RoughVacOn = true;
       ui->HVOFFButton->setText("ROUGH\nVAC OFF");
       emit SIG_SF_StationInfo(aStation);
       emit SIG_SF_turnRoughVACON(aStation);
       return;
   }
   else
   {
       aStation.RoughVacOn = false;
       ui->HVOFFButton->setText("ROUGH\nVAC ON");
       emit SIG_SF_StationInfo(aStation);
       emit SIG_SF_turnRoughVACOFF(aStation);
   }
}

void StationForm::updateStationInfo()
{
    aStation.PumpAddr = ui->PumpAddrSpinBox->value();
    aStation.PumpCH = ui->PumpCHSpinBox->value();
    aStation.StationNumber = StationNumber;

//    aStation.SDCSNumber =
}

void StationForm::updateStationInfotoScreen()
{
    aStation.StationPageInitiated = true;

    ui->PumpAddrSpinBox->setValue(aStation.PumpAddr);
    ui->PumpCHSpinBox->setValue(aStation.PumpCH);
    if(aStation.HVON == true)
    {
        ui->HVONButton->setText("HV OFF");
    }
    else
    {
        ui->HVONButton->setText("HV ON");
    }
    if(aStation.RoughVacOn == true)
    {
       ui->HVOFFButton->setText("ROUGH\nVAC OFF");
    }
    else
    {
        ui->HVOFFButton->setText("ROUGH\nVAC ON");
    }
    if(aStation.aStationInfo.RFID != "")
    {
        ui->RFIDLineEdit->setText(aStation.aStationInfo.RFID);
        ui->PNLineEdit->setText(aStation.aStationInfo.KTPN);
        ui->SerialLineEdit->setText(aStation.aStationInfo.KTSerial);
        ui->LPNLineEdit->setText(aStation.aStationInfo.LPN);
        ui->TestDateLineEdit->setText(aStation.aStationInfo.TestDate);
        ui->GunnOffGCClosedLineEdit->setText(aStation.aStationInfo.GunOffClose);
        ui->POLineEdit->setText(aStation.aStationInfo.PO);
        ui->RCVLineEdit->setText(aStation.aStationInfo.ReceivedDate);
        ui->ShippedLineEdit->setText(aStation.aStationInfo.TestDate);
    }



     //check if there are Data, then load to graph

    if(aStation.StationPageInitiated == true)
    {
        // Pressure, Voltage, Current 0 1 2
        if(aVoltageDataStation.data.isEmpty() == false)
        {
            int DataSize = aVoltageDataStation.data.size();
             ui->StationGraph->xAxis->setRange(aVoltageDataStation.data[0].time,aVoltageDataStation.data.size(),Qt::AlignRight);
            for(int i = 0; i < DataSize; i++)
            {
               ui->StationGraph->graph(1)->addData(aVoltageDataStation.data[i].time, aVoltageDataStation.data[i].voltage);
            }

        }
        if(aPressureDataStation.data.isEmpty()==false)
        {
            int DataSize = aPressureDataStation.data.size();

            for(int i = 0; i <DataSize; i++)
            {
               ui->StationGraph->graph(0)->addData(aPressureDataStation.data[i].time, aPressureDataStation.data[i].pressure);
            }

        }
        if(aCurrentDataStation.data.isEmpty() == false)
        {
            int DataSize = aCurrentDataStation.data.size();

            for(int i = 0; i <DataSize; i++)
            {
               ui->StationGraph->graph(2)->addData(aCurrentDataStation.data[i].time,aCurrentDataStation.data[i].current);
            }

        }

        ui->StationGraph->replot();
    }
}

void StationForm::setPumpConfiguration(int addr, int CH)
{
    ui->PumpAddrSpinBox->setValue(addr);
    ui->PumpCHSpinBox->setValue(CH);
}

void StationForm::InformCommandToPumpIsNotExecuted(int pumpaddr)
{
    if(aStation.PumpAddr == pumpaddr)
    {
        QMessageBox::warning(this,(tr("Warning")),tr("Command sent to Pump cannot be executed\n Please check error on pump controller"));
    }
}

void StationForm::on_pushButton_2_clicked()
{

}

void StationForm::updateSDCSdatatoScreen(SDCS aSDCS)
{
    if((aStation.SDCSID == aSDCS.SDCSID) && (aSDCS.currentPacketNumber == (StationNumber - 1)))
    {
        QString ba = aSDCS.RFIDList[aSDCS.currentPacketNumber].toHex();
        ui->RFIDLineEdit->setText(ba);
        aStation.aStationInfo.RFID = ba;
        QString defaultRFID = "00000000";
        if(QString::compare(ba,defaultRFID,Qt::CaseInsensitive) == 0)
        {
            ui->PNLineEdit->setText("");
            ui->SerialLineEdit->setText("");
            ui->LPNLineEdit->setText("");
            ui->TestDateLineEdit->setText("");
            ui->GunnOffGCClosedLineEdit->setText("");
            ui->POLineEdit->setText("");
            ui->RCVLineEdit->setText("");
            ui->ShippedLineEdit->setText("");
        }
        else
        {
            ui->RFIDLineEdit->setText(aStation.aStationInfo.RFID);
            ui->PNLineEdit->setText(aStation.aStationInfo.KTPN);
            ui->SerialLineEdit->setText(aStation.aStationInfo.KTSerial);
            ui->LPNLineEdit->setText(aStation.aStationInfo.LPN);
            ui->TestDateLineEdit->setText(aStation.aStationInfo.TestDate);
            ui->GunnOffGCClosedLineEdit->setText(aStation.aStationInfo.GunOffClose);
            ui->POLineEdit->setText(aStation.aStationInfo.PO);
            ui->RCVLineEdit->setText(aStation.aStationInfo.ReceivedDate);
            ui->ShippedLineEdit->setText(aStation.aStationInfo.TestDate);
        }

    }
}
