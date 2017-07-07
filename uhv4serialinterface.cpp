#include "uhv4serialinterface.h"
#include <QSerialPortInfo>
#include <QEventLoop>
#include <QObject>


UHV4SerialInterface::UHV4SerialInterface(QObject *parent)
{
    serial.setReadBufferSize(64);
    // set default Baudrate = 9600;
    serial.setBaudRate(38400);

    connect(&serial, SIGNAL(readyRead()), this, SLOT(SerialReceive()));
    connect(this, SIGNAL(messageReady(UHV4ReceiveMessage)), this,
            SLOT(MessageParser(UHV4ReceiveMessage)));
    //connect(this, SIGNAL(SIG_UHV4_NACK(int)), this, SLOT(S_UHV4_Send_CheckError_command(int)));
    connect(this, SIGNAL(SIG_UHV4_NACK(int)), this, SLOT(setFlagNACK(int)));

    connect(&serial,SIGNAL(error(QIODevice::write())),this,SLOT(UHV4_Serial_Error_Handler(QSerialPort::SerialPortError)));
}

UHV4SerialInterface::~UHV4SerialInterface()
{
    serial.close();
}

bool UHV4SerialInterface::SerialConfig(const QString &PortName, const int &Baudrate)
{
    this->_PortName = PortName;
    serial.setPortName(PortName);

    this->_Baudrate = Baudrate;
    //serial.setBaudRate(this->_Baudrate);

    if (!serial.open(QIODevice::ReadWrite)) {
#if _DEBUG
        qDebug() << "can not open port " << serial.portName();
#endif

        return false;
    }
    return true;
}

void UHV4SerialInterface::SerialDisconnect()
{
    serial.close();
}

void UHV4SerialInterface::ReadCommand(int ADDR, QByteArray WIN)
{
    QByteArray Message;
    int CRC;
    //QByteArray S_WIN;

    //S_WIN.append(QString::number(WIN));

    /* Compile the message */
    /* Message Format <STX><ADDR><WIN><READCOMMAND><NODATA><ETX><CRC> */
    Message.append(STX);
    Message.append((ADDR_HEADER + ADDR));
    Message.append(WIN);
    Message.append(READ_COMMAND);
    Message.append(ETX);

    CRC = (int)((ADDR_HEADER + ADDR) ^ READ_COMMAND ^ WIN[0] ^ WIN[1] ^ WIN[2] ^ ETX);
#if _DEBUG
    qDebug() << "Sending read message:" << Message.toHex();
#endif
    Message.append(QString::number(CRC, 16).toUpper());

    /* Send the message */
    serial.write(Message);
    Busy = true;
}

void UHV4SerialInterface::WriteCommand(int ADDR, QByteArray WIN, QByteArray DATA)
{
    QByteArray Message;
    int CRC;
    //QByteArray S_WIN;

    //S_WIN.append(QString::number(WIN));

    /* Compile the message */
    /* Message Format <STX><ADDR><WIN><READCOMMAND><NODATA><ETX><CRC> */
    Message.append(STX);
    Message.append((ADDR_HEADER + ADDR));
    Message.append(WIN);
    Message.append(WRITE_COMMAND);
    Message.append(DATA);
    Message.append(ETX);

    CRC = (int)((ADDR_HEADER + ADDR) ^ WRITE_COMMAND ^ WIN[0] ^ WIN[1] ^ WIN[2] ^ ETX);

    for (int i = 0; i < DATA.size(); i++) {
        CRC = CRC ^ DATA[i];
    }

    Message.append(QString::number(CRC, 16).toUpper());

#if _DEBUG
    qDebug() << "sending write message:" << Message.toHex();
#endif

    /* Send the message */
    //while (serial.isReadable());
    this->blockSignals(true);
    serial.write(Message);
    this->blockSignals(false);
}

