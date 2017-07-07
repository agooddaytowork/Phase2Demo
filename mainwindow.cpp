#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include "stationform.h"
#include <QDebug>
#include <QListView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    initMainwindow();

    //CANSDCSINT->init();

    connect(this,SIGNAL(StationClicked(int)),this,SLOT(createStationWindow(int)));
    connect(this,SIGNAL(SIG_sendAlertEmail(int,QByteArray)),this,SLOT(SLOT_sendAlertEmail(int,QByteArray)));
    connect(this,SIGNAL(SIG_sendTestEmail()),this,SLOT(SLOT_sendTestEmail()));
    connect(UHV4INT,SIGNAL(SIG_UHV4_PRESSURE(QByteArray,int,QByteArray)),this,SLOT(updateStationStatusBaseOnPressure(QByteArray,int,QByteArray)));
    connect(UHV4INT,SIGNAL(SIG_UHV4_CURRENT(QByteArray,int,QByteArray)),this,SLOT(saveCurrentData(QByteArray,int,QByteArray)));
    connect(UHV4INT,SIGNAL(SIG_UHV4_PRESSURE(QByteArray,int,QByteArray)),this,SLOT(savePressureData(QByteArray,int,QByteArray)));
    connect(UHV4INT,SIGNAL(SIG_UHV4_VOLTAGE(QByteArray,int,int)),this,SLOT(saveVoltageData(QByteArray,int,int)));

    connect(CANSDCSINT,SIGNAL(SIG_CAN_FoundNewSDCS(SDCS)),this,SLOT(registSDCS(SDCS)));
    connect(CANSDCSINT,SIGNAL(SIG_CAN_FoundNewSDCS(SDCS)),this,SLOT(showNewSDCSonSreen(SDCS)));
    connect(CANSDCSINT,SIGNAL(SIG_CAN_newDatafromSDCS(SDCS)),this,SLOT(registSDCS(SDCS)));


    // Create Data list ;

    PressureDataStation aPressureStation;
    CurrentDataStation  aCurrentStation;
    VoltageDataStation  aVoltageStation;

    for (int i = 1; i <= 4; i++)
    {
        aPressureStation.StationNumber = i;
        aCurrentStation.StationNumber = i;
        aVoltageStation.StationNumber = i;

        PressureDataStationList.append(aPressureStation);
        CurrentDataStationList.append(aCurrentStation);
        VoltageDataStationList.append(aVoltageStation);
    }


    startGetdata();

}

MainWindow::~MainWindow()
{
    UHV4INT->disconnect();
    delete ui;
}

void MainWindow::initMainwindow()
{
    // Set To white Background
    QPixmap pixmap(":/images/test.jpg");

    QPalette aPal;
    aPal.setBrush(QPalette::Background,pixmap);

    this->setPalette(aPal);

    // Initialize Icons
    StationOnPixMap =  QPixmap(":/images/station-on.png");
    StationOffPixMap = QPixmap(":/images/station-off.png");
    StaionNoPixMap = QPixmap(":/images/station-no.png");
    StationWarningPixMap = QPixmap(":/images/station-warning1.png");

    StationOnIcon = QIcon(StationOnPixMap);
    StationoffIcon = QIcon(StationOffPixMap);
    StationNoIcon = QIcon(StaionNoPixMap);
    StationWarningIcon = QIcon(StationWarningPixMap);

    ui->StationButton_1->setIcon(StationNoIcon);
    ui->StationButton_1->setIconSize(StationOnPixMap.size());

    ui->StationButton_5->setIcon(StationNoIcon);
    ui->StationButton_5->setIconSize(StationOnPixMap.size());

    ui->StationButton_3->setIcon(StationNoIcon);
    ui->StationButton_3->setIconSize(StationOnPixMap.size());

    ui->StationButton_4->setIcon(StationNoIcon);
    ui->StationButton_4->setIconSize(StationOnPixMap.size());

    ui->RoughVacButton_1->setIcon(StationNoIcon);
    ui->RoughVacButton_1->setIconSize(StationOnPixMap.size());

    ui->RoughVacButton_2->setIcon(StationNoIcon);
    ui->RoughVacButton_2->setIconSize(StationOnPixMap.size());

    ui->RoughVacButton_3->setIcon(StationNoIcon);
    ui->RoughVacButton_3->setIconSize(StationOnPixMap.size());

    ui->RoughVacButton_4->setIcon(StationNoIcon);
    ui->RoughVacButton_4->setIconSize(StationOnPixMap.size());

    // load the Serial Ports

    foreach (QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        ui->RS485comboBox->addItem(port.portName());
    }

}


