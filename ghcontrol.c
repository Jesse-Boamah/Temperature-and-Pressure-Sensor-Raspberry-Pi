/** @brief Gh control functions
 *  @file ghcontrol.c
 */
#include"ghcontrol.h"
#include<time.h>
#include <string.h>
#include "sensehat.h"

#if SENSEHAT
    SenseHat Sh;
#endif


const char alarmnames[NALARMS][ALARMNMSZ] = { 
    "No Alarms", "High Temperature", "Low Temperature", 
    "High Humidity", "Low Humidity", "HighPressure", 
    "Low Pressure"
};


/**@brief Displays the header with the student's name.
 * @param sname Name of the student
 */
void GhDisplayHeader(const char * sname)
{
	fprintf(stdout, "%s's CENG153 Greenhouse Controller\n", sname);
}

/**@param Generates a random number within the specified range.
 * @param range The upper limit of the random number range
 * @return A random number within the specified range
 */
int GhGetRandom(int range)
{
	return rand() % range;
}

/**@brief Busy-waits for a delay in milliseconds.
 * @param milliseconds The delay time (ms).
 */
void GhDelay(int milliseconds) 
{
    long wait;
    clock_t now, start;

    wait = milliseconds * (CLOCKS_PER_SEC / 1000);
    start = clock();
    now = start;

    while ((now - start) < wait) {
        now = clock();
    }
}


/**@brief Initializes the greenhouse controller
 */
void GhControllerInit(void)
{
    srand((unsigned) time(NULL));	
    GhDisplayHeader("\nJesse Boamah");
}

/**@brief Displays the current control settings.
 * @param ctrl The controls data structure
 */
void GhDisplayControls(control_s ctrl) 
{
    fprintf(stdout, " Controls\tHeater: %d\tHumidifier:%d\n", ctrl.heater, ctrl.humidifier);
}


/**@brief Displays the target values for temperatur and humidity
 *@param spts the setpoints data structure
 *@return
 */
void GhDisplayTargets(setpoint_s spts) {
    fprintf(stdout, " Setpoints\tT: %5.1fC\tH: %5.1f%%\n", spts.temperature, spts.humidity);
}


/**@brief Displays the current sensor readings.
 * @param rdata The readings data structure
 */
void GhDisplayReadings(reading_s rdata)
{
    fprintf(stdout, "\n%s Readings\tT: %5.1fC\tH: %5.1f%%\tP: %6.1f mB\n",
            ctime(&rdata.rtime), rdata.temperature, rdata.humidity,
            rdata.pressure);
}

/**@brief Sets the controls settings.
 * @param target The setpoints data structure
 * @param rdata The readings data structure
 * @return The controls data structure
 */
control_s GhSetControls(setpoint_s target, reading_s rdata)
{
    control_s cset;
    if (rdata.temperature < target.temperature)
    {
        cset.heater = ON;
    }
    else
    {
        cset.heater = OFF;
    }
    if (rdata.humidity < target.humidity)
    {
        cset.humidifier = ON;
    }
    else
    {
        cset.humidifier = OFF;
    }
    return cset;
}

/**@brief Sets the target values
 * @return The setpoints data structure
 */
setpoint_s GhSetTargets(void) {
    setpoint_s cpoints;

    cpoints = GhRetrieveSetpoints("setpoints.dat");
    if (cpoints.temperature == 0.0) {
        cpoints.temperature = STEMP;
        cpoints.humidity = SHUMID;
        GhSaveSetpoints("setpoints.dat", cpoints);
    }

    return cpoints;
}
/**@brief Gets the current humidity value.
 * @return The humidity value
 */
float GhGetHumidity(void) 
{
#if SIMHUMIDITY
     return (GhGetRandom(USHUMID - LSHUMID + 1) + LSHUMID);
#else
     return Sh.GetHumidity();
#endif
}

/**@brief Gets the current pressure value.
 * @return The pressure value
 */
float GhGetPressure(void)
{
#if SIMPRESSURE
      return (GhGetRandom(USPRESS - LSPRESS + 1) + LSPRESS);
#else
      return Sh.GetPressure();
#endif
}
/**@brief Gets the current temperature value.
 * @return The temperature value
 */
float GhGetTemperature(void)
{
#if SIMTEMPERATURE
      return (GhGetRandom(USTEMP - LSTEMP + 1) + LSTEMP);
#else
      return Sh.GetTemperature();
#endif
}

