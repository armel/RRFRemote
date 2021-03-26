// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Board
#define BOARD BASIC

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

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "font.h"
#include "settings.h"

// Version
#define VERSION "2.0.5"

// Wifi
WiFiClient clientRemote, clientTracker, clientHamSQL, clientWhereis;

// Preferences
Preferences preferences;

// Color (https://www.rapidtables.com/web/color/RGB_Color.html)
typedef struct __attribute__((__packed__))
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} colorType;

colorType TFT_BACK = {48, 48, 48};
colorType TFT_GRAY = {128, 128, 128};
colorType TFT_FRONT = {51, 153, 255};
colorType TFT_HEADER = {0, 76, 153};

const char *color[] = {"ROUGE", "ORANGE", "VERT", "TURQUOISE", "BLEU", "ROSE", "VIOLET", "GRIS"};

int colorCurrent = 0;

const colorType TFT_FRONT_ROUGE = {255, 102, 102};
const colorType TFT_HEADER_ROUGE = {153, 0, 0};

const colorType TFT_FRONT_ORANGE = {255, 178, 102};
const colorType TFT_HEADER_ORANGE = {255, 128, 0};

const colorType TFT_FRONT_VERT = {51, 204, 102};
const colorType TFT_HEADER_VERT = {0, 102, 51};

const colorType TFT_FRONT_TURQUOISE = {51, 204, 204};
const colorType TFT_HEADER_TURQUOISE = {0, 102, 102};

const colorType TFT_FRONT_BLEU = {51, 153, 255};
const colorType TFT_HEADER_BLEU = {0, 76, 153};

const colorType TFT_FRONT_ROSE = {255, 102, 178};
const colorType TFT_HEADER_ROSE = {204, 0, 102};

const colorType TFT_FRONT_VIOLET = {165, 105, 189};
const colorType TFT_HEADER_VIOLET = {91, 44, 111};

const colorType TFT_FRONT_GRIS = {192, 192, 192};
const colorType TFT_HEADER_GRIS = {96, 96, 96};

// Icon
#define ICON_FONT &icon_works_webfont14pt7b
#define ICON_STAT 40
#define ICON_CLOCK 105
#define ICON_CALL 77
#define ICON_TOT 105
#define ICON_SETTING 106
#define ICON_LEFT 119
#define ICON_RIGHT 87
#define ICON_BAT100 118
#define ICON_BAT075 117
#define ICON_BAT050 116
#define ICON_BAT025 115
#define ICON_BAT000 34
#define ICON_CHARGING 37

// HTTP endpoint
String endpointHamQSL = "http://www.hamqsl.com/solarxml.php";
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
int pos;

// Misceleanous
const char *room[] = {"RRF", "TECHNIQUE", "BAVARDAGE", "LOCAL", "INTERNATIONAL", "FON"};
const int dtmf[] = {96, 98, 100, 101, 99, 97};
const char *menu[] = {"CONFIG", "QSY", "FOLLOW", "RAPTOR", "PERROQUET", "REBOOT", "TOT", "COULEUR", "LUMINOSITE", "QUITTER"};

String tmpString;
String jsonData = "", xmlData = "", whereisData = "";
String jsonDataNew = "";

String dateString, dateStringOld;
String dureeString, dureeStringOld;
String emissionString, emissionStringOld;
String linkTotalString, linkTotalStringOld;
String linkActifString, linkActifStringOld;
String txTotalString, txTotalStringOld;
String elsewhereString, elsewhereStringOld;
String indicatifString;
String whereisString;
String departmentString;

int reset = 0;
int alternance = 0;
int refresh = 0;
int type = 0;
int qsy = 0;

int transmitOn = 0, transmitOff = 0;

int configCurrent = 0;
int roomCurrent = 0;
int whereisCurrent = 0;
int brightnessCurrent = 32;
int totCurrent = 0;
int raptorCurrent = 0;
int followCurrent = 0;
int batteryChargeCurrent = 0;
int batteryLevelCurrent = 0;
int menuCurrent = 0;
int menuMode = 0;
int menuSelected = -1;
int menuRefresh = 0;

unsigned long screensaver;
int screensaverLimit = 5 * 60 * 1000;  // 5 minutes
int screensaverMode = 0;