#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <Ticker.h>
#include "font.h"

// Version

#define VERSION 0.1

// Debug

#define DEBUG false

// Color (https://htmlcolorcodes.com/fr/)

#define COLOR_BACK_R 48
#define COLOR_BACK_G 48
#define COLOR_BACK_B 48

/*
// Orange
#define COLOR_FRONT_R   245
#define COLOR_FRONT_G   176
#define COLOR_FRONT_B   65

#define COLOR_HEADER_R  186
#define COLOR_HEADER_G  74
#define COLOR_HEADER_B  0
*/

/*
// Vert
#define COLOR_FRONT_R   52
#define COLOR_FRONT_G   174
#define COLOR_FRONT_B   96

#define COLOR_HEADER_R  20
#define COLOR_HEADER_G  90
#define COLOR_HEADER_B  50
*/

/*
// Rouge
#define COLOR_FRONT_R   231
#define COLOR_FRONT_G   76
#define COLOR_FRONT_B   60

#define COLOR_HEADER_R  120
#define COLOR_HEADER_G  40
#define COLOR_HEADER_B  31
*/

// Bleu

#define COLOR_FRONT_R 52
#define COLOR_FRONT_G 152
#define COLOR_FRONT_B 219

#define COLOR_HEADER_R 27
#define COLOR_HEADER_G 79
#define COLOR_HEADER_B 114

// Icon

#define ICON_FONT &icon_works_webfont14pt7b
#define ICON_STAT 40
#define ICON_CLOCK 105
#define ICON_CALL 77
#define ICON_TOT 105
#define ICON_SETTING 106
#define ICON_BAT100 118
#define ICON_BAT075 117
#define ICON_BAT050 116
#define ICON_BAT025 115
#define ICON_BAT000 34
#define ICON_CHARGING 37

/*
#defile ICON_FONT       &PeaxDrawnIcons20pt7b
#define ICON_STAT       86
#define ICON_CLOCK      51
#define ICON_CALL       104
#define ICON_TOT        50
#define ICON_SETTING    35
#define ICON_BAT100     118
#define ICON_BAT075     117
#define ICON_BAT050     116 
#define ICON_BAT025     115
#define ICON_BAT000     34
#define ICON_CHARGING   37
*/

// Wifi

const char *ssid = "\\_(o)_/";
const char *password = "petitchaton";
WiFiClient client;

// HTTP endpoint

String spotnik = "http://192.168.1.99:3000/";

String endpoint[] = {
    "http://rrf.f5nlg.ovh:8080/RRFTracker/RRF-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/TECHNIQUE-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/BAVARDAGE-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/LOCAL-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/INTERNATIONAL-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/FON-today/rrf_tiny.json"};

Ticker tickerHTTP;

// Scroll