/**@brief Gets the current sensor readings.
 * @return The readings data structure
 */
reading_s GhGetReadings(void)
{
    reading_s now = {0};
    now.rtime = time(NULL);
    now.temperature = GhGetTemperature();
    now.humidity = GhGetHumidity();
    now.pressure = GhGetPressure();
    return now;
}

/**@brief Logs the readings data to a file.
 * @param fname The name of the file
 * @param ghdata The readings data structure
 * @return 1 if successful, 0 if not
 */
int GhLogData(char * fname, reading_s ghdata)
{
    FILE *fp;
    char ltime[CTIMESTRSZ];

    fp = fopen(fname, "a");
    if (fp == NULL)
    {
        return 0;
    }

    strcpy(ltime, ctime(&ghdata.rtime));
    ltime[3] = ',';
    ltime[7] = ',';
    ltime[10] = ',';
    ltime[19] = ',';

    fprintf(fp, "\n%.24s,%5.1lf,%5.1lf,%6.1lf",
            ltime, ghdata.temperature, ghdata.humidity,
            ghdata.pressure);

    fclose(fp);
    return 1;
}


/**@brief Saves the setpoints data to a file.
 * @param fname The name of the file
 * @param spts The setpoints data structure
 * @return 1 if successful, 0 if not
 */
int GhSaveSetpoints(char * fname, setpoint_s spts)
{
    FILE *fp;
    fp = fopen(fname, "w"); 
    if (fp == NULL)
    {
        return 0;  
    }

    fwrite(&spts, sizeof(setpoint_s), 1, fp);  
    fclose(fp); 
    return 1;  
}

/**@brief Retrieves the setpoints data from a file.
 * @param fname The name of the file
 * @return The setpoints data structure
 */
setpoint_s GhRetrieveSetpoints(char * fname)
{
    setpoint_s spts = {0.0, 0.0}; 
    FILE *fp;
    fp = fopen(fname, "r");  
    if (fp == NULL)
    {
        return spts;  
    }

    fread(&spts, sizeof(setpoint_s), 1, fp);
    fclose(fp);
    return spts;
}

/**@brief Retrieves the serial number of the device.
 * @return The serial number
 */
uint64_t GhGetSerial(void)
{
	static uint64_t serial = 0;
	FILE * fp;
	char buf[SYSINFOBUFSZ];
	char searchstring[] = SEARCHSTR;
	fp = fopen ("/proc/cpuinfo", "r");
	if (fp != NULL)
	{
		while (fgets(buf, sizeof(buf), fp) != NULL)
		{
			if (!strncasecmp(searchstring, buf, strlen(searchstring)))
			{
				sscanf(buf+strlen(searchstring), "%Lx", &serial);
			}
		}
		fclose(fp);
	}
     if(serial==0)
     {
         system("uname -a");
         system("ls --fu /usr/lib/codeblocks | grep -Po '\\.\\K[^ ]+'>stamp.txt");
         fp = fopen ("stamp.txt", "r");
         if (fp != NULL)
         {
             while (fgets(buf, sizeof(buf), fp) != NULL)
             {
                sscanf(buf, "%Lx", &serial);
             }
             fclose(fp);
        }
     }
	return serial;
}


/** @brief Sets a vertical bar of pixels on a Sense 
 * 
 * @param bar The vertical bar (0-7).
 * @param pxc The color for the pixels.
 * @param value The height of the bar (0-7).
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on invalid input.
 */
int ShSetVerticalBar(int bar, COLOR_SENSEHAT pxc, uint8_t value) {
    int i;

    
    if (value > 7) {
        value = 7;
    }

    if (bar < 0 || bar > 7 || value < 0 || value > 7) {
        return EXIT_FAILURE;
    }

    for (i = 0; i <= value; i++) {
        Sh.LightPixel(i, bar, pxc);
    }

    // Set pixels in the bar
    for (i = value + 1; i < 8; i++) {
        Sh.LightPixel(i, bar, BLACK);
    }

    // When done looping, return EXIT_SUCCESS
    return EXIT_SUCCESS;
}

/** @brief Sets a vertical bar of pixels on a Sense HAT.
 * @param bar The vertical bar (0-7).
 * @param pxc The color for the pixels.
 * @param value The height of the bar (0-7).
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on invalid input.
 */
