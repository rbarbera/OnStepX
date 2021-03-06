// -------------------------------------------------------------------------------------------------
// Pin map for OnStep using RAMPS 1.4 Pin defs (Arduino Mega2560 & Arduino DUE)

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__SAM3X8E__)

// The multi-purpose pins (Aux3..Aux8 can be analog (pwm/dac) if supported)
#define Aux0                   11               // Status LED
#define Aux1                   29               // ESP8266 GPIO0, SPI MISO/Fault
#define Aux2                   37               // ESP8266 RST, SPI MISO/Fault
#if PINMAP == MksGenL1
  #define Aux3                 14               // Home SW (GenL1 Y- endstop)
  #define Aux4                 15               // 1-Wire (requires h/w modification,) Home SW (GenL1 Y+ endstop)
#else
  #define Aux3                 32               // Home SW; note modified pinmap 10/2/19 Aux3 and Aux4 were changed, 9/30/19 Aux5 was removed (Aux3 not present Gen-L1)
  #define Aux4                 39               // 1-Wire, Home SW (Aux4 not present Gen-L1)
#endif
#define Aux5                   7                // Dew Heater0 (E1 , MKS Gen-L)
#define Aux6                   8                // Dew Heater1 (BED, MKS Gen-L, RAMPS1.4)
#define Aux7                   9                // Dew Heater2 (FAN, MKS Gen-L, RAMPS1.4)
#define Aux8                   10               // Dew Heater3 (E0 , MKS Gen-L, RAMPS1.4)

#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN        53               // Default CS Pin for DS3234 on SPI
#endif
#ifndef BME280_CS_PIN
  #define BME280_CS_PIN        49               // Default CS Pin for BME280 on SPI
#endif
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN         Aux4             // Default Pin for OneWire bus
#endif
#ifndef ADDON_GPIO0_PIN
  #define ADDON_GPIO0_PIN      Aux1             // ESP8266 GPIO0 or SPI MISO/Fault
#endif
#ifndef ADDON_RESET_PIN
  #define ADDON_RESET_PIN      Aux2             // ESP8266 RST or SPI MISO/Fault
#endif

// For software SPI
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define SSPI_SHARED
#endif
#define SSPI_SCK               52
#define SSPI_MISO              50
#define SSPI_MOSI              51

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#if PINMAP == MksGenL1
  #define PEC_SENSE_PIN        41               // GenL1 EXP2
  #define PEC_ANALG_PIN        OFF              // N/A
#else
  #define PEC_SENSE_PIN        57               // RAMPS AUX1, A-OUT (1=+5V, 2=GND, 3=PEC)
  #define PEC_ANALG_PIN        A3               // Note A3 is (57)
#endif

// The limit switch sense is a 5V logic input which uses the internal (or external 2k) pull up, shorted to ground it stops gotos/tracking
#define LIMIT_SENSE_PIN        3                // RAMPS X- (1=LMT, 2=GND, 3=+5)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED1_PIN        Aux0             // RAMPS SERVO1 (1=GND, 2=+5, 3=LED-) (active LOW)
#define STATUS_LED2_PIN        6                // RAMPS SERVO2 (1=GND, 2=+5, 3=LED-) (active LOW)
#define RETICLE_LED_PIN        5                // RAMPS SERVO3 (1=GND, 2=+5, 3=LED-) (active LOW)

// Pin for a piezo buzzer output on RAMPS Y-MIN
#define BUZZER_PIN             4                // RAMPS SERVO4 (1=GND, 2=+5, 3=TONE+) (active HIGH)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PPS_SENSE_PIN          2                // RAMPS X+, Interrupt 0 on Pin 2

// Pins to Axis1 RA/Azm on RAMPS X
#define AXIS1_ENABLE_PIN       38
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS1_M0_PIN         51               // SPI MOSI
  #define AXIS1_M1_PIN         52               // SPI SCK
  #define AXIS1_M2_PIN         A9               // SPI CS
  #define AXIS1_M3_PIN         50               // SPI MISO
#else
  #define AXIS1_M0_PIN         23               // SPI MOSI
  #define AXIS1_M1_PIN         25               // SPI SCK
  #define AXIS1_M2_PIN         27               // SPI CS
  #define AXIS1_M3_PIN         Aux1             // SPI MISO
