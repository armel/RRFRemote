// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Board

#define BOARD BASIC

#define BASIC 1
#define GREY  2
#define CORE2 3

#if BOARD == BASIC
  #include <M5Stack.h>
  #include "BasicAndGrey.h"
#elif BOARD == GREY
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
#define VERSION "1.1.8"

// Wifi
WiFiClient clientRemote, clientTracker, clientHamSQL, clientWhereis;
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// Indicatif
String indicatif = INDICATIF;

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
String endpointSpotnik = SPOTNIK;
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
const char *menu[] = {"QSY", "RAPTOR", "PERROQUET", "FOLLOW", "COULEUR", "LUMINOSITE", "QUITTER"};

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
String whereisString = "";

int reset = 0;
int alternance = 0;
int refresh = 0;
int type = 0;
int qsy = 0;

int transmitOn = 0, transmitOff = 0;

int roomCurrent = 0;
int whereisCurrent = 0;
int brightnessCurrent = 32;
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

// Parse data
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Compute interpolation
int interpolation(int value, int inMin, int inMax, int outMin, int outMax)
{
  if ((inMax - inMin) != 0)
  {
    return int((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
  }
  else
  {
    return 0;
  }
}

// Reset color
void resetColor()
{
  switch(colorCurrent) {
    case 0: TFT_FRONT = TFT_FRONT_ROUGE; TFT_HEADER = TFT_HEADER_ROUGE; break;
    case 1: TFT_FRONT = TFT_FRONT_ORANGE; TFT_HEADER = TFT_HEADER_ORANGE; break;
    case 2: TFT_FRONT = TFT_FRONT_VERT; TFT_HEADER = TFT_HEADER_VERT; break;
    case 3: TFT_FRONT = TFT_FRONT_TURQUOISE; TFT_HEADER = TFT_HEADER_TURQUOISE; break;
    case 4: TFT_FRONT = TFT_FRONT_BLEU; TFT_HEADER = TFT_HEADER_BLEU; break;
    case 5: TFT_FRONT = TFT_FRONT_ROSE; TFT_HEADER = TFT_HEADER_ROSE; break;
    case 6: TFT_FRONT = TFT_FRONT_VIOLET; TFT_HEADER = TFT_HEADER_VIOLET; break;
    case 7: TFT_FRONT = TFT_FRONT_GRIS; TFT_HEADER = TFT_HEADER_GRIS; break;
  }
}

// Clear screen
void clear()
{
  // Reset
  message = "";
  M5.lcd.clear();
  resetColor();

  // Header
  M5.Lcd.fillRect(0, 0, 320, 44, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
  M5.Lcd.drawFastHLine(  0,  0, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(  0, 44, 320, TFT_WHITE);

  // Grey zone
  M5.Lcd.drawLine(0, 100, 320, 100, TFT_WHITE);
  M5.Lcd.fillRect(0, 101, 320, 239, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

  // Elsewhere
  M5.Lcd.fillRoundRect(0, 168, 150, 71, 4, TFT_WHITE);
  M5.Lcd.fillRoundRect(1, 169, 24, 69, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));

  M5.Lcd.drawFastVLine(22, 169, 69, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
  M5.Lcd.drawFastVLine(23, 169, 69, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
  M5.Lcd.drawFastVLine(24, 169, 69, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));

  M5.Lcd.drawFastVLine(25, 169, 69, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastVLine(96, 169, 69, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

  M5.Lcd.drawFastHLine(2, 182, 147, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastHLine(2, 196, 147, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastHLine(2, 210, 147, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastHLine(2, 224, 147, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

  // Log
  M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
}

// Manage buttons

void button()
{
  static int btnALast = 0;
  static int btnBLast = 0;
  static int btnCLast = 0;

  // Manage button bump
  if(btnB && btnBLast) {
    btnB = 0;
  }
  btnBLast = btnB;

  if(menuMode == 0) {
    if(btnA && btnALast) {
      btnA = 0;
    }
    btnALast = btnA;

    if(btnC && btnCLast) {
      btnC = 0;
    }
    btnCLast = btnC;
  }

  // Manage screensaver
  if (screensaverMode == 1)
  {
    if (btnA || btnB || btnC)
    {
      screensaver = millis(); // Screensaver update !!!
    }
  }
  else
  { // Mode menu inactive
    if (menuMode == 0)
    {
      if(followCurrent == 0) {
        if (btnA && followCurrent == 0)
        {
          screensaver = millis(); // Screensaver update !!!
          roomCurrent -= 1;
          refresh = 0;
          reset = 0;

          roomCurrent = (roomCurrent < 0) ? 5 : roomCurrent;
          roomCurrent = (roomCurrent > 5) ? 0 : roomCurrent;
          preferences.putUInt("room", roomCurrent);
        }
        else if (btnC && followCurrent == 0)
        {
          screensaver = millis(); // Screensaver update !!!
          roomCurrent += 1;
          refresh = 0;
          reset = 0;

          roomCurrent = (roomCurrent < 0) ? 5 : roomCurrent;
          roomCurrent = (roomCurrent > 5) ? 0 : roomCurrent;
          preferences.putUInt("room", roomCurrent);
        }
      }
      if (btnB)
      {
        screensaver = millis(); // Screensaver update !!!
        menuMode = 1;
        menuRefresh = 0;
      }
    }
    else
    { // Mode menu active, no selection
      if (menuSelected == -1)
      {
        if (btnA)
        {
          menuCurrent -= 1;
        }
        else if (btnC)
        {
          menuCurrent += 1;
        }
        else if (btnB)
        {
          menuSelected = menuCurrent;
          menuRefresh = 0;
        }

        menuCurrent = (menuCurrent < 0) ? 6 : menuCurrent;
        menuCurrent = (menuCurrent > 6) ? 0 : menuCurrent;
        preferences.putUInt("menu", menuCurrent);
      }
      else
      {
        // Mode menu active, QSY
        if (menuSelected == 0)
        {
          qsy = dtmf[roomCurrent];
         
          menuMode = 0;
          reset = 0;
          refresh = 0;        
        }
        // Mode menu active, Raptor
        else if (menuSelected == 1)
        {
          qsy = 200;
          raptorCurrent = (raptorCurrent == 0) ? 1 : 0;
          
          menuMode = 0;
          reset = 0;
          refresh = 0;
        }
        // Mode menu active, Parrot
        else if (menuSelected == 2)
        {
          qsy = 95;

          menuMode = 0;
          reset = 0;
          refresh = 0;
        }
        // Mode menu active, Follow
        else if (menuSelected == 3)
        {
          followCurrent = (followCurrent == 0) ? 1 : 0;

          menuMode = 0;
          reset = 0;
          refresh = 0;

          preferences.putUInt("follow", followCurrent);
        }
        // Mode menu active, Color
        else if (menuSelected == 4)
        {
          int change = 0;
          if (btnA)
          {
            colorCurrent -= 1;
            change = 1;
          }
          else if (btnC)
          {
            colorCurrent += 1;
            change = 1;
          }
          else if (btnB)
          {
            menuMode = 0;
            reset = 0;
            refresh = 0;
          }

          colorCurrent = (colorCurrent < 0) ? 7 : colorCurrent;
          colorCurrent = (colorCurrent > 7) ? 0 : colorCurrent;

          if(change == 1) {
            clear();
            menuRefresh = 0;
            refresh = 0;
            change = 0;
          }

          preferences.putUInt("color", colorCurrent);
        }
        // Mode menu active, Brightness
        else if (menuSelected == 5)
        {
          if (btnA)
          {
            brightnessCurrent -= 1;
          }
          else if (btnC)
          {
            brightnessCurrent += 1;
          }
          else if (btnB)
          {
            menuMode = 0;
            reset = 0;
            refresh = 0;
          }

          brightnessCurrent = (brightnessCurrent < 10) ? 10 : brightnessCurrent;
          brightnessCurrent = (brightnessCurrent > 128) ? 128 : brightnessCurrent;
          preferences.putUInt("brightness", brightnessCurrent);
          M5.Lcd.setBrightness(brightnessCurrent);
        }
        // Mode menu active, Escape
        else if (menuSelected == 6)
        {
          menuMode = 0;
          reset = 0;
          refresh = 0;
        }
      }
    }
  }

  // Clean current value
  btnA = 0;
  btnB = 0;
  btnC = 0;
}

// Build scroll
void buildScroll()
{
  int h = 20;
  int w = M5.Lcd.width();

  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    img.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  img.setTextSize(1);          // Font size scaling is x1
  img.setTextFont(2);          // Font 2 selected
  img.setTextColor(TFT_WHITE); // White text, no background colour
  img.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  img.setCursor(pos, 2); // Print text at pos
  img.print(message);

  img.setCursor(pos - w, 2); // Print text at pos - sprite width
  img.print(message);
}

// Scroll
void scroll(int pause)
{
  if(btnA == 0 && btnB == 0 && btnC == 0) {
    getButton();
  }

  // Sprite for scroll
  buildScroll();
  img.pushSprite(0, 78);

  pos -= 1;
  if (pos == 0)
  {
    pos = M5.Lcd.width();
  }

  delay(pause);
}

// Get data from RRFTracker and manage QSY
void rrftracker(void *pvParameters)
{
  HTTPClient http;
  unsigned long timer = 0, wait = 0, limit = 750; // Retry all 750ms

  for (;;)
  {
    timer = millis();
    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {

      while (qsy > 0)
      {
        http.begin(clientRemote, endpointSpotnik + String("?dtmf=") + String(qsy));  // Specify the URL
        http.addHeader("Content-Type", "text/plain");                                     // Specify content-type header
        http.setTimeout(500);                                                             // Set timeout
        int httpCode = http.GET();                                                        // Make the request
        if (httpCode == 200)                                                              // Check for the returning code
        {
          qsy = 0;
          refresh = 0;
          menuMode = 0;
          whereisCurrent = dtmf[roomCurrent];
          whereisString = String(room[roomCurrent]);
        }
        http.end(); // Free the resources
      }
 
      http.begin(clientTracker, endpointRRF[roomCurrent]);    // Specify the URL
      http.addHeader("Content-Type", "text/plain");                 // Specify content-type header
      http.setTimeout(750);                                         // Set Time Out
      int httpCode = http.GET();                                    // Make the request
      if (httpCode == 200)                                          // Check for the returning code
      {
        jsonDataNew = http.getString(); // Get data
      } 
      http.end(); // Free the resources
    }

    wait = millis() - timer;
    if (wait < limit)
    {
      vTaskDelay(pdMS_TO_TICKS(limit - wait));
    }
  }
}

// Get data from HamQSL
void hamqsl(void *pvParameters)
{
  HTTPClient http;
  unsigned int limitShort = 1 * 60 * 1000; // Retry all minute
  unsigned int limitLong = 60 * 60 * 1000; // Retry all hours

  for (;;)
  {
    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {
      http.begin(clientHamSQL, endpointHamQSL);     // Specify the URL
      http.addHeader("Content-Type", "text/plain");   // Specify content-type header
      http.setTimeout(1000);                          // Set Time Out
      int httpCode = http.GET();                      // Make the request
      if (httpCode == 200)                            // Check for the returning code
      {
        xmlData = http.getString(); // Get data
        http.end(); // Free the resources
        vTaskDelay(pdMS_TO_TICKS(limitLong));
      }
      else {
        http.end(); // Free the resources
        vTaskDelay(pdMS_TO_TICKS(limitShort));
      }
    }
  }
}

// Get data from Spotnik
void whereis(void *pvParameters)
{
  HTTPClient http;
  unsigned long timer = 0, wait = 0, limit = 1 * 10 * 1000; // Retry all 10 seconds

  for (;;)
  {
    timer = millis();
    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {
      http.begin(clientWhereis, endpointSpotnik + String("?dtmf=0")); // Specify the URL
      http.addHeader("Content-Type", "text/plain");                     // Specify content-type header
      http.setTimeout(1000);                                            // Set Time Out
      int httpCode = http.GET();                                        // Make the request
      if (httpCode == 200)                                              // Check for the returning code
      {
        whereisData = http.getString(); // Get data

        whereisCurrent = whereisData.substring(0, whereisData.indexOf(", ")).toInt();

        whereisData = whereisData.substring(whereisData.indexOf(", ") + 2);
        whereisString = whereisData.substring(0, whereisData.indexOf(", "));
        
        whereisData = whereisData.substring(whereisData.indexOf(", ") + 2);

        if(whereisData.substring(0, whereisData.indexOf(", ")) == "OFF") {
          raptorCurrent = 0;
        } else {
          raptorCurrent = 1;
        }
      }
      http.end(); // Free the resources
    }

    wait = millis() - timer;
    if (wait < limit)
    {
      vTaskDelay(pdMS_TO_TICKS(limit - wait));
    }
  }
}