void MainWindow::createStationWindow(int StationNumber)
{
    getDatatimer.stop();
    StationForm *aStation = new StationForm(0, StationNumber);

    aStation->aStation.aStationInfo.KTPN = "0352569-000";
    aStation->aStation.aStationInfo.KTSerial = "M804";
    aStation->aStation.aStationInfo.LPN = "SU-0000022BAEFF";
    aStation->aStation.aStationInfo.TestDate = "9/6/16";
    aStation->aStation.aStationInfo.GunOffClose=("4E-10");
    aStation->aStation.aStationInfo.PO=("ZR12468577");
    aStation->aStation.aStationInfo.ReceivedDate=("9/14/16");


    for(int i = 0; i < StationList.size();i++)
    {
        if(StationList[i].StationNumber == StationNumber)
        {
            StationList[i].StationPageOpen = true;
            aStation->aStation = StationList[i];



            int ii = 0;
            for(ii =0; ii < CurrentDataStationList.size();ii++)
            {
                if(CurrentDataStationList[ii].StationNumber == StationNumber)
                {
                    aStation->aCurrentDataStation = CurrentDataStationList[ii];
                    break;
                }
            }
            for(ii =0; ii < PressureDataStationList.size();ii++)
            {
                if(PressureDataStationList[ii].StationNumber == StationNumber)
                {
                    aStation->aPressureDataStation = PressureDataStationList[ii];
                    break;
                }
            }

            for(ii =0; ii < VoltageDataStationList.size();ii++)
            {
                if(VoltageDataStationList[ii].StationNumber == StationNumber)
                {
                    aStation->aVoltageDataStation = VoltageDataStationList[ii];
                    break;
                }
            }
            break;
        }
    }
//    aStation->aStation.StationPageOpen = true;
//    SDCS aSDCS;


    aStation->updateStationInfotoScreen();
    aStation->show();
    aStation->setPumpConfiguration(0,StationNumber); // this one is only temporary



    connect(aStation,SIGNAL(SIG_SF_StationInfo(FRUSTATION)),this,SLOT(registStation(FRUSTATION)));
    connect(aStation,SIGNAL(SIG_SF_turnHVON(FRUSTATION)),this,SLOT(turnStationOn(FRUSTATION)));
    connect(aStation,SIGNAL(SIG_SF_turnHVOFF(FRUSTATION)),this,SLOT(turnStationOff(FRUSTATION)));
    connect(aStation,SIGNAL(SIG_SF_turnRoughVACOFF(FRUSTATION)),this,SLOT(turnRoughVACOff(FRUSTATION)));
    connect(aStation,SIGNAL(SIG_SF_turnRoughVACON(FRUSTATION)),this,SLOT(turnRoughVACOn(FRUSTATION)));
//    connect(aStation,SIGNAL(SIG_SF_saveCurrentData(CurrentDataStation)),this,SLOT(saveCurrentData(CurrentDataStation)));
//    connect(aStation,SIGNAL(SIG_SF_savePressureData(PressureDataStation)),this,SLOT(savePressureData(PressureDataStation)));
//    connect(aStation,SIGNAL(SIG_SF_saveVoltageData(VoltageDataStation)),this,SLOT(saveVoltageData(VoltageDataStation)));

    connect(UHV4INT,SIGNAL(SIG_UHV4_CURRENT(QByteArray,int,QByteArray)),aStation,SLOT(plotCurrent(QByteArray,int,QByteArray)));
    connect(UHV4INT,SIGNAL(SIG_UHV4_PRESSURE(QByteArray,int,QByteArray)),aStation,SLOT(plotPressure(QByteArray,int,QByteArray)));
    connect(UHV4INT,SIGNAL(SIG_UHV4_VOLTAGE(QByteArray,int,int)),aStation,SLOT(plotVoltage(QByteArray,int,int)));
    connect(UHV4INT,SIGNAL(SIG_UHV4_NACK(int)),aStation,SLOT(InformCommandToPumpIsNotExecuted(int)));



    connect(this,SIGNAL(newDatafromSDCS(SDCS)),aStation,SLOT(updateSDCSdatatoScreen(SDCS)));
    //connect(CANSDCSINT,SIGNAL(SIG_CAN_newDatafromSDCS(SDCS)),aStation,SLOT(updateSDCSdatatoScreen(SDCS)));

    getDatatimer.start(DATAUPDATEINTERVAL);
}