#endif
#define AXIS1_STEP_PIN         54
#define AXIS1_DIR_PIN          55
#define AXIS1_DECAY_PIN        AXIS1_M2_PIN
#define AXIS1_FAULT_PIN        Aux1
#define AXIS1_HOME_SENSE_PIN   Aux3

// Axis2 Dec/Alt step/dir driver on RMAPS Y
#define AXIS2_ENABLE_PIN       56               // (Pin A2)
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS2_M0_PIN         51               // SPI MOSI
  #define AXIS2_M1_PIN         52               // SPI SCK
  #define AXIS2_M2_PIN         A10              // SPI CS
  #define AXIS2_M3_PIN         50               // SPI MISO
#else
  #define AXIS2_M0_PIN         31               // SPI MOSI
  #define AXIS2_M1_PIN         33               // SPI SCK
  #define AXIS2_M2_PIN         35               // SPI CS
  #define AXIS2_M3_PIN         Aux2             // SPI MISO
#endif
#define AXIS2_STEP_PIN         60               // (Pin A6)
#define AXIS2_DIR_PIN          61               // (Pin A7)
#define AXIS2_DECAY_PIN        AXIS2_M2_PIN
#define AXIS2_FAULT_PIN        Aux2
#define AXIS2_HOME_SENSE_PIN   Aux4

// For rotator stepper driver on RAMPS Z
#define AXIS3_ENABLE_PIN       62               // (Pin A8)
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS3_M0_PIN         51               // SPI MOSI
  #define AXIS3_M1_PIN         52               // SPI SCK
  #define AXIS3_M2_PIN         A11              // SPI CS
  #define AXIS3_M3_PIN         50               // SPI MISO
#else
  #define AXIS3_M0_PIN         OFF              // SPI MOSI
  #define AXIS3_M1_PIN         OFF              // SPI SCK
  #define AXIS3_M2_PIN         OFF              // SPI CS
  #define AXIS3_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS3_STEP_PIN         46
#define AXIS3_DIR_PIN          48

// For focuser1 stepper driver on RAMPS E0
#define AXIS4_ENABLE_PIN       24
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS4_M0_PIN         51               // SPI MOSI
  #define AXIS4_M1_PIN         52               // SPI SCK
  #define AXIS4_M2_PIN         A12              // SPI CS
  #define AXIS4_M3_PIN         50               // SPI MISO
#else
  #define AXIS4_M0_PIN         OFF              // SPI MOSI
  #define AXIS4_M1_PIN         OFF              // SPI SCK
  #define AXIS4_M2_PIN         OFF              // SPI CS
  #define AXIS4_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS4_STEP_PIN         26
#define AXIS4_DIR_PIN          28

// For focuser2 stepper driver on RAMPS E1
#define AXIS5_ENABLE_PIN       30
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS5_M0_PIN         51               // SPI MOSI
  #define AXIS5_M1_PIN         52               // SPI SCK
  #if PINMAP == MksGenL21
    #define AXIS5_M2_PIN       12               // SPI CS
  #else
    #define AXIS5_M2_PIN       21               // SPI CS
  #endif
  #define AXIS5_M3_PIN         50               // SPI MISO
#else
  #define AXIS5_M0_PIN         OFF              // SPI MOSI
  #define AXIS5_M1_PIN         OFF              // SPI SCK
  #define AXIS5_M2_PIN         OFF              // SPI CS
  #define AXIS5_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS5_STEP_PIN         36
#define AXIS5_DIR_PIN          34

#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  // ST4 interface on MksGenL2 EXP-1
  #define ST4_RA_W_PIN         27               // ST4 RA- West
  #define ST4_DEC_S_PIN        23               // ST4 DE- South
  #define ST4_DEC_N_PIN        25               // ST4 DE+ North
  #define ST4_RA_E_PIN         35               // ST4 RA+ East
#else
  // ST4 interface on RAMPS AUX-2
  #define ST4_RA_W_PIN         A9               // ST4 RA- West
  #define ST4_DEC_S_PIN        40               // ST4 DE- South
  #define ST4_DEC_N_PIN        42               // ST4 DE+ North
  #define ST4_RA_E_PIN         A11              // ST4 RA+ East
#endif

#else
#error "Wrong processor for this configuration!"

#endif
