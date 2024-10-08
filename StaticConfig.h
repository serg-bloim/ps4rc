#define SRXL 1
#define SBUS 2
#ifndef SERIAL_MODE
// #define SERIAL_MODE SRXL
#define SERIAL_MODE SBUS
#endif

#if SERIAL_MODE == SRXL
#endif

#if SERIAL_MODE == SBUS
#define SERIAL_MODE_STR "SBUS"
#endif

String DEFAULT_PS4_BT_ADDR = "50:54:4E:50:4E:58";
#define DEFAULT_SERIAL_PIN 13
#define DEFAULT_SERIAL_INVERTED false
#define CONFIG_PIN_OUT 26
#define CONFIG_PIN_IN 27

#ifndef PS4RC_VERSION
#define PS4RC_VERSION "undefined"
#endif

#ifndef PS4RC_CODE_REV
#define PS4RC_CODE_REV "undefined"
#endif