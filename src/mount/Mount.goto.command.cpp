//--------------------------------------------------------------------------------------------------
// telescope mount control, commands
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../coordinates/Convert.h"
#include "../coordinates/Transform.h"
#include "../coordinates/Site.h"
#include "../commands/ProcessCmds.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"
#include "Mount.h"

extern unsigned long periodSubMicros;

bool Mount::commandGoto(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {
  PrecisionMode precisionMode = PM_HIGH;

  //  C - Sync Control
  // :CS#       Synchonize the telescope with the current right ascension and declination coordinates
  //            Returns: Nothing (Sync's fail silently)
  // :CM#       Synchonize the telescope with the current database object (as above)
  //            Returns: "N/A#" on success, "En#" on failure where n is the error code per the :MS# command
  if (cmd("CS") || cmd("CM")) {
    CommandError e;
    //if (alignActive()) { e = alignStar(); if (e != CE_NONE) { alignNumStars = 0; alignThisStar = 0; commandError = e; } } else e = syncEqu(gotoTarget);
    e = syncEqu(&gotoTarget, preferredPierSide);
    if (command[1] == 'M') {
      if (e >= CE_SLEW_ERR_BELOW_HORIZON && e <= CE_SLEW_ERR_UNSPECIFIED) strcpy(reply,"E0"); reply[1] = (char)(e - CE_SLEW_ERR_BELOW_HORIZON) + '1';
      if (e == CE_NONE) strcpy(reply,"N/A");
    }
    *numericReply = false;
  } else

  // :GA#       Get Mount Altitude
  //            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
  // :GAH#      High precision
  //            Returns: sDD*MM'SS.SSS# (high precision)
  if (cmdH("GA")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    transform.site.convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current, true).a), false, true, precisionMode);
    *numericReply = false;
  } else

  // :Ga#       Get Target Altitude
  //            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
  // :GaH#      High precision
  //            Returns: sDD*MM'SS.SSS# (high precision)
  if (cmdH("Ga")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    transform.site.convert.doubleToHms(reply, radToDeg(gotoTarget.a), false, precisionMode);
    *numericReply = false;
  } else

  // :GD#       Get Mount Declination
  //            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
  // :GDH#      Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("GD")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    transform.site.convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current).d), false, true, precisionMode);
    *numericReply = false;
  } else

  // :Gd#       Get Target Declination
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GdH#      High precision
  //            Returns: HH:MM:SS.SSS# (high precision)
  if (cmdH("Gd")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    transform.site.convert.doubleToDms(reply, radToDeg(gotoTarget.d), false, true, precisionMode);
    *numericReply = false;
  } else

  // :GR#       Get Mount Right Ascension
  //            Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
  // :GRH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("GR")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    transform.site.convert.doubleToHms(reply, radToHrs(transform.mountToNative(&current).r), false, precisionMode);
    *numericReply = false;
  } else

  // :Gr#       Get Target Right Ascension
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GrH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("Gr")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    transform.site.convert.doubleToHms(reply, radToHrs(gotoTarget.r), false, precisionMode);
    *numericReply = false;
  } else

  // :GZ#       Get Mount Azimuth
  //            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
  // :GZH#      High precision
  //            Returns: DDD*MM'SS.SSS# (high precision)
  if (cmdH("GZ")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    transform.site.convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current, true).z), true, false, precisionMode);
    *numericReply = false;
  } else

  // :Gz#       Get Target Azimuth
  //            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
  // :GzH#      High precision
  //            Returns: DDD*MM'SS.SSS# (high precision)
  if (cmdH("Gz")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    transform.site.convert.doubleToDms(reply, radToDeg(gotoTarget.z), false, true, precisionMode);
    *numericReply = false;
  } else

  // :MA#       Goto the target Alt and Az
  //            Returns: 0..9, see :MS#
  if (command[1] == 'A' && parameter[0] == 0) {
    transform.horToEqu(&gotoTarget);
    CommandError e = setMountTarget(&gotoTarget, preferredPierSide);
    if (e >= CE_SLEW_ERR_BELOW_HORIZON && e <= CE_SLEW_ERR_UNSPECIFIED) reply[0] = (char)(e - CE_SLEW_ERR_BELOW_HORIZON) + '1';
    if (e == CE_NONE) reply[0] = '0';
    reply[1] = 0;
    *numericReply = false; 
    *supressFrame = true;
    *commandError = e;
  } else

  // :MD#       Goto Destination pier side for the Target Object
  //            Returns:
  //              0=destination is East of the pier
  //              1=destination is West of the pier
  //              2=an error occured
  if (cmd("MD"))  {
    CommandError e = setMountTarget(&gotoTarget, preferredPierSide);
    strcpy(reply, "2");
    if (e == CE_NONE && target.pierSide == PIER_SIDE_EAST) reply[0] = '0';
    if (e == CE_NONE && target.pierSide == PIER_SIDE_WEST) reply[0] = '1';
    *numericReply = false;
    *supressFrame = true;
    *commandError = e;
  } else

  //  :MN#   Goto current RA/Dec but East of the Pier (within meridian limit overlap for GEM mounts)
  //  :MNe#  as above
  //  :MNw#  Goto current RA/Dec but West of the Pier (within meridian limit overlap for GEM mounts)
  //         Returns: 0..9, see :MS#
  if (cmdP("MN"))  {
    updatePosition();
    Coordinate newTarget = current;
    CommandError e;
    if (parameter[0] == 0 ||
       (parameter[0] == 'e' && parameter[1] == 0)) e = gotoEqu(&newTarget, PSS_EAST_ONLY); else
    if (parameter[0] == 'w' && parameter[1] == 0) e = gotoEqu(&newTarget, PSS_WEST_ONLY); else e = CE_CMD_UNKNOWN;
    if (e != CE_CMD_UNKNOWN) {
      if (e >= CE_SLEW_ERR_BELOW_HORIZON && e <= CE_SLEW_ERR_UNSPECIFIED) reply[0] = (char)(e - CE_SLEW_ERR_BELOW_HORIZON) + '1';
      if (e == CE_NONE) reply[0] = '0';
      reply[1] = 0;
      *numericReply = false;
      *supressFrame = true;
    }
    *commandError = e;
  } else

  // :MP#       Goto the Current Position for Polar Align
  //            Returns: 0..9, see :MS#
  if (cmd("MP"))  {
    if (transform.mountType != ALTAZM) {
      updatePosition();
      Coordinate newTarget = current;
      validateGoto();
      CommandError e = validateGoto();
      if (e == CE_NONE) e = validateGotoCoords(&newTarget);
      if (e == CE_NONE) {
        // AlignE.altCor = 0.0;
        // AlignE.azmCor = 0.0;
        e = gotoEqu(&newTarget, PSS_SAME_ONLY);
      }
      if (e >= CE_SLEW_ERR_BELOW_HORIZON && e <= CE_SLEW_ERR_UNSPECIFIED) reply[0] = (char)(e - CE_SLEW_ERR_BELOW_HORIZON) + '1';
      if (e == CE_NONE) reply[0] = '0';
      reply[1] = 0;
      *numericReply = false;
      *supressFrame = true;
      *commandError = e;
    } else *commandError = CE_CMD_UNKNOWN;
  } else

  // :MS#       Goto the Target Object
  //            Returns:
  //              0=Goto is possible
  //              1=below the horizon limit
  //              2=above overhead limit
  //              3=controller in standby
  //              4=mount is parked
  //              5=Goto in progress
  //              6=outside limits (AXIS2_LIMIT_MAX, AXIS2_LIMIT_MIN, AXIS1_LIMIT_MIN/MAX, MERIDIAN_E/W)
  //              7=hardware fault
  //              8=already in motion
  //              9=unspecified error
  if (cmd("MS")) {
    CommandError e = gotoEqu(&gotoTarget, preferredPierSide);
    strcpy(reply,"0");
    if (e >= CE_SLEW_ERR_BELOW_HORIZON && e <= CE_SLEW_ERR_UNSPECIFIED) reply[0] = (char)(e - CE_SLEW_ERR_BELOW_HORIZON) + '1';
    if (e == CE_NONE) reply[0] = '0';
    *numericReply = false;
    *supressFrame = true;
    *commandError = e;
  } else

  //  :Sa[sDD*MM]# or :Sa[sDD*MM'SS]# or :Sa[sDD*MM'SS.SSS]#
  //            Set Target Altitude
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sa")) {
    if (!transform.site.convert.dmsToDouble(&gotoTarget.a, parameter, true)) *commandError = CE_PARAM_RANGE; else
    gotoTarget.a = degToRad(gotoTarget.a);
  } else

  //  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
  //            Set Target Declination
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sd")) {
    if (!transform.site.convert.dmsToDouble(&gotoTarget.d, parameter, true)) *commandError = CE_PARAM_RANGE; else
    gotoTarget.d = degToRad(gotoTarget.d);
  } else

  //  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
  //            Set Target Right Ascension
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sr")) {
    if (!transform.site.convert.hmsToDouble(&gotoTarget.r, parameter)) *commandError = CE_PARAM_RANGE; else
    gotoTarget.r = hrsToRad(gotoTarget.r);
  } else

  //  :Sz[DDD*MM]# or :Sz[DDD*MM'SS]# or :Sz[DDD*MM'SS.SSS]#
  //            Set Target Azmuith
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sz")) {
    if (!transform.site.convert.dmsToDouble(&gotoTarget.z, parameter, false)) *commandError = CE_PARAM_RANGE; else
    gotoTarget.z = degToRad(gotoTarget.z);
  } else return false;

  return true;
}

#endif