TFT_eSprite img = TFT_eSprite(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object
int pos;
String msg = "";

// Misceleanous
String tmp_str;
String json_data;
String json_data_new;
String doc_old_serialize;

String date_str, date_str_old;
String emission_str, emission_str_old;
String link_total_str, link_total_str_old;
String tx_total_str, tx_total_str_old;

int room_current = 0;
int transmit_on = 0;
int transmit_off = 0;
int reset = 0;
int alternance = 0;
int refresh = 0;
int type = 0;
int battery_current = -1;
int qsy = 0;

unsigned long timer = 0, wait = 0, limit = 750;

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

int interpolation(int value, int in_min, int in_max, int out_min, int out_max)
{
  if ((in_max - in_min) != 0)
  {
    return int((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
  }
  else
  {
    return 0;
  }
}

// Clear screen

void clear()
{
  msg = "";
  M5.lcd.clear();

  // Loading
  /*
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.drawString("QSY", 160, 60);
  */

  // Grey zone
  M5.Lcd.drawLine(0, 100, 320, 100, TFT_WHITE);
  M5.Lcd.fillRect(0, 101, 320, 239, M5.Lcd.color565(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B));

  // Elsewhere
  M5.Lcd.drawRoundRect(0, 169, 150, 70, 4, TFT_WHITE);
  M5.Lcd.fillRoundRect(1, 170, 148, 68, 4, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));

  // Log
  M5.Lcd.drawRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
  M5.Lcd.fillRoundRect(161, 118, 158, 120, 4, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));

  M5.Lcd.drawFastVLine(25, 170, 130, TFT_WHITE);
  M5.Lcd.drawFastVLine(80, 170, 130, TFT_WHITE);

  M5.Lcd.drawFastHLine(0, 182, 149, TFT_WHITE);
  M5.Lcd.drawFastHLine(0, 196, 149, TFT_WHITE);
  M5.Lcd.drawFastHLine(0, 210, 149, TFT_WHITE);
  M5.Lcd.drawFastHLine(0, 224, 149, TFT_WHITE);
}

// Scroll
void build_scroll()
{
  int h = 20;
  int w = M5.Lcd.width();

  // We could just use fillSprite(color) but lets be a bit more creative...

  while (h--)
    img.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  img.setTextSize(1);          // Font size scaling is x1
  img.setTextFont(2);          // Font 4 selected
  img.setTextColor(TFT_WHITE); // Black text, no background colour
  img.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  img.setCursor(pos, 2); // Print text at xpos
  img.print(msg);

  img.setCursor(pos - w, 2); // Print text at xpos - sprite width
  img.print(msg);
}

void scroll()
{
  // Sprite for scroll

  build_scroll();
  img.pushSprite(0, 80);

  pos -= 1;
  if (pos == 0)
  {
    pos = M5.Lcd.width();
  }
}

void task(void *pvParameters)
{
  HTTPClient http;

  for (;;)
  {
    timer = millis();

    if (room_current < 0)
    {
      room_current = 5;
    }
    else if (room_current > 5)
    {
      room_current = 0;
    }

    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {                                             

      if (qsy == 1) {
        const char *dtmf[] = {"96", "98", "100", "101", "99", "97"};

        //Serial.println("Push");
        //Serial.flush();

        http.begin(client, spotnik + String("?dtmf=") + String(dtmf[room_current])); // Specify the URL
        http.addHeader("Content-Type", "text/plain");             //Specify content-type header
        http.setTimeout(500);
        int httpCode = http.GET();  // Make the request
        if (httpCode < 0)
        {                                 
          qsy = 0;
        }
      }

      http.begin(client, endpoint[room_current]); // Set URL
      http.addHeader("Content-Type", "text/plain");             //Specify content-type header
      http.setTimeout(500);                       // Set Time Out
      int httpCode = http.GET();                  // Make the request

      if (httpCode > 0)
      {                                   // Check for the returning code
        json_data_new = http.getString(); // Get data
        http.end();                       // Free the resources
      }
    }

    wait = millis() - timer;
    if (wait < limit)
    {
      if (DEBUG)
      {
        char tmp[40];
        sprintf(tmp, "Timer : %u", int(limit - wait));
        Serial.println(tmp);
      }
      vTaskDelay(pdMS_TO_TICKS(limit - wait));
    }
  }
}

// Setup

void setup()
{
  // Init screen

  M5.begin(true, false, true, true);
  M5.Power.begin();
  M5.Speaker.mute();

  M5.Lcd.setBrightness(32);

  M5.Lcd.fillScreen(M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
  M5.Lcd.setTextDatum(CC_DATUM);

  // Scroll

  pos = M5.Lcd.width();
  img.createSprite(M5.Lcd.width(), 20);

  // Title

  M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
  M5.Lcd.drawString("RRFRemote", 160, 20);

  M5.Lcd.setFreeFont(0);
  M5.Lcd.drawString("Version 0.1 by F4HWN", 160, 50);

  // We start by connecting to a WiFi network

  tmp_str = ssid;

  M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
  M5.Lcd.drawString(tmp_str, 160, 140);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  M5.Lcd.setFreeFont(0);
  tmp_str = WiFi.localIP().toString().c_str();
  M5.Lcd.drawString(tmp_str, 160, 170);

  xTaskCreatePinnedToCore(
      task,   /* Function to implement the task */
      "task", /* Name of the task */
      8192,   /* Stack size in words */
      NULL,   /* Task input parameter */
      2,      /* Priority of the task */
      NULL,   /* Task handle. */
      0);     /* Core where the task should run */

  delay(1000);

  clear();
}

// Button 
void button() {
  M5.update();  // Read the state of button

  if(M5.BtnA.isPressed() && M5.BtnC.isPressed()) {
    M5.Power.deepSleep();
  } else {
    if (M5.BtnA.wasPressed()) {
      room_current -= 1;
      reset = 0;
    } else if (M5.BtnC.wasPressed()) {
      room_current += 1;
      reset = 0;
    } else if (M5.BtnB.wasPressed()) {
      qsy = 1;
    }
  }
}