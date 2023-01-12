#ifndef __SETTINGS__
#define __SETTINGS__

#define BRIGHTNESS 64
#define STRIPS 8

#define LED_PIN1 23
#define LED_PIN2 19
#define LED_PIN3 18
//#define LED_PIN4 5
#define LED_PIN4 22
#define LED_PIN5 0
#define LED_PIN6 25
#define LED_PIN7 26
#define LED_PIN8 27

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

/* #define SW_PIN 21
#define X_PIN 35
#define Y_PIN 32
#define JOY_SENS 250
#define JOY_REF 1900
#define JOY_LO (JOY_REF-JOY_SENS)
#define JOY_HI (JOY_REF+JOY_SENS) */

#define UPDATES_PER_SECOND 50
#define FADE_RATE 10

/* #define UPDATES_PER_BLINK 12
#define UPDATES_PER_COLLAPSE 25
#define START_SPEED 30 */

#define URL "http://192.168.1.20:8001" //office pc

// Params for width and height
const uint16_t kMatrixWidth = 32;
const uint16_t kMatrixHeight = 32;

// Param for different pixel layouts
const bool kMatrixSerpentineLayout = true;
const bool kMatrixTopToBottom = false;

#define MAX_BARS 300
#define MAX_BEATS 1200
#define MAX_TATUMS 2400

#endif
