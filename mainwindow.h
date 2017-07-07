#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QIcon>
#include "uhv4serialinterface.h"
#include "uhv4receivemessage.h"
#include <QTimer>
#include <QList>
#include "canint.h"
#include <QMessageBox>
#include "email/SmtpMime"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void startGetdata();
private slots:
    void createStationWindow(int StationNumber);

    void on_RS485ConnectButton_clicked();

    void on_StationButton_1_clicked();

    void on_StationButton_2_clicked();

    void on_StationButton_3_clicked();

    void on_StationButton_4_clicked();

    void getData();
    void getRFIDData();

    void registStation(FRUSTATION aStation);
    void turnStationOn(FRUSTATION aStation);
    void turnStationOff(FRUSTATION aStation);
    void turnRoughVACOn(FRUSTATION aStation);
    void turnRoughVACOff(FRUSTATION aStation);
    void updateStationStatusBaseOnPressure(QByteArray,int,QByteArray);
    void setStationStatus(int StationNumber, QIcon theIcon);
    void SetRoughVACStatus(int StationNumber, QIcon theIcon);

    void registSDCS(SDCS aSDCS);
    void on_ScanSDCSButton_clicked();

    void on_CloseButton_clicked();
    void showNewSDCSonSreen(SDCS aSDCS);

    void savePressureData(QByteArray pumpAddr, int pumpCH, QByteArray pressure);
    void saveCurrentData(QByteArray pumpAddr, int pumpCH, QByteArray current);
    void saveVoltageData(QByteArray pumpAddr, int pumpCH, int voltage);


    void on_StationButton_5_clicked();

    // Email Function
    void SLOT_sendAlertEmail(int StationNumber, QByteArray Pressure);
    void SLOT_sendTestEmail();
    void on_sendTestEmailButton_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_toggled(bool checked);

signals:
    void StationClicked(int Stationnumber);
    void newDatafromSDCS(SDCS aSDCS);
    void SIG_sendAlertEmail(int StationNumber, QByteArray Pressure);
    void SIG_EmailSent();
    void SIG_sendTestEmail();
private:

    void initMainwindow();

    Ui::MainWindow *ui;

    QPixmap StationOnPixMap;
    QPixmap StationOffPixMap;
    QPixmap StaionNoPixMap;
    QPixmap StationWarningPixMap;

    QIcon StationOnIcon;
    QIcon StationoffIcon;
    QIcon StationNoIcon;
    QIcon StationWarningIcon;


    UHV4SerialInterface *UHV4INT = new UHV4SerialInterface(this);
    CANINT *CANSDCSINT = new CANINT(this);

    bool SerialConnected=false;
    QList<FRUSTATION> StationList;
    QList<SDCS> SDCSList;
    QList<PressureDataStation> PressureDataStationList;
    QList<VoltageDataStation> VoltageDataStationList;
    QList<CurrentDataStation> CurrentDataStationList;

    QTimer getDatatimer;
    QTimer getRFIDtimer;
    double convertPressureToDouble(QByteArray input);
    bool sendEmail = false;

};

#endif // MAINWINDOW_H
