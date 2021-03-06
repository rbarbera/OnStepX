//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#if AXIS1_STEPS_PER_WORMROT == 0
  #define AXIS1_PEC OFF
#else
  #define AXIS1_PEC ON
#endif

#include "../debug/Debug.h"
#include "../commands/ProcessCmds.h"
#include "../coordinates/Transform.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"

enum MeridianFlip     {MF_NEVER, MF_ALWAYS};
enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
enum TrackingState    {TS_NONE, TS_SIDEREAL};
enum GotoState        {GS_NONE, GS_GOTO, GS_GOTO_SYNC, GS_GOTO_ABORT};
enum GotoStage        {GG_NONE, GG_START, GG_WAYPOINT, GG_DESTINATION};
enum GotoType         {GT_NONE, GT_HOME, GT_PARK};
enum PierSideSelect   {PSS_NONE, PSS_EAST, PSS_WEST, PSS_BEST, PSS_EAST_ONLY, PSS_WEST_ONLY, PSS_SAME_ONLY};
enum GuideState       {GU_NONE, GU_GUIDE, GU_PULSE_GUIDE};
enum GuideRateSelect  {GR_QUARTER, GR_HALF, GR_1X, GR_2X, GR_4X, GR_8X, GR_20X, GR_48X, GR_HALF_MAX, GR_MAX, GR_CUSTOM};
enum GuideAction      {GA_NONE, GA_BREAK, GA_FORWARD, GA_REVERSE};
enum ParkState        {PS_NONE, PS_UNPARKED, PS_PARKING, PS_PARKED, PS_PARK_FAILED};
enum PecState         {PEC_NONE, PEC_READY_PLAY, PEC_PLAY, PEC_READY_RECORD, PEC_RECORD};

#pragma pack(1)
typedef struct AltitudeLimits {
  float min;
  float max;
} AltitudeLimits;

#define LimitsSize 16
typedef struct Limits {
  AltitudeLimits altitude;
  float pastMeridianE;
  float pastMeridianW;
} Limits;

#define MiscSize 9
typedef struct Misc {
  bool syncToEncodersOnly:1;
  bool meridianFlipAuto  :1;
  bool meridianFlipPause :1;
  bool buzzer:1;
  float usPerStepCurrent;
  GuideRateSelect pulseGuideRateSelect;
} Misc;

typedef struct Worm {
  long rotationSteps;
  long sensePositionSteps;
} Worm;

#define PecSize 13
typedef struct Pec {
  bool recorded:1;
  PecState state;
  Worm worm;
} Pec;
#pragma pack()

typedef struct AlignState {
  uint8_t currentStar;
  uint8_t lastStar;
} AlignState;

typedef struct AltitudeError {
  uint8_t minExceeded:1;
  uint8_t maxExceeded:1;
} AltitudeError;

typedef struct MerdianError {
  uint8_t eastExceeded:1;
  uint8_t westExceeded:1;
} MerdianError;

typedef struct MountError {
  AltitudeError altitude;
  MerdianError  meridian;
  uint8_t       parkFailed: 1;
} MountError;

class Mount {
  public:
    void init();
    void initPec();

