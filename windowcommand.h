#ifndef WINDOWCOMMAND_H
#define WINDOWCOMMAND_H

#define STX             0x02
#define ADDR_HEADER     0x80
#define READ_COMMAND    0x30
#define WRITE_COMMAND   0x31
#define ETX             0x03

/* Define WINDOW COMMANDS */


/* TURN ON OFF CHANNELS COMMAND */
#define UHV4_CH1_HV_ONOFF_COMMAND               11
#define UHV4_CH2_HV_ONOFF_COMMAND               12
#define UHV4_CH3_HV_ONOFF_COMMAND               13
#define UHV4_CH4_HV_ONOFF_COMMAND               14

/* SET BAUDRATE OF SERIAL PROTOCOL*/
#define SERIAL_BAUDRATE_COMMAND                 108

/* CONTROLLER INFORMATION */
#define UHV4_STATUS_COMMAND                     205
#define UHV4_ERROR_COMMAND                      206

#define UHV4_CONTROLLER_MODEL_COMMAND           319
#define UHV4_CONTROLLER_SERIAL_MODEL_COMMAND    323

/* SET ADDRESS OF CONTROLLER FOR RS485*/

#define UHV4_SET_SERIALTYPE_COMMAND             504
#define UHV4_CHANNEL_SELECT_COMMAND             505

/* PUMP COMMANDS */
#define UHV4_SET_PRESSURE_UNIT_COMMAND  600
#define UHV4_SET_OPERATINGMODE                  601
#define UHV4_PROTECT_COMMAND                    602
#define UHV4_FIXED_STEP_COMMAND                 603

#define UHV4_DEVICE_NUMBER_CH1_COMMAND          610
#define UHV4_POWERMAX_CH1_COMMAND               612
#define UHV4_V_TARGET_CH1_COMMAND               613
#define UHV4_I_PROTECT_CH1_COMMAND              614
#define UHV4_SETPOINT_CH1_COMMAND               615

#define UHV4_DEVICE_NUMBER_CH2_COMMAND          620
#define UHV4_POWERMAX_CH2_COMMAND               622
#define UHV4_V_TARGET_CH2_COMMAND               623
#define UHV4_I_PROTECT_CH2_COMMAND              624
#define UHV4_SETPOINT_CH2_COMMAND               625

#define UHV4_DEVICE_NUMBER_CH3_COMMAND          630
#define UHV4_POWERMAX_CH3_COMMAND               632
#define UHV4_V_TARGET_CH3_COMMAND               633
#define UHV4_I_PROTECT_CH3_COMMAND              634
#define UHV4_SETPOINT_CH3_COMMAND               635

#define UHV4_DEVICE_NUMBER_CH4_COMMAND          640
#define UHV4_POWERMAX_CH4_COMMAND               642
#define UHV4_V_TARGET_CH4_COMMAND               643
#define UHV4_I_PROTECT_CH4_COMMAND              644
#define UHV4_SETPOINT_CH4_COMMAND               645

/** READ DATA COMMAND **/

#define UHV4_TEMP_FAN_COMMAND                   800
#define UHV4_TEMP_HV1_COMMAND                   801
#define UHV4_TEMP_HV2_COMMAND                   802
#define UHV4_TEMP_HV3_COMMAND                   808
#define UHV4_TEMP_HV4_COMMAND                   809

#define UHV4_INTERLOCK_STATUS                   803
#define UHV4_STATUS_SETPOINT_COMMAND            804

#define UHV4_V_MEASURE_CH1_COMMAND              810
#define UHV4_I_MEASURE_CH1_COMMAND              811
#define UHV4_PRESSURE_MEASURE_CH1_COMMAND       812


#define UHV4_V_MEASURE_CH2_COMMAND              820
#define UHV4_I_MEASURE_CH2_COMMAND              821
#define UHV4_PRESSURE_MEASURE_CH2_COMMAND       822

#define UHV4_V_MEASURE_CH3_COMMAND              830
#define UHV4_I_MEASURE_CH3_COMMAND              831
#define UHV4_PRESSURE_MEASURE_CH3_COMMAND       832

#define UHV4_V_MEASURE_CH4_COMMAND              840
#define UHV4_I_MEASURE_CH4_COMMAND              841
#define UHV4_PRESSURE_MEASURE_CH4_COMMAND       842

/** ERROR FLAG FOR SINGLE CHANNEL
 * NEED TO CONSULT THE WINDOW COMMAND 505 to
 * select the channel first before reading this*/

#define UHV4_ERR_FAN            1
#define UHV4_ERR_IN_PW_HV       2
#define UHV4_ERR_IN_PW_PFC      4
#define UHV4_ERR_OVER_TMP_PFC   8
#define UHV4_ERR_COMM_CPU_HV    16
#define UHV4_ERR_INLOCK_CBL     32
#define UHV4_ERR_OVER_TMP_HV    64
#define UHV4_ERR_PROTECT        128
#define UHV4_ERR_MEASURE        256
#define UHV4_ERR_HV_OUT         512
#define UHV4_ERR_SHORT_CIRCUIT  1024
#define UHV4_ERR_HV_DISABLE     2048


#endif // WINDOWCOMMAND_H
