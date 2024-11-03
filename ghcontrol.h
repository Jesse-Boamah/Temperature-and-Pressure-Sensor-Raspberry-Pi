/** @brief Gh control constants, structures, function prototypes
*   @file ghcontrol.h
*/

#ifndef GHCONTROL_H
#define GHCONTROL_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sensehat.h"


// Constants
#define GHUPDATE 2000
#define SENSORS 3
#define TEMPERATURE 0
#define HUMIDITY 1
#define PRESSURE 2

#define SEARCHSTR "serial\t\t:"
#define SYSINFOBUFSZ 512

#define SIMULATE 0
#define USTEMP 50
#define LSTEMP -10
#define USHUMID 100
#define LSHUMID 0
#define USPRESS 1016
#define LSPRESS 975

#define STEMP 25.0
#define SHUMID 55.0

#define ON 1
#define OFF 0

#define CTIMESTRSZ 25

#define NUMBARS 8
#define NUMPTS 8.0
#define TBAR 7
#define HBAR 5
#define PBAR 3
#define SENSEHAT 1

#define NALARMS 7
#define ALARMNMSZ 18

// Constants for alarm limits
#define LOWERATEMP 10
#define UPPERATEMP 30
#define LOWERAHUMID 25
#define UPPERAHUMID 70
#define LOWERAPRESS 985
#define UPPERAPRESS 1016

// Enumerated Types
typedef enum { NOALARM, HTEMP, LTEMP, HHUMID, LHUMID, HPRESS, LPRESS } alarm_e;


// Simulation control constants
#define SIMTEMPERATURE 0
#define SIMHUMIDITY 0
#define SIMPRESSURE 0


// Structures

typedef struct readings
{
    time_t rtime;
    float temperature;
    float humidity;
    float pressure;
}reading_s;


typedef struct setpoints
{
    float temperature;
    float humidity;
}setpoint_s;

typedef struct controls
{
    int heater;
    int humidifier;
}control_s;

typedef struct alarmlimits
{
    float hight;
    float lowt;
    float highh;
    float lowh;
    float highp;
    float lowp;
} alarmlimit_s;

typedef struct alarms
{
    alarm_e code;
    time_t atime;
    float value;
} alarm_s;




// Function Prototypes
///@cond INTERNAL
u_int64_t GhGetSerial(void);
void GhControllerInit(void);
void GhDisplayControls(control_s ctrl);
void GhDisplayReadings(reading_s rdata);
void GhDisplayTargets(setpoint_s spts);
control_s GhSetControls(setpoint_s target, reading_s rdata);
setpoint_s GhSetTargets(void);
void GhDisplayHeader(const char * sname);
int GhGetRandom(int range);
void GhDelay(int milliseconds);
float GhGetHumidity(void);
float GhGetPressure(void);
float GhGetTemperature(void);
reading_s GhGetReadings(void);
int GhLogData(char * fname, reading_s ghdata);  
int GhSaveSetpoints(char * fname, setpoint_s spts); 
setpoint_s GhRetrieveSetpoints(char * fname); 
int GhSetVerticalBar(int bar, COLOR_SENSEHAT pxc, uint8_t value, struct fb_t *fb);
void GhDisplayAll(reading_s rd, setpoint_s sd);
alarmlimit_s GhSetAlarmLimits(void);
void GhSetAlarms(alarm_s calarm[NALARMS], alarmlimit_s alarmpt, reading_s rdata);
void GhDisplayAlarms(alarm_s calarm[NALARMS]);
///@endcond

#endif
