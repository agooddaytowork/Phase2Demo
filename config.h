#ifndef CONFIG_H
#define CONFIG_H

#define _DEBUG (0)
#define DATAUPDATEINTERVAL 5000
#define RFIDUPDATEINTERVAL 10000
#define TIMER_TIMEOUT_SERIAL 100
#define GET_DATA_TIMEOUT 500
#define EMAILSENDTIMEOUT 15000
#define PRESSURE_UPPER 10e-8
#define PRESSURE_LOWER 10e-12
#define VOLTAGE_UPPER 8000
#define VOLTAGE_LOWER 0
#define CURRENT_UPPER 10e-8
#define CURRENT_LOWER 10e-12
#define TIMEAXIS_TICKCOUNT 5

#define TIMEAXIS_AUTO_RANGE 300
#define DATASAMPLETIME (DATAUPDATEINTERVAL/1000)
#define PRESSURE_GLOBAL_SPEC_UPPER_LIMIT (2.0*pow10(-9))
#define PRESSURE_GLOBAL_SPEC_WARNING_LIMIT (1.0*pow10(-9))
#define MAXIMUMDATAPOINT 50


#define MAXSAMPLETOSENDMAIL  10
#endif // CONFIG_H