/******
 * Serial recieve

   -> read the serial string
   -> for loop, looking for the STX 0x02 (head) and ETX 0x03 (tail)
   -> assign STX address to head
   -> assign ETX address to tail

   -> Check whether there are data after ETX For CRC data

   -> Do subtraction to get the length between Head  and tail . Will run into 3 case
    -> lenght = 6  : ACK,NACK, UNKNOWN WINDOW, DATA TYPE ERROR, Win disable <STX><ADDR><FLAG><ETX>
    -> lenght = 12  : Read instruction of numeric window <STX><ADDR><WIN><COM><DATA><ETX> ->> <ADDR><WIN><COM><DATA> = 11 bytes
    -> lenght = 16 : read instruction of alphanumeric window <STX><ADDR><WIN><COM><DATA><ETX> ->> <ADDR><WIN><COM><DATA> = 15 bytes

   -> if lenght = 6 : return the status flag by emiting signal correspond to the status
   -> if lenght = 12 : return a UHV4receivemessage, emitting signal of corresponded command
   -> if lenght = 16 : return a UHV4receivemessage, emitting signal of corresponded command

   -> clear the serial string

 *****/
void UHV4SerialInterface::SerialReceive()
{
    UHV4ReceiveMessage TempMSG;
    static int head = 0;
    static int tail = 0;

    ReceiveData += serial.readAll();


    QString S_WIN;
    static bool hitTail = false;

    for (int i = 0; i < ReceiveData.size(); i++) {

        if (ReceiveData.at(i) == 0x02) {
            head = i;
        }
        if (ReceiveData.at(i) == 0x03) {
            tail = i;
            hitTail = true;
        }

        if (hitTail == true && (ReceiveData.size() - i) >= 2) {
            hitTail = false;

            int DataLenght = tail - head;

            switch (DataLenght) {
            case 3:

                // ACK 0x06
                if ( ReceiveData.at(head + 2) == 0x06) {

                    emit SIG_UHV4_ACK(0);
                    ReceiveData.clear();
                    return;
                }

                // NACK 0x15
                if (ReceiveData.at(head + 2) == 0x15) {
                    //getDatadisable = true;
                    emit SIG_UHV4_NACK(0);
                    ReceiveData.clear();
                    return;
                }
#if _DEBUG
                // UNKNOWN WINDOW 0x32
                if (ReceiveData.at(head + 2) == 0x32) {
                    qDebug() << "Unknow Window";
                    ReceiveData.clear();
                    return;
                }
                //DataTypeError 0x33
                if (ReceiveData.at(head + 2) == 0x33) {
                    qDebug() << "Data Type Error";
                    ReceiveData.clear();
                    return;
                }
#endif


                break;

            case 12:
            case 16:

                int CRC;

                TempMSG.addr.append(ReceiveData.at(head + 1));   // head +1
                S_WIN.append(ReceiveData.at(head + 2));   // head +2
                S_WIN.append(ReceiveData.at(head + 3));   // head + 3
                S_WIN.append(ReceiveData.at(head + 4));   // head + 4
                TempMSG.win = S_WIN.toInt();
                TempMSG.com.append(ReceiveData.at(head + 5));   // head + 5
//                TempMSG.CRC.append(ReceiveData.at(tail + 1));
//                TempMSG.CRC.append(ReceiveData.at(tail + 2));

                /* Calculate sCRC */

//                CRC = (int)((TempMSG.addr.at(0)) ^ TempMSG.com.at(0) ^ ReceiveData.at(
//                                head + 2)^ ReceiveData.at(head + 3)^ ReceiveData.at(head + 4) ^ ETX);

//                CRC = ((ADDR_HEADER) ^ TempMSG.com.at(0) ^ ReceiveData.at(
//                           head + 2)^ ReceiveData.at(head + 3)^ ReceiveData.at(head + 4) ^ ETX);
                /* get data */

                while (((unsigned int)(tail - (head + 5))) > 1) {
                    head++;
                    TempMSG.data.append(ReceiveData.at(head + 5));
                    CRC = (CRC ^ ReceiveData.at(head + 5));
                }

                // convert int to QByteArray ASCII format
//                QString sCRC;
//                sCRC.append(QString::number(CRC, 16).toUpper());

//                QString aCRC;
//                aCRC.append(TempMSG.CRC);

//                // Compare sCRC and Tempmessage CRC to set the valid status
//                if ((QString::compare(sCRC, aCRC, Qt::CaseSensitive)) == 0) {
//                    TempMSG.valid = true;
//                }

                /**** DEBUGG!!!!! */

                TempMSG.valid = true;

                /*** Send Debug Data of Receive messages to Console ****/
#if _DEBUG
                qDebug() << "__________________________________________";
                qDebug() << "Received Message!";
                qDebug() << "addr: " << TempMSG.addr.toHex();
                qDebug() << "WIN: " << TempMSG.win;
                qDebug() << "Data: " << TempMSG.data.toHex();
                qDebug() << "Data: " << TempMSG.data;
                qDebug() << "Valid: " << TempMSG.valid;
#endif


                // Emit signal to parse data from message
                emit messageReady(TempMSG);

                //* Set BUSY !!!!!!!!!! REALLY IMPORTANT HERE

                Busy = false;
                // Return to main program
                ReceiveData.clear();
                break;
                return;

            }
        }




    }

    //* Set BUSY !!!!!!!!!! REALLY IMPORTANT HERE

    Busy = false;

    //emit signal to parse TempMSG;
}

