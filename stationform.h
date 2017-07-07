#ifndef STATIONFORM_H
#define STATIONFORM_H

#include <QWidget>
#include "uhv4receivemessage.h"

namespace Ui {
class StationForm;
}

class StationForm : public QWidget
{
    Q_OBJECT

public:
    explicit StationForm(QWidget *parent = 0,  int Station =0);
    ~StationForm();

    double convertToDouble(QByteArray input);



    FRUSTATION aStation;
    void updateStationInfotoScreen();
    void setPumpConfiguration(int addr, int CH);
    PressureDataStation aPressureDataStation;
    CurrentDataStation aCurrentDataStation;
    VoltageDataStation aVoltageDataStation;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_HVONButton_clicked();

    void on_HVOFFButton_clicked();

public slots:
    void plotCurrent(QByteArray addr, int CH, QByteArray current);
    void plotPressure(QByteArray addr, int CH, QByteArray pressure);
    void plotVoltage(QByteArray addr, int CH, int voltage);
    void InformCommandToPumpIsNotExecuted(int pumpaddr);
    void updateSDCSdatatoScreen(SDCS aSDCS);
signals:
    //void requestData(UHV4GetDataRequest);
    void SIG_SF_StationInfo(FRUSTATION);
    void SIG_SF_turnHVON(FRUSTATION);
    void SIG_SF_turnHVOFF(FRUSTATION);
    void SIG_SF_turnRoughVACON(FRUSTATION);
    void SIG_SF_turnRoughVACOFF(FRUSTATION);
//    void SIG_SF_saveCurrentData(CurrentDataStation);
//    void SIG_SF_saveVoltageData(VoltageDataStation);
//    void SIG_SF_savePressureData(PressureDataStation);
private:
    Ui::StationForm *ui;
    void InitGraph();
    int StationNumber;
    void updateStationInfo();




};

#endif // STATIONFORM_H
