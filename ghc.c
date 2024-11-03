/** @brief Defines the entry point for the console application.
 *  @file ghc.c
 *  @version ceng153, Serial: 401b6db6
 */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "ghcontrol.h"


int main(void) 
{
    reading_s creadings = {0};
    control_s ctrl = {0};
    setpoint_s sets = {0};
    alarmlimit_s alimits = {0};
    alarm_s warn[NALARMS] ;
    int logged;

    GhControllerInit();
    sets = GhSetTargets();
    alimits = GhSetAlarmLimits();

    while (1) 
    {
        creadings = GhGetReadings();
        logged = GhLogData("ghdata.txt", creadings);
        ctrl = GhSetControls(sets, creadings);
        GhSetAlarms(warn, alimits, creadings);

        GhDisplayAll(creadings, sets);  
        GhDisplayReadings(creadings);
        GhDisplayTargets(sets);
        GhDisplayControls(ctrl);
        GhDisplayAlarms(warn);

        GhDelay(GHUPDATE);
    }



    return EXIT_FAILURE;
}

