// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define VERSION "3.3.0"
#define AUTHOR "F4HWN"
#define NAME "RRFRemote"

#define DEBUG 0
#define ATOM atom
#define WIDTH displayWidth
#define HEIGHT displayHeight

#define TIMEOUT_BIN_LOADER 3              // 3 sec
#define TIMEOUT_MENU 10 * 1000            // 10 sec
#define TIMEOUT_TOT_RRF 55 * 1000         // 55 sec
#define TIMEOUT_TOT_ELSEWHERE 285 * 1000  // 285 sec, 4min 45sec

#define FASTLED_INTERNAL // To disable pragma messages on compile

#define M5ATOMDISPLAY_LOGICAL_WIDTH WIDTH     // Width
#define M5ATOMDISPLAY_LOGICAL_HEIGHT HEIGHT   // Height
#define M5ATOMDISPLAY_REFRESH_RATE 60         // Refresh rate
#define M5ATOMDISPLAY_OUTPUT_WIDTH 960
#define M5ATOMDISPLAY_OUTPUT_HEIGHT 480

#define M5MODULEDISPLAY_LOGICAL_WIDTH WIDTH   // Width
#define M5MODULEDISPLAY_LOGICAL_HEIGHT HEIGHT // Height
#define M5MODULEDISPLAY_REFRESH_RATE 60       // Refresh rate
#define M5MODULEDISPLAY_OUTPUT_WIDTH 960
#define M5MODULEDISPLAY_OUTPUT_HEIGHT 480

#define SDU_HEADLESS // For Bin Loader

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <FS.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include <FastLED.h>
#include <time.h>

#if ATOM == 1
  #include <M5AtomDisplay.h>
#else
  #include <M5ModuleDisplay.h>
#endif

#include <M5Unified.h>
#include <M5StackUpdater.h>

// Wifi
WiFiClientSecure clientISS;
WiFiClient clientRemote, clientTracker, clientHamSQL, clientWhereis;
WiFiClient httpClient;
WiFiServer httpServer(80);

// Web site Screen Capture stuff
#define GET_unknown 0
#define GET_index_page 1
#define GET_screenshot 2

// Display
uint16_t offsetX = 0;
uint16_t offsetY = 0;

// Flags for button presses via Web site Screen Capture
boolean buttonLeftPressed = false;
boolean buttonCenterPressed = false;
boolean buttonRightPressed = false;

// LED
#define FASTLED_INTERNAL // To disable pragma messages on compile
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

// Timezone
const char *ntpServer = "pool.ntp.org";
const char *ntpTimeZone = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"; // For Europe/Paris
// const char* ntpTimeZone = "CET-1CEST,M3.5.0,M10.5.0/3"; // For Europe/Brussels
// const char* ntpTimeZone = "EET-2EEST,M3.5.0/3,M10.5.0/4"; // For Europe/Sofia
// const char* ntpTimeZone = "EST5EDT,M3.2.0,M11.1.0"; // For America/Montreal
// const char* ntpTimeZone = "AST4"; // For America/Martinique
// const char* ntpTimeZone = "AST4"; // For America/Guadeloupe
// const char* ntpTimeZone = "NCT-11"; // For Pacific/Noumea

int utc = 1;

// Refresh temporisation
#define REFRESH 500

// Preferences
Preferences preferences;

// Color https://www.rapidtables.com/web/color/RGB_Color.html
// Color https://color.adobe.com/fr/create/color-wheel
// Color https://visme.co/blog/fr/palette-de-couleur/

typedef struct __attribute__((__packed__))
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} colorType;

uint16_t TFT_HEADER = 0x0000;
uint16_t TFT_INFO = 0x0000;
uint16_t TFT_FRONT = 0x0000;
uint16_t TFT_BACK = 0x0000;

#define TFT_MENU_BORDER M5.Displays(display).color565(115, 135, 159)
#define TFT_MENU_BACK M5.Displays(display).color565(24, 57, 92)
#define TFT_MENU_SELECT M5.Displays(display).color565(255, 255, 255)

// http://www.rinkydinkelectronics.com/calc_rgb565.php
// #undef TFT_BLACK
// #define TFT_BLACK 0x2945
// #define TFT_BLACKdisplay.color565(40, 40, 40)

const colorType TFT_HEADER_ROUGE = {152, 0, 0};
const colorType TFT_INFO_ROUGE = {0, 0, 0};
const colorType TFT_FRONT_ROUGE = {255, 104, 104};
const colorType TFT_BACK_ROUGE = {114, 0, 32};

const colorType TFT_HEADER_ROSE = {200, 0, 104};
const colorType TFT_INFO_ROSE = {0, 0, 0};
const colorType TFT_FRONT_ROSE = {255, 104, 176};
const colorType TFT_BACK_ROSE = {120, 0, 72};

