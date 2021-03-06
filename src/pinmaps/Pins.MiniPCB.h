// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MiniPCB (Teensy3.0, 3.1, 3.2, and 4.0)

#if defined(__MK20DX256__) || defined(_mk20dx128_h_) || defined(__MK20DX128__) || defined(__IMXRT1052__) || defined(__IMXRT1062__)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0                    19               // Status LED
#define Aux1                    18               // ESP8266 GPIO0, SPI MISO/Fault
#define Aux2                    5                // ESP8266 RST, SPI MISO/Fault
#define Aux3                    4                // Limit SW, Home SW
#define Aux4                    22               // Status2 LED, Reticle LED, Home SW
#if !defined(_mk20dx128_h_) && !defined(__MK20DX128__) && !defined(__IMXRT1052__) && !defined(__IMXRT1062__)
  #define Aux5                  A14              // true analog output
#endif
#define Aux5_Analog

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          24               // Default Pin for OneWire bus
#endif
#if PINMAP == MiniPCB13
  #define ADDON_GPIO0_PIN       2                // ESP8266 GPIO0 (Dir2)
  #define ADDON_RESET_PIN       Aux2             // ESP8266 RST
#else
  #define ADDON_GPIO0_PIN       Aux1             // ESP8266 GPIO0 or SPI MISO/Fault
  #define ADDON_RESET_PIN       Aux2             // ESP8266 RST or SPI MISO/Fault
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN           23
#define PEC_ANALG_PIN           23               // PEC Sense, analog or digital

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED1_PIN         Aux0             // Drain
#define STATUS_LED2_PIN         Aux4             // Drain
#define RETICLE_LED_PIN         Aux4             // Drain

// For a piezo buzzer
#define BUZZER_PIN              29               // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PPS_SENSE_PIN           28               // PPS time source, GPS for example

#define LIMIT_SENSE_PIN         Aux3             // The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        14
#define AXIS1_M0_PIN            15               // SPI MOSI
#define AXIS1_M1_PIN            16               // SPI SCK
#define AXIS1_M2_PIN            17               // SPI CS or Decay Mode
#define AXIS1_M3_PIN            Aux1             // SPI MISO
#define AXIS1_STEP_PIN          20
#define AXIS1_DIR_PIN           21
#define AXIS1_DECAY_PIN         AXIS1_M2_PIN
#define AXIS1_FAULT_PIN         Aux1             // SPI MISO
#define AXIS1_HOME_SENSE_PIN    Aux3

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        9
#define AXIS2_M0_PIN            8                // SPI MOSI
#define AXIS2_M1_PIN            7                // SPI SCK
#define AXIS2_M2_PIN            6                // SPI CS
#if PINMAP == MiniPCB13
  #define AXIS2_M3_PIN          Aux1             // SPI MISO
#else
  #define AXIS2_M3_PIN          Aux2             // SPI MISO
#endif
#define AXIS2_STEP_PIN          3
#define AXIS2_DIR_PIN           2
#define AXIS2_DECAY_PIN         AXIS2_M2_PIN
#define AXIS2_FAULT_PIN         AXIS2_M3_PIN
#define AXIS2_HOME_SENSE_PIN    Aux4

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        OFF
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS
#define AXIS3_M3_PIN            OFF              // SPI MISO
#define AXIS3_STEP_PIN          30
#define AXIS3_DIR_PIN           33

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        OFF
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS
#define AXIS4_M3_PIN            OFF              // SPI MISO
#define AXIS4_STEP_PIN          31
#define AXIS4_DIR_PIN           32

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        OFF
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS
#define AXIS5_M3_PIN            OFF              // SPI MISO
#define AXIS5_STEP_PIN          30
#define AXIS5_DIR_PIN           33

// ST4 interface
#define ST4_RA_W_PIN            10               // ST4 RA- West
#define ST4_DEC_S_PIN           11               // ST4 DE- South
#define ST4_DEC_N_PIN           12               // ST4 DE+ North
#define ST4_RA_E_PIN            13               // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