/***** MESSAGE PARSER *****/
/* Check if message is valid
 * if valid, then open a switch cases base on the Window command Integer
 * if message runs into a certain cases, emit signal to handle that case,
 * along with the address of the controller
 * then return */

void UHV4SerialInterface::MessageParser(UHV4ReceiveMessage Message)
{
    if (Message.valid) {

        switch (Message.win) {

        case UHV4_PRESSURE_MEASURE_CH1_COMMAND:
            emit SIG_UHV4_PRESSURE(Message.addr, 1, Message.data);
            break;
        case UHV4_PRESSURE_MEASURE_CH2_COMMAND:
            emit SIG_UHV4_PRESSURE(Message.addr, 2, Message.data);
            break;
        case UHV4_PRESSURE_MEASURE_CH3_COMMAND:
            emit SIG_UHV4_PRESSURE(Message.addr, 3, Message.data);
            break;
        case UHV4_PRESSURE_MEASURE_CH4_COMMAND:
            emit SIG_UHV4_PRESSURE(Message.addr, 4, Message.data);
            break;
        case UHV4_V_MEASURE_CH1_COMMAND:
            emit SIG_UHV4_VOLTAGE(Message.addr, 1, Message.data.toInt());
            break;
        case UHV4_V_MEASURE_CH2_COMMAND:
            emit SIG_UHV4_VOLTAGE(Message.addr, 2, Message.data.toInt());
            break;
        case UHV4_V_MEASURE_CH3_COMMAND:
            emit SIG_UHV4_VOLTAGE(Message.addr, 3, Message.data.toInt());
            break;
        case UHV4_V_MEASURE_CH4_COMMAND:
            emit SIG_UHV4_VOLTAGE(Message.addr, 4, Message.data.toInt());
            break;
        case UHV4_I_MEASURE_CH1_COMMAND:
            emit SIG_UHV4_CURRENT(Message.addr, 1, Message.data);
            break;
        case UHV4_I_MEASURE_CH2_COMMAND:
            emit SIG_UHV4_CURRENT(Message.addr, 2, Message.data);
            break;
        case UHV4_I_MEASURE_CH3_COMMAND:
            emit SIG_UHV4_CURRENT(Message.addr, 3, Message.data);
            break;
        case UHV4_I_MEASURE_CH4_COMMAND:
            emit SIG_UHV4_CURRENT(Message.addr, 4, Message.data);
            break;
        case UHV4_CONTROLLER_MODEL_COMMAND:
            emit SIG_UHV4_MODEL(Message.addr, Message.data);
            break;
        case UHV4_CONTROLLER_SERIAL_MODEL_COMMAND:
            emit SIG_UHV4_MODEL_SERIAL(Message.addr, Message.data);
            break;
        case UHV4_ERROR_COMMAND:
            S_UHV4_ErrorParser(0, currentChanel, Message.data.toInt());
            break;

        }
        Busy = false;
        //ReceiveData.clear();
        emit SIG_readyWrite();
    }
}