void GhDisplayAll(reading_s rd, setpoint_s sd) {
    int rv, sv, avh, avl;
    COLOR_SENSEHAT pxc;

    // Clear the dis
    Sh.WipeScreen(BLACK);

    // Scale the temperature value
    rv = (NUMPTS * (((rd.temperature - LSTEMP) / (USTEMP - LSTEMP)) + 0.05)) - 1;
 
    ShSetVerticalBar(TBAR, GREEN, rv);

    // Scale the humidity value
    rv = (NUMPTS * (((rd.humidity - LSHUMID) / (USHUMID - LSHUMID)) + 0.05)) - 1;
    
    ShSetVerticalBar(HBAR, GREEN, rv);

    // Scale the pressure value
    rv = (NUMPTS * (((rd.pressure - LSPRESS) / (USPRESS - LSPRESS)) + 0.05)) - 1;
    
    ShSetVerticalBar(PBAR, GREEN, rv);

    // Display the temperature setpoint
    sv = (NUMPTS * (((sd.temperature - LSTEMP) / (USTEMP - LSTEMP)) + 0.05)) - 1;
   
    Sh.LightPixel(sv, TBAR, MAGENTA);

    // Display the humidity setpoint
    sv = (8.0* (((sd.humidity - LSHUMID) / (USHUMID - LSHUMID)) + 0.05)) - 1;
    
    Sh.LightPixel(sv, HBAR, MAGENTA);
}

/** 
 * @brief Sets the alarm limits for temperature, humidity, and pressure.
 * @return The alarm limits data structure
 */
alarmlimit_s GhSetAlarmLimits(void) {
    alarmlimit_s calarm;
    calarm.hight = UPPERATEMP;
    calarm.lowt = LOWERATEMP;
    calarm.highh = UPPERAHUMID;
    calarm.lowh = LOWERAHUMID;
    calarm.highp = UPPERAPRESS;
    calarm.lowp = LOWERAPRESS;
    return calarm;
}

/**
 * @brief Sets the alarm states based on the provided limits and reading data.
 * 
 * @param calarm An array of alarm structures to be updated
 * @param alarmpt The alarm limit structure containing high and low thresholds
 * @param rdata The reading data structure containing the current readings
 */
void GhSetAlarms(alarm_s calarm[NALARMS], alarmlimit_s alarmpt, reading_s rdata) {
    for (int i = 0; i < NALARMS; i++) {
        calarm[i].code = NOALARM;
    }

    if (rdata.temperature >= alarmpt.hight) {
        calarm[HTEMP].code = HTEMP;
        calarm[HTEMP].atime = rdata.rtime;
        calarm[HTEMP].value = rdata.temperature;
    }
    if (rdata.temperature <= alarmpt.lowt) {
        calarm[LTEMP].code = LTEMP;
        calarm[LTEMP].atime = rdata.rtime;
        calarm[LTEMP].value = rdata.temperature;
    }
    if (rdata.humidity >= alarmpt.highh) {
        calarm[HHUMID].code = HHUMID;
        calarm[HHUMID].atime = rdata.rtime;
        calarm[HHUMID].value = rdata.humidity;
    }
    if (rdata.humidity <= alarmpt.lowh) {
        calarm[LHUMID].code = LHUMID;
        calarm[LHUMID].atime = rdata.rtime;
        calarm[LHUMID].value = rdata.humidity;
    }
    if (rdata.pressure >= alarmpt.highp) {
        calarm[HPRESS].code = HPRESS;
        calarm[HPRESS].atime = rdata.rtime;
        calarm[HPRESS].value = rdata.pressure;
    }
    if (rdata.pressure <= alarmpt.lowp) {
        calarm[LPRESS].code = LPRESS;
        calarm[LPRESS].atime = rdata.rtime;
        calarm[LPRESS].value = rdata.pressure;
    }
}


/**
 * @brief Displays the active alarms.
 * 
 * @param calarm An array of alarm structures to be displayed
 */
void GhDisplayAlarms(alarm_s calarm[NALARMS]) {
    printf("\nAlarm\n");
    for (int i = 1; i < NALARMS; i++) {
        if (calarm[i].code != NOALARM) {
            printf("%s Alarm %s", alarmnames[calarm[i].code], ctime(&calarm[i].atime));
        }
    }
}