void MainWindow::on_RS485ConnectButton_clicked()
{

    if (ui->RS485comboBox->currentText() !="")
 {
     //connect to serial port.
     if(!SerialConnected)
     {
         if (UHV4INT->SerialConfig(ui->RS485comboBox->currentText(), 38400)) {

   #if _DEBUG
             qDebug() << "Serial port conneted, do something here";
   #endif
             UHV4INT->Serialconnected = true;
            SerialConnected = true;
            ui->RS485ConnectButton->setText("Disconnect");
            return;
         }
     }
     else
     {
         SerialConnected = false;
         UHV4INT->SerialDisconnect();
         ui->RS485ConnectButton->setText("Connect");
#if _DEBUG
          qDebug() << "Serial port disconneted";
#endif
     }
 }

}

void MainWindow::on_StationButton_1_clicked()
{
    emit StationClicked(1);
}

void MainWindow::on_StationButton_2_clicked()
{
    emit StationClicked(2);
}

void MainWindow::on_StationButton_3_clicked()
{
    emit StationClicked(3);
}

void MainWindow::on_StationButton_4_clicked()
{
    emit StationClicked(4);
}





void MainWindow::on_ScanSDCSButton_clicked()
{
    CANSDCSINT->ScanforAvailableSDCS();
}

void MainWindow::on_CloseButton_clicked()
{
    this->close();
}

void MainWindow::on_StationButton_5_clicked()
{
    emit StationClicked(2);
}

void MainWindow::on_sendTestEmailButton_clicked()
{
   QEventLoop loop;
   QTimer atimer;
   connect(&atimer,SIGNAL(timeout()),&loop,SLOT(quit()));
   connect(this,SIGNAL(SIG_EmailSent()),&loop,SLOT(quit()));

  emit SIG_sendTestEmail();
   atimer.start(5000);
   loop.exec();
}

void MainWindow::SLOT_sendTestEmail()
{
    try
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
      //message.addRecipient(new EmailAddress("tuan.q.nguyen@kla-tencor.com", "Tuan Nguyen"));
       message.setSubject("Test");

       // Now add some text to the email.
       // First we create a MimeText object.

       MimeText text;

       text.setText("Test Email");

       // Now add it to the mail

       message.addPart(&text);

       // Now we can send the mail

       smtp.connectToHost();
       smtp.login();
       smtp.sendMail(message);
       smtp.quit();
    }
    catch(QByteArray error)
    {
        qDebug() << error;

    }
     emit SIG_EmailSent();

}

void MainWindow::on_checkBox_stateChanged(int arg1)
{

}

void MainWindow::on_checkBox_toggled(bool checked)
{
    if(checked)
    {
        sendEmail = true;
    }
    else
    {
        sendEmail = false;
    }
}