/** Turn the HV channels on or off
* Parameter:
* addr - address of the ion pump controller
* CH - Chanel number from 1 -> 4 ; if int = 0; turn on or OFF 4 channels
* status: true - HV ON; false - HV OFF */

void UHV4SerialInterface::S_UHV4_HV_ON_OFF(int addr, int CH, bool status)
{
    int i = 0;

    if (CH == 0 && status) {
        for (i = 1 ; i < 5; i++) {
            switch (i) {
            case 1:
                WriteCommand(addr, "011", "1");
                break;
            case 2:
                WriteCommand(addr, "012", "1");
                break;
            case 3:
                WriteCommand(addr, "013", "1");
                break;
            case 4:
                WriteCommand(addr, "014", "1");
                break;
            default:
                break;
            }
        }
    }

    else if (CH == 0 && !status) {
        for (i = 1 ; i < 5; i++) {
            switch (i) {
            case 1:
                WriteCommand(addr, "011", "0");
                break;
            case 2:
                WriteCommand(addr, "012", "0");
                break;
            case 3:
                WriteCommand(addr, "013", "0");
                break;
            case 4:
                WriteCommand(addr, "014", "0");
                break;
            default:
                break;
            }
        }
    } else if (status) {
        switch (CH) {
        case 1:
            WriteCommand(addr, "011", "1");
            break;
        case 2:
            WriteCommand(addr, "012", "1");
            break;
        case 3:
            WriteCommand(addr, "013", "1");
            break;
        case 4:
            WriteCommand(addr, "014", "1");
            break;
        default:
            break;
        }
    } else {
        switch (CH) {
        case 1:
            WriteCommand(addr, "011", "0");
            break;
        case 2:
            WriteCommand(addr, "012", "0");
            break;
        case 3:
            WriteCommand(addr, "013", "0");
            break;
        case 4:
            WriteCommand(addr, "014", "0");
            break;
        default:
            break;
        }
    }
}


/** Turn the HV channels on or off
* Parameter:
* addr - address of the ion pump controller
* CH - Chanel number from 1 -> 4
* status: true - PROTECT ON; PROTECT OFF */

void UHV4SerialInterface::S_UHV4_PROTECT_ON_OFF(int addr, int CH, bool status)
{
    //    int i = 0;

    //    if (CH == 0 && status) {
    //        for (i = 1 ; i < 5; i++) {
    //            WriteCommand(addr, (10 + i), "1");
    //        }
    //    } else if (CH == 0 && !status) {
    //        for (i = 1 ; i < 5; i++) {
    //            WriteCommand(addr, (10 + i), "0");
    //        }
    //    } else if (status) {
    //        WriteCommand(addr, (10 + CH), "1");
    //    } else {
    //        WriteCommand(addr, (10 + CH), "0");
    //    }

}


void UHV4SerialInterface::S_UHV4_read_Pressure(int addr, int CH)
{
    int i = 0;

    if (CH == 0) {
        for (i = 1 ; i < 5; i++) {
            switch (i) {
            case 1:
                ReadCommand(addr, "812");
                break;
            case 2:
                ReadCommand(addr, "822");
                break;
            case 3:
                ReadCommand(addr, "832");
                break;
            case 4:
                ReadCommand(addr, "842");
                break;
            default:
                break;
            }
        }
    }


    else {
        switch (CH) {
        case 1:
            ReadCommand(addr, "812");
            break;
        case 2:
            ReadCommand(addr, "822");
            break;
        case 3:
            ReadCommand(addr, "832");
            break;
        case 4:
            ReadCommand(addr, "842");
            break;
        default:
            break;
        }
    }

    Busy = true;
}

void UHV4SerialInterface::S_UHV4_read_Voltage(int addr, int CH)
{
    int i = 0;

    if (CH == 0) {
        for (i = 1 ; i < 5; i++) {
            switch (i) {
            case 1:
                ReadCommand(addr, "810");
                return;
            case 2:
                ReadCommand(addr, "820");
                return;
            case 3:
                ReadCommand(addr, "830");
                return;
            case 4:
                ReadCommand(addr, "840");
                return;
            default:
                return;
            }
        }
    }


    else {
        switch (CH) {
        case 1:
            ReadCommand(addr, "810");
            return;
        case 2:
            ReadCommand(addr, "820");
            return;
        case 3:
            ReadCommand(addr, "830");
            return;
        case 4:
            ReadCommand(addr, "840");
            return;
        default:
            return;
        }
    }
    // Busy = true;
}