const colorType TFT_HEADER_VIOLET = {96, 48, 112};
const colorType TFT_INFO_VIOLET = {0, 0, 0};
const colorType TFT_FRONT_VIOLET = {160, 104, 184};
const colorType TFT_BACK_VIOLET = {64, 32, 72};

const colorType TFT_HEADER_BLEU = {0, 56, 120};
const colorType TFT_INFO_BLEU = {0, 0, 0};
const colorType TFT_FRONT_BLEU = {56, 114, 168};
const colorType TFT_BACK_BLEU = {0, 56, 104};

const colorType TFT_HEADER_TURQUOISE = {0, 104, 104};
const colorType TFT_INFO_TURQUOISE = {0, 0, 0};
const colorType TFT_FRONT_TURQUOISE = {56, 184, 184};
const colorType TFT_BACK_TURQUOISE = {0, 64, 64};

const colorType TFT_HEADER_VERT = {0, 104, 56};
const colorType TFT_INFO_VERT = {0, 0, 0};
const colorType TFT_FRONT_VERT = {56, 176, 144};
const colorType TFT_BACK_VERT = {0, 64, 32};

const colorType TFT_HEADER_ORANGE = {255, 64, 0};
const colorType TFT_INFO_ORANGE = {0, 0, 0};
const colorType TFT_FRONT_ORANGE = {255, 157, 77};
const colorType TFT_BACK_ORANGE = {230, 108, 44};

const colorType TFT_HEADER_MARRON = {120, 80, 72};
const colorType TFT_INFO_MARRON = {0, 0, 0};
const colorType TFT_FRONT_MARRON = {160, 136, 128};
const colorType TFT_BACK_MARRON = {96, 64, 56};

const colorType TFT_HEADER_GRIS = {96, 96, 96};
const colorType TFT_INFO_GRIS = {0, 0, 0};
const colorType TFT_FRONT_GRIS = {160, 160, 160};
const colorType TFT_BACK_GRIS = {64, 64, 64};

const colorType TFT_HEADER_CREPUSCULE = {92, 74, 114};
const colorType TFT_INFO_CREPUSCULE = {163, 88, 109};
const colorType TFT_FRONT_CREPUSCULE = {243, 176, 90};
const colorType TFT_BACK_CREPUSCULE = {244, 135, 75};

const colorType TFT_HEADER_FORET = {65, 89, 57};
const colorType TFT_INFO_FORET = {63, 50, 50};
const colorType TFT_FRONT_FORET = {181, 193, 180};
const colorType TFT_BACK_FORET = {116, 89, 61};

const colorType TFT_HEADER_ARDOISE = {84, 103, 143};
const colorType TFT_INFO_ARDOISE = {64, 64, 64};
const colorType TFT_FRONT_ARDOISE = {145, 153, 190};
const colorType TFT_BACK_ARDOISE = {108, 107, 116};

const colorType TFT_HEADER_FLORAL = {43, 25, 62};
const colorType TFT_INFO_FLORAL = {79, 114, 142};
const colorType TFT_FRONT_FLORAL = {238, 108, 129};
const colorType TFT_BACK_FLORAL = {141, 45, 86};

colorType BMP_HEADER;
colorType BMP_INFO;
colorType BMP_FRONT;
colorType BMP_BACK;

// Icon
#define ICON_FONT &icon_works_webfont14pt7b
#define ICON_STAT 40
#define ICON_CLOCK 105
#define ICON_CALL 77
#define ICON_TOT 105
#define ICON_SETTING 106
#define ICON_LEFT 119
#define ICON_RIGHT 87
#define ICON_ISS 51
#define ICON_BAT100 118
#define ICON_BAT075 117
#define ICON_BAT050 116
#define ICON_BAT025 115
#define ICON_BAT000 34
#define ICON_CHARGING 37

// HTTP endpoint
String endpointHamQSL = "http://rrf.globalis-dev.com:8080/html/solarxml.xml";
String endpointISS = "https://api.wheretheiss.at/v1/satellites/25544";

String endpointRRF[] = {
    "http://rrf.globalis-dev.com:8080/RRFTracker/RRF-today/rrf_tiny.json",
    "http://rrf.globalis-dev.com:8080/RRFTracker/FON-today/rrf_tiny.json",
    "http://rrf.globalis-dev.com:8080/RRFTracker/TECHNIQUE-today/rrf_tiny.json",
    "http://rrf.globalis-dev.com:8080/RRFTracker/INTERNATIONAL-today/rrf_tiny.json",
    "http://rrf.globalis-dev.com:8080/RRFTracker/BAVARDAGE-today/rrf_tiny.json",
    "http://rrf.globalis-dev.com:8080/RRFTracker/LOCAL-today/rrf_tiny.json",
    "http://rrf.globalis-dev.com:8080/RRFTracker/IDF-today/rrf_tiny.json"
    };

