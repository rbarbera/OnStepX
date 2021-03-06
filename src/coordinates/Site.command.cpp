//--------------------------------------------------------------------------------------------------
// telescope mount time and location, commands
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../lib/nv/NV.h"
extern NVS nv;
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#include "../coordinates/Convert.h"
#include "../commands/ProcessCmds.h"
#include "Site.h"

bool Site::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {
  PrecisionMode precisionMode = convert.precision;

  // :Ga#       Get standard time in 12 hour format
  //            Returns: HH:MM:SS#
  if (cmd("Ga"))  {
    double time = backInHours(getTime() - location.timezone);
    if (time > 12.0) time -= 12.0;
    convert.doubleToHms(reply, time, false, PM_HIGH);
    *numericReply = false;
  } else

  // :GC#       Get standard calendar date
  //            Returns: MM/DD/YY#
  if (cmd("GC")) {
    JulianDate julianDay = ut1;
    double hour = getTime() - location.timezone;
    while (hour >= 24.0) { hour -= 24.0; julianDay.day += 1.0; }
    if    (hour < 0.0)   { hour += 24.0; julianDay.day -= 1.0; }
    GregorianDate date = julianDayToGregorian(julianDay);
    date.year -= 2000; while (date.year >= 100) date.year -= 100;
    sprintf(reply,"%02d/%02d/%02d", (int)date.month, (int)date.day, (int)date.year);
    *numericReply = false;
  } else

  // :Gc#       Get the Local Standard Time format
  //            Returns: 24#
  if (cmd("Gc")) {
    strcpy(reply, "24");
    *numericReply = false;
  } else

  // :Gt#       Get current site Latitude, positive for North latitudes
  //            Returns: sDD*MM#
  // :GtH#      Get current site Latitude, positive for North latitudes
  //            Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("Gt")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(location.latitude), false, true, precisionMode);
    *numericReply = false;
  } else 

  // :GG#       Get UTC offset time, hours and minutes to add to local time to convert to UTC
  //            Returns: [s]HH:MM#
  if (cmd("GG"))  {
    convert.doubleToHms(reply, location.timezone, true, PM_LOWEST);
    *numericReply=false;
  } else

  // :Gg#       Get Current Site Longitude, east is negative
  //            Returns: sDDD*MM#
  // :GgH#      Get current site Longitude
  //            Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("Gg"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(location.longitude), true, true, precisionMode);
    *numericReply = false;
  } else 

  // :GL#       Get Local Standard Time in 24 hour format
  //            Returns: HH:MM:SS#
  // :GLH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("GL")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, backInHours(getTime() - location.timezone), false, precisionMode);
    *numericReply = false;
  } else

  // :GM#       Get site 1 name
  // :GN#       Get site 2 name
  // :GO#       Get site 3 name
  // :GP#       Get site 4 name
  //            Returns: s#
  if (command[0] == 'G' && (command[1] == 'M' || command[1] == 'N' || command[1] == 'O' || command[1] == 'P') && parameter[0] == 0)  {
    Location tempLocation;
    uint8_t locationNumber = command[1] - 'M';
    nv.readBytes(NV_LOCATION_BASE + locationNumber*LocationSize, &tempLocation, LocationSize);
    strcpy(reply, tempLocation.name);
    if (reply[0] == 0) { strcat(reply,"None"); }
    *numericReply = false; 
  } else

  // :GS#       Get the Sidereal Time as sexagesimal value in 24 hour format
  //            Returns: HH:MM:SS#
  // :GSH#      Returns: HH:MM:SS.ss# (high precision)
  if (cmdH("GS")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, getSiderealTime(), false, precisionMode);
    *numericReply = false;
  } else

  if (cmdGX("GX8")) {

    // :GX80#     Get the UT1 Time as sexagesimal value in 24 hour format
    //            Returns: HH:MM:SS.ss#
    if (parameter[1] == '0') {
      convert.doubleToHms(reply, backInHours(getTime()), false, PM_HIGH);
      *numericReply = false;
    } else

    // :GX81#     Get the UT1 Date
    //            Returns: MM/DD/YY#
    if (parameter[1] == '1') {
      JulianDate julianDay = ut1;
      double hour = getTime();
      while (hour >= 24.0) { hour -= 24.0; julianDay.day += 1.0; }
      if    (hour < 0.0)   { hour += 24.0; julianDay.day -= 1.0; }
      GregorianDate date = julianDayToGregorian(julianDay);
      date.year -= 2000; if (date.year >= 100) date.year -= 100;
      sprintf(reply,"%02d/%02d/%02d", (int)date.month, (int)date.day, (int)date.year);
      *numericReply = false;
    } else

    // :GX89#     Date/time ready status
    //            Return: 0 ready, 1 not ready
    if (parameter[1] == '9') {
      if (dateIsReady && timeIsReady) *commandError = CE_0;
    } else return false;
  } else

  if (cmdGX("GX9")) {

    // :GX9A#     temperature in deg. C
    //            Returns: +/-n.n
    if (parameter[1] == 'A') {
      dtostrf(siteConditions.temperature,3,1,reply);
      *numericReply = false;
     } else

    // :GX9B#     pressure in mb
    //            Returns: +/-n.n
    if (parameter[1] == 'B') {
      dtostrf(siteConditions.pressure,3,1,reply);
      *numericReply = false;
    } else

    // :GX9C#     relative humidity in %
    //            Returns: +/-n.n
    if (parameter[1] == 'C') {
      dtostrf(siteConditions.humidity,3,1,reply);
      *numericReply = false;
    } else

    // :GX9D#     altitude in meters
    //            Returns: +/-n.n
    if (parameter[1] == 'D') {
      dtostrf(siteConditions.altitude,3,1,reply);
      *numericReply = false;
    } else

    // :GX9E#     dew point in deg. C
    //            Returns: +/-n.n
    if (parameter[1] == 'E') {
      dtostrf(dewPoint(siteConditions),3,1,reply);
      *numericReply = false;
    } else return false;
  } else
 
  // :SC[MM/DD/YY]#
  //            Change standard date to MM/DD/YY
  //            Return: 0 on failure, 1 on success
  if (cmdP("SC"))  {
    GregorianDate date = convert.strToDate(parameter);
    if (date.valid) {
      ut1 = gregorianToJulianDay(date);
      ut1.hour = backInHours(getTime());
      double hour = ut1.hour - location.timezone;
      if (hour >= 24.0) { hour -= 24.0; ut1.day += 1.0; } else
      if (hour <  0.0)  { hour += 24.0; ut1.day -= 1.0; }
      setSiderealTime(ut1, julianDateToLAST(ut1));
      if (NV_ENDURANCE >= NVE_MID) nv.updateBytes(NV_JD_BASE, &ut1, JulianDateSize);
      dateIsReady = true;
      if (error.TLSinit && dateIsReady && timeIsReady) error.TLSinit = false;
      delay(10);
    } else *commandError = CE_PARAM_FORM;
  } else

  //  :SG[sHH]# or :SG[sHH:MM]# (where MM is 00, 30, or 45)
  //            Set the number of hours added to local time to yield UTC
  //            Return: 0 failure, 1 success
  if (cmdP("SG")) {
    double hour;
    if (convert.tzToDouble(&hour, parameter)) {
      if (hour >= -13.75 || hour <= 12.0) {
        location.timezone = hour;
        nv.updateBytes(NV_LOCATION_BASE + number*LocationSize, &location, LocationSize);
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  :Sg[(s)DDD*MM]# or :Sg[(s)DDD*MM:SS]# or :Sg[(s)DDD*MM:SS.SSS]#
  //            Set current site longitude, east longitudes can be negative or > 180 degrees
  //            Return: 0 failure, 1 success
  if (cmdP("Sg"))  {
    double degs;
    int i;
    if (parameter[0] == '-' || parameter[0] == '+') i = 1; else i = 0;
    if (convert.dmsToDouble(&degs, (char *)&parameter[i], false)) {
      if (parameter[0] == '-') location.longitude = -location.longitude;
      if (degs >= -180.0 && degs <= 360.0) {
        if (degs >= 180.0) degs -= 360.0;
        location.longitude = degToRad(degs);
        update();
        nv.updateBytes(NV_LOCATION_BASE + number*LocationSize, &location, LocationSize);
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  :SL[HH:MM:SS]# or :SL[HH:MM:SS.SSS]#
  //            Set the local Time
  //            Return: 0 failure, 1 success
  if (cmdP("SL"))  {
    double hour;
    if (convert.hmsToDouble(&hour, parameter, PM_HIGH) || convert.hmsToDouble(&hour, parameter, PM_HIGHEST)) {
      ut1.hour = hour + location.timezone;
      setTime(ut1);
      if (NV_ENDURANCE >= NVE_MID) nv.updateBytes(NV_JD_BASE, &ut1, JulianDateSize);
      timeIsReady = true;
      if (error.TLSinit && dateIsReady && timeIsReady) error.TLSinit = false;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  :SM[s]# or :SN[s]# or :SO[s]# or :SP[s]#
  //            Set site name, string may be up to 15 characters
  //            Return: 0 on failure
  //                    1 on success
  if (command[0] == 'S' && (command[1] == 'M' || command[1] == 'N' || command[1] == 'O' || command[1] == 'P')) {
    uint8_t locationNumber = command[1] - 'M';
    if (strlen(parameter) <= 15) {
      Location tempLocation;
      nv.readBytes(NV_LOCATION_BASE + locationNumber*LocationSize, &tempLocation, LocationSize);
      strcpy(tempLocation.name, parameter);
      nv.updateBytes(NV_LOCATION_BASE + locationNumber*LocationSize, &tempLocation, LocationSize);
    } else *commandError = CE_PARAM_RANGE;
  } else

  //  :St[sDD*MM]# or :St[sDD*MM:SS]# or :St[sDD*MM:SS.SSS]#
  //            Set current site latitude
  //            Return: 0 failure, 1 success
  if (cmdP("St"))  {
    double degs;
    if (convert.dmsToDouble(&degs, parameter, true)) {
      location.latitude = degToRad(degs);
      update();
      nv.updateBytes(NV_LOCATION_BASE + number*LocationSize, &location, LocationSize);
    } else *commandError = CE_PARAM_FORM;
  } else 

  if (cmdSX("SX9")) {
    char *conv_end;
    float f = strtod(&parameter[3], &conv_end);
    if (&parameter[3] == conv_end) f = NAN;
    // :SX9A,[sn.n]#
    //            Set temperature in deg. C
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'A') {
      if (f >= -100.0 && f < 100.0) siteConditions.temperature = f; else *commandError = CE_PARAM_RANGE;
    } else
    // :SX9B,[n.n]#
    //            Set pressure in mb
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'B') {
      if (f >= 500.0 && f < 1500.0) siteConditions.pressure = f; else *commandError = CE_PARAM_RANGE;
    } else
    // :SX9C,[n.n]#
    //            relative humidity in %
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'C') {
      if (f >= 0.0 && f < 100.0) siteConditions.humidity = f; else *commandError = CE_PARAM_RANGE;
    } else
    // :SX9D,[sn.n]#
    //            altitude
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'D') {
      if (f >= -100.0 && f < 20000.0) siteConditions.altitude = f; else *commandError = CE_PARAM_RANGE;
    } else return false;
  } else

  // :W[n]#     Sets current site to n, where n = 0..3
  //            Returns: Nothing
  if (command[0] == 'W' && (command[1] >= '0' && command[1] <= '3') && parameter[0] == 0) {
    number = command[1] - '0';
    nv.update(NV_LOCATION_NUMBER, number);
    readLocation(number);
    update();
    *numericReply = false;
  } else

  // :W?#       Queries current site
  //            Returns: n#
  if (command[0] == 'W' && command[1] == '?') {
    sprintf(reply, "%d", (int)nv.read(NV_LOCATION_NUMBER));
    *numericReply = false;
  } else return false;

  return true;
}