void UHV4SerialInterface::S_UHV4_read_Current(int addr, int CH)
{
    int i = 0;

    if (CH == 0) {
        for (i = 1 ; i < 5; i++) {
            switch (i) {
            case 1:
                ReadCommand(addr, "811");
                break;
            case 2:
                ReadCommand(addr, "821");
                break;
            case 3:
                ReadCommand(addr, "831");
                break;
            case 4:
                ReadCommand(addr, "841");
                break;
            default:
                break;
            }
        }
    }

    else {
        switch (CH) {
        case 1:
            ReadCommand(addr, "811");
            break;
        case 2:
            ReadCommand(addr, "821");
            break;
        case 3:
            ReadCommand(addr, "831");
            break;
        case 4:
            ReadCommand(addr, "841");
            break;
        default:
            break;
        }
    }
    Busy = true;
}


/** Send Get Data Command to controller
* Wait for BUSY flag them continue
* Must add TIMEOUT HERE !!! */

void UHV4SerialInterface::S_UHV4_getData()
{
    QEventLoop loop;
    QTimer  timeOuttimer;
    QObject::connect(this, SIGNAL(SIG_readyWrite()), &loop, SLOT(quit()));
    timeOuttimer.singleShot(GET_DATA_TIMEOUT, &loop, SLOT(quit()));
    if (getDatadisable == false) {
        if (CH1_ON == true) {

            S_UHV4_read_Pressure(0, 1);

            loop.exec();

            S_UHV4_read_Current(0, 1);

            loop.exec();
            //  while (Busy);

            S_UHV4_read_Voltage(0, 1);

            loop.exec();
        }
        if (CH2_ON == true) {
            S_UHV4_read_Pressure(0, 2);

            loop.exec();

            S_UHV4_read_Current(0, 2);

            loop.exec();

            S_UHV4_read_Voltage(0, 2);

            loop.exec();
        }
        if (CH3_ON == true) {
            S_UHV4_read_Pressure(0, 3);

            loop.exec();
            S_UHV4_read_Current(0, 3);

            loop.exec();
            S_UHV4_read_Voltage(0, 3);

            loop.exec();
        }
        if (CH4_ON == true) {

            S_UHV4_read_Pressure(0, 4);

            loop.exec();

            S_UHV4_read_Current(0, 4);

            loop.exec();

            S_UHV4_read_Voltage(0, 4);

            loop.exec();
        }
    }

}

/*** send command to check error of all channels
 *  Report Parameter currentChanel so that the MessageParser
 * can determine the channel being checked
 * implement Qeventloop */