const char *room[] = {"RRF", "FON", "TECHNIQUE", "INTERNATIONAL", "BAVARDAGE", "LOCAL", "IDF"};
const uint8_t dtmf[] = {96, 97, 98, 99, 100, 101, 104};

// Scroll
LGFX_Sprite Sprite(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object
String message;
int16_t pos = 0;

// Bin Loader && Ini Loader
#define NUMBER_OF_FILENAME 64
#define MAX_FILENAME_SIZE 64

File root;
char fileName[NUMBER_OF_FILENAME][MAX_FILENAME_SIZE];
uint8_t fileIndex = 0;

// Task Handle
TaskHandle_t rrfdataHandle;
TaskHandle_t buttonHandle;

// Misceleanous
char **settingsMenu;
char swap[32];

String tmpString;

String jsonData = "", issData = "", xmlData = "", whereisData = "";
String jsonDataNew = "", issDataNew = "";

String lastString;
String dateString, dateStringOld;
String dureeString, dureeStringOld;
String emissionString, emissionStringOld;
String linkTotalString, linkTotalStringOld;
String linkActifString, linkActifStringOld;
String txTotalString, txTotalStringOld;
String elsewhereString, elsewhereStringOld;
String baselineString, baselineStringOld;
String issString, issStringOld;
String optionString, optionStringOld;
String titleString, titleStringOld;

String indicatifString;
String whereisString;

boolean settingsMode = false;
boolean settingLock = true;
boolean reset = 0;
boolean refresh = 0;
boolean menuRefresh = 0;
boolean screensaverMode = 0;
boolean wifiConnected = 0;

int8_t menuSize;
int8_t menuMode = 0;
int8_t menuSelected = -1;
int8_t menuCurrent = 0;
int8_t colorCurrent = 0;
int8_t configCurrent = 0;
int8_t sysopCurrent = 0;
int8_t roomCurrent = 0;
int8_t modeCurrent = 1;
int8_t hdmiCurrent = 0;
int8_t followCurrent = 0;
int8_t screensaverCurrent = 0;
int8_t issCurrent = 0;
int8_t totCurrent = 0;
int8_t raptorCurrent = 0;
int8_t brightnessCurrent = 32;
int8_t beepCurrent = 32;
int8_t modeOld = 1;
int8_t modeNew = 1;
int8_t display = 0;

uint8_t htmlGetRequest;
uint8_t htmlGetRefresh = 3;
uint8_t alternance = 0;
uint8_t type = 0;
uint8_t action = 0;
uint8_t ping = 0;
uint8_t dst;

uint8_t transmitOn = 0;
uint8_t transmitOff = 0;
uint8_t whereisCurrent = 0;
uint8_t tempCurrent = 0;
uint8_t batteryChargeCurrent = 0;
uint8_t batteryLevelCurrent = 0;

uint16_t qsy = 0;
uint16_t issDistance = 0;

uint32_t screensaver;

#undef SPI_READ_FREQUENCY
#define SPI_READ_FREQUENCY 40000000

// Menu
const char *menuSpotnikOn[] = {(char*)"Config", (char*)"Sysop", (char*)"Perroquet", (char*)"QSY", (char*)"Follow", (char*)"Raptor", (char*)"TOT", (char*)"ISS", (char*)"Affichage", (char*)"Themes", (char*)"Luminosite", (char*)"Beep", (char*)"Screensaver", (char*)"HDMI", (char*)"Addresse IP", (char*)"Eteindre", (char*)"Exit"};
const char *menuSpotnikOff[] = {(char*)"Config",(char*)"TOT", (char*)"ISS", (char*)"Affichage", (char*)"Themes", (char*)"Luminosite", (char*)"Beep", (char*)"Screensaver", (char*)"HDMI", (char*)"Addresse IP", (char*)"Eteindre", (char*)"Exit"};

const char *choiceSysop[] = {"REBOOT", "IP", "SCAN RAPIDE", "LIBRE"};
const char *choicePerroquet[] = {"OFF", "ON"};
const char *choiceFollow[] = {"OFF", "ON"};
const char *choiceRaptor[] = {"OFF", "ON"};
const char *choiceTOT[] = {"OFF", "ON"};
const char *choiceISS[] = {"OFF", "ON"};
const char *choiceThemes[] = {"ROUGE", "ROSE", "VIOLET", "BLEU", "TURQUOISE", "VERT", "ORANGE", "MARRON", "GRIS", "CREPUSCULE", "FORET", "ARDOISE", "FLORAL"};
const char *choiceHDMI[] = {"OFF", "ON"};
const char *choiceAffichage[] = {"EXPERT", "BASIC"};
