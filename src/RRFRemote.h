// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Board
#define BOARD GREY

#define BASIC 1
#define GREY  2
#define CORE2 3

#if BOARD == BASIC
  #define M5STACK_MPU6886
  #include <M5Stack.h>
  #include "BasicAndGrey.h"
#elif BOARD == GREY
  #define M5STACK_MPU6886
  #include <M5Stack.h>
  #include "BasicAndGrey.h"
#elif BOARD == CORE2
  #include <M5Core2.h>
  #include "Core2.h"
#endif

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "time.h"
#include "font.h"
#include "settings.h"

// Version
#define VERSION "2.3.9"

// Wifi
WiFiClient clientRemote, clientTracker, clientHamSQL, clientWhereis;
WiFiClientSecure clientISS;

// Timezone
const char* ntpServer = "pool.ntp.org";
const int8_t utc = 1;       // UTC + 1 for France
const int8_t daylight = 1;  // If your country observes Daylight saving time set it to 1. Otherwise, set it to 0.

// Temporisation
#define LIMIT 500

// Preferences
Preferences preferences;

// Color (https://www.rapidtables.com/web/color/RGB_Color.html)
// Color (https://color.adobe.com/fr/create/color-wheel)
typedef struct __attribute__((__packed__))
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} colorType;

//colorType TFT_BACK = {48, 48, 48};
colorType TFT_BACK = {117, 0, 76};
colorType TFT_GRAY = {128, 128, 128};
colorType TFT_FRONT = {51, 153, 255};
colorType TFT_HEADER = {0, 76, 153};

const char *color[] = {"ROUGE", "ORANGE", "VERT", "TURQUOISE", "BLEU", "ROSE", "VIOLET", "GRIS"};

const colorType TFT_FRONT_ROUGE = {255, 102, 102};
const colorType TFT_HEADER_ROUGE = {153, 0, 0};
const colorType TFT_BACK_ROUGE = {117, 0, 31};

const colorType TFT_FRONT_ORANGE = {255, 157, 77};
const colorType TFT_HEADER_ORANGE = {255, 116, 0};
const colorType TFT_BACK_ORANGE = {217, 65, 0};

const colorType TFT_FRONT_VERT = {51, 204, 102};
const colorType TFT_HEADER_VERT = {0, 102, 51};
const colorType TFT_BACK_VERT = {0, 64, 32};

const colorType TFT_FRONT_TURQUOISE = {51, 204, 204};
const colorType TFT_HEADER_TURQUOISE = {0, 102, 102};
const colorType TFT_BACK_TURQUOISE = {0, 64, 64};

const colorType TFT_FRONT_BLEU = {51, 110, 170};
const colorType TFT_HEADER_BLEU = {0, 57, 115};
const colorType TFT_BACK_BLEU = {0, 51, 102};

const colorType TFT_FRONT_ROSE = {255, 102, 178};
const colorType TFT_HEADER_ROSE = {204, 0, 102};
const colorType TFT_BACK_ROSE = {117, 0, 76};

const colorType TFT_FRONT_VIOLET = {165, 105, 189};
const colorType TFT_HEADER_VIOLET = {91, 44, 111};
const colorType TFT_BACK_VIOLET = {61, 30, 74};

const colorType TFT_FRONT_GRIS = {160, 160, 160};
const colorType TFT_HEADER_GRIS = {96, 96, 96};
const colorType TFT_BACK_GRIS = {64, 64, 64};

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
String endpointHamQSL = "http://www.hamqsl.com/solarxml.php";
String endpointISS = "https://api.wheretheiss.at/v1/satellites/25544";

String endpointRRF[] = {
    "http://rrf.f5nlg.ovh:8080/RRFTracker/RRF-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/TECHNIQUE-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/BAVARDAGE-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/LOCAL-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/INTERNATIONAL-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/FON-today/rrf_tiny.json"};

// Scroll
TFT_eSprite img = TFT_eSprite(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object
String message = "";
int16_t pos;

// Misceleanous
const char *room[] = {"RRF", "TECHNIQUE", "BAVARDAGE", "LOCAL", "INTERNATIONAL", "FON"};
const uint8_t dtmf[] = {96, 98, 100, 101, 99, 97};
const char *menuSpotnikOn[]  = {"CONFIG", "QSY", "FOLLOW", "RAPTOR", "PERROQUET", "SYSOP", "TOT", "ISS", "COULEUR", "LUMINOSITE", "QUITTER"};
const char *menuSpotnikOff[] = {"CONFIG", "TOT", "ISS", "COULEUR", "LUMINOSITE", "QUITTER"};
const char *sysop[] = {"REBOOT", "IP", "SCAN RAPIDE", "LIBRE"};
char **menu;

String tmpString;

String jsonData = "", issData = "", xmlData = "", whereisData = "";
String jsonDataNew = "", issDataNew = "";

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

bool reset = 0;
bool refresh = 0;
bool menuRefresh = 0;
bool screensaverMode = 0;

uint16_t qsy = 0;
uint16_t issDistance = 0;

int8_t menuSize;
int8_t menuMode = 0;
int8_t menuSelected = -1;
int8_t menuCurrent = 0;
int8_t colorCurrent = 0;
int8_t configCurrent = 0;
int8_t sysopCurrent = 0;
int8_t roomCurrent = 0;

uint8_t alternance = 0;
uint8_t type = 0;
uint8_t action = 0; 

uint8_t transmitOn = 0;
uint8_t transmitOff = 0;
uint8_t whereisCurrent = 0;
uint8_t brightnessCurrent = 32;
uint8_t totCurrent = 0;
uint8_t issCurrent = 0;
uint8_t raptorCurrent = 0;
uint8_t followCurrent = 0;
uint8_t batteryChargeCurrent = 0;
uint8_t batteryLevelCurrent = 0;

uint32_t screensaver;
uint32_t screensaverLimit = 5 * 60 * 1000;  // 5 minutes