void UHV4SerialInterface::S_UHV4_Send_CheckError_command(int addr)
{
    // Stop the timer


    QEventLoop loop;
    // QTimer  timeOuttimer;
    QObject::connect(this, SIGNAL(SIG_UHV4_ErrorShowed()), &loop, SLOT(quit()));
    QObject::connect(this, SIGNAL(SIG_UHV4_ACK(int)), &loop, SLOT(quit()));
    // Check ERROR of channel 1


    disconnect(this, SIGNAL(SIG_UHV4_NACK(int)), 0, 0);

    if (currentChanel == 1) {
        WriteCommand(addr, "505", "000001");
        // timeOuttimer.singleShot(TIMER_TIMEOUT_SERIAL, &loop, SLOT(quit()));
        loop.exec();

        ReadCommand(addr, "206");
        //timeOuttimer.singleShot(TIMER_TIMEOUT_SERIAL, &loop, SLOT(quit()));
        loop.exec();
    } else if (currentChanel == 2)

    {
        // Check ERROR of channel 2

        WriteCommand(addr, "505", "000002");
        //timeOuttimer.singleShot(TIMER_TIMEOUT_SERIAL, &loop, SLOT(quit()));
        loop.exec();
        ReadCommand(addr, "206");
        //timeOuttimer.singleShot(TIMER_TIMEOUT_SERIAL, &loop, SLOT(quit()));
        loop.exec();
    } else if (currentChanel == 3) {
        // Check ERROR of channel 3

        WriteCommand(addr, "505", "000003");
        // timeOuttimer.singleShot(TIMER_TIMEOUT_SERIAL, &loop, SLOT(quit()));
        loop.exec();
        ReadCommand(addr, "206");
        //timeOuttimer.singleShot(TIMER_TIMEOUT_SERIAL, &loop, SLOT(quit()));
        loop.exec();
    } else if (currentChanel == 4) {
        //Check ERROR of channel 4

        WriteCommand(addr, "505", "000004");

        // timeOuttimer.singleShot(TIMER_TIMEOUT_SERIAL, &loop, SLOT(quit()));
        loop.exec();
        ReadCommand(addr, "206");
        // timeOuttimer.singleShot(TIMER_TIMEOUT_SERIAL, &loop, SLOT(quit()));
        loop.exec();
    }

    // reset current channel to avoid mistakes
    currentChanel = 0;
    // recover GET DATA FUNCTION
    getDatadisable = false;


}

/** BETTER COMMAND THIS FUNCTION REALLY GOOOODDDD!!!! */

void UHV4SerialInterface::S_UHV4_ErrorParser(int addr, int CH, int Message)
{

    UHV4ErrorMessage TempErrorMEssage;

    TempErrorMEssage.addr = addr;
    TempErrorMEssage.CH = CH;

    if ((Message & UHV4_ERR_IN_PW_HV)) {
        TempErrorMEssage.UHV4_PowerInput_HV_error = true;
    }
    if ((Message & UHV4_ERR_IN_PW_PFC)) {
        TempErrorMEssage.UHV4_PowerInput_PFC_error = true;
    }
    if ((Message & UHV4_ERR_INLOCK_CBL)) {
        TempErrorMEssage.UHV4_InterLock_error = true;
    }
    if ((Message & UHV4_ERR_OVER_TMP_HV)) {
        TempErrorMEssage.UHV4_OverTemp_HV_error = true;
    }
    if ((Message & UHV4_ERR_PROTECT)) {
        TempErrorMEssage.UHV4_Protect_error = true;
    }
    if ((Message & UHV4_ERR_SHORT_CIRCUIT)) {
        TempErrorMEssage.UHV4_Short_circuit_error = true;
    }
    if ((Message & UHV4_ERR_HV_OUT)) {
        TempErrorMEssage.UHV4_HV_out_error = true;
    }
    if ((Message & UHV4_ERR_HV_DISABLE)) {
        TempErrorMEssage.UHV4_HV_disable_error = true;
    }

    emit SIG_UHV4_ErrorReadyShow(TempErrorMEssage);

#if _DEBUG

    if (CH != 0) {
        qDebug() << "__________________________________________";
        qDebug() << "Erro ON!";
        qDebug() << "Pump Controller addr: " << TempErrorMEssage.addr;
        qDebug() << "channel: " << TempErrorMEssage.CH;
        qDebug() << "INTERLOCK_ERR: " << TempErrorMEssage.UHV4_InterLock_error;
        qDebug() << "ProTect_ERR: " << TempErrorMEssage.UHV4_Protect_error;
    }



#endif
    connect(this, SIGNAL(SIG_UHV4_NACK(int)), this, SLOT(S_UHV4_Send_CheckError_command(int)));
}

// handle NACK emit signal
// set a global flag UHV4_NACK to true;
// function that use global flag to set to false after checking FLAG status

void UHV4SerialInterface::setFlagNACK(int currentaddr)
{
    currentPumpaddr = currentaddr;
    UHV4_FLAG_NACK = true;
}


/** Handle Serial Port Error
 * By emit another signal to the GUI */

void UHV4SerialInterface::UHV4_Serial_Error_Handler(QSerialPort::SerialPortError Error)
{

    emit SIG_UHV4_SerialError();
}