    // handle mount commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);
    bool commandGoto(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);
    bool commandGuide(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);
    bool commandLimit(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);
    bool commandPec(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

    // update the home position
    void updateHomePosition();

    // reset all mount related errors
    void resetErrors();

    // check for any mount related error
    bool anyError();

    // monitor goto operation
    void gotoPoll();

    // monitor guide operation
    void guidePoll();

    // monitor pec operation
    void pecPoll();

    Transform  transform;

    Axis axis1;
    Axis axis2;

    MountError error;

  private:
    // general status checks ahead of sync or goto
    CommandError validateGoto();
    // target coordinate checks ahead of sync or goto
    CommandError validateGotoCoords(Coordinate *coords);

    CommandError setMountTarget(Coordinate *coords, PierSideSelect pierSideSelect);

    // sync. to equatorial coordinates
    CommandError syncEqu(Coordinate *coords, PierSideSelect pierSideSelect);

    // goto equatorial coordinates
    CommandError gotoEqu(Coordinate *coords, PierSideSelect pierSideSelect);

    // reset mount
    CommandError resetHome();

    // set any waypoints required for a goto
    void setWaypoint();

    // change tracking state
    void setTrackingState(TrackingState state);

    // update where we are pointing *now*
    void updatePosition();

    // update tracking rates for refraction, pointing model, and alt/azm tracking
    void updateTrackingRates();

    // update acceleration rates for goto and guiding
    void updateAccelerationRates();

    // estimate average microseconds per step lower limit
    float usPerStepLowerLimit();

    // return guide rate (sidereal x) for guide rate selection
    float guideRateSelectToRate(GuideRateSelect guideRateSelect, uint8_t axis = 1);

    // valid guide on Axis1
    bool validGuideAxis1(GuideAction guideAction);
    // valid guide on Axis2
    bool validGuideAxis2(GuideAction guideAction);

    // start guide on Axis1
    CommandError startGuideAxis1(GuideAction guideAction, GuideRateSelect guideRateSelect, unsigned long guideTimeLimit);
    // start guide on Axis2
    CommandError startGuideAxis2(GuideAction guideAction, GuideRateSelect guideRateSelect, unsigned long guideTimeLimit);
    // stop guide on Axis1
    void stopGuideAxis1();
    // stop guide on Axis2
    void stopGuideAxis2();
    // check for spiral guide using both axes
    bool isSpiralGuiding();

    // disable PEC
    void pecDisable();
    // PEC low pass and linear regression filters
    void pecCleanup();

    const double radsPerCentisecond  = (degToRad(15.0/3600.0)/100.0)*SIDEREAL_RATIO;
    
    // current position in Mount coordinates (Observed Place with no corrections except index offset)
    // coordinates are either Horizon (a, z) or Equatorial (h, d) depending on the mountType
    Coordinate current;
    TrackingState trackingState         = TS_NONE;
    #if TRACK_REFRACTION_RATE_DEFAULT == ON
      RateCompensation rateCompensation = RC_REFR_RA;
    #else
      RateCompensation rateCompensation = RC_NONE;
    #endif
    float trackingRate                  = 1.0F;
    float trackingRateAxis1             = 0.0F;
    float trackingRateAxis2             = 0.0F;
    float deltaRateAxis1                = 0.0F;
    float deltaRateAxis2                = 0.0F;
    float stepsPerSiderealSecondAxis1   = 0.0F;
    float stepsPerCentisecondAxis1      = 0.0F;

    // align
    AlignState alignState = {0, 0};

    // goto
    PierSideSelect preferredPierSide    = (PierSideSelect)PIER_SIDE_PREFERRED_DEFAULT;
    MeridianFlip meridianFlip           = MF_ALWAYS;
    Coordinate gotoTarget;
    Coordinate start, destination, target;
    GotoState  gotoState                = GS_NONE;
    GotoStage  gotoStage                = GG_START;
    GotoState  gotoStateAbort           = GS_NONE;
    GotoState  gotoStateLast            = GS_NONE;
    uint8_t    gotoTaskHandle           = 0;
    float      usPerStepDefault         = 64.0F;
    float      usPerStepBase            = 128.0F;
    float      radsPerSecondCurrent;

    // limits
    bool       limitsEnabled            = false;
    Limits limits = { { degToRad(-10), degToRad(85) }, degToRad(15), degToRad(15) };

    // homing
    Coordinate home;
    bool       atHome                   = true;
    bool       waitingHome              = false;
    bool       waitingHomeContinue      = false;
  
    // guiding
    float guideRateAxis1                = 0.0F;
    float guideRateAxis2                = 0.0F;
    float customGuideRateAxis1          = 0.0F;
    float customGuideRateAxis2          = 0.0F;
    GuideState      guideState          = GU_NONE;
    GuideRateSelect guideRateSelect     = GR_20X;
    GuideRateSelect guideRateSelectAxis1= GR_20X;
    GuideRateSelect guideRateSelectAxis2= GR_20X;
    GuideAction     guideActionAxis1    = GA_NONE;
    GuideAction     guideActionAxis2    = GA_NONE;
    unsigned long guideFinishTimeAxis1  = 0;
    unsigned long guideFinishTimeAxis2  = 0;

    // pec
    long      pecBufferSize             = 0;
    float     pecRateAxis1              = 0;
    #if AXIS1_PEC == ON
      Pec pec = {false, PEC_NONE, {0, 0}};
      bool     pecIndexSensedSinceLast  = false;
      int      pecAnalogValue           = 0;

      bool     pecFirstRecording        = false;
      long     pecRecordStopTime        = 0;
      float    accPecGuideAxis1         = 0.0F;
      long     pecIndex                 = 0;
      int      pecValue                 = 0;
      long     wormRotationSteps        = 0;
      long     lastWormRotationSteps    = -1;
      long     wormSenseSteps           = 0;
      bool     wormSenseAgain           = false;
      uint32_t wormPeriodStartCs        = 0;
      long     wormRotationSeconds      = 0;
      bool     pecBufferStart           = false;
      int8_t*  pecBuffer;
    #endif

    // park
    ParkState parkState = PS_UNPARKED;

    // misc. settings stored in NV
    Misc misc = {false, false, false, false, 64.0, GR_20X};

    float timerRateRatio = 1.0;

};

#endif
