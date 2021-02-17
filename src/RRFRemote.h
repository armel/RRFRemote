#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <Preferences.h>
#include "font.h"
#include "settings.h"

// Version
#define VERSION "1.1.3"

// Wifi
WiFiClient client_rrfremote, client_rrftracker, client_hamqsl, client_whereis;
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// Indicatif
String indicatif = INDICATIF;

// Preferences
Preferences preferences;

// Color (https://htmlcolorcodes.com/fr/)
typedef struct __attribute__((__packed__))
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color_t;

color_t TFT_BACK = {48, 48, 48};
color_t TFT_GRAY = {128, 128, 128};
color_t TFT_FRONT = {52, 152, 219};
color_t TFT_HEADER = {27, 79, 114};

const char *color[] = {"BLEU", "ORANGE", "VERT", "ROUGE", "VIOLET", "GRIS"};
int color_current = 0;

const color_t TFT_FRONT_BLEU = {52, 152, 219};
const color_t TFT_HEADER_BLEU = {27, 79, 114};

const color_t TFT_FRONT_ORANGE = {245, 176, 65};
const color_t TFT_HEADER_ORANGE = {186, 74, 0};

const color_t TFT_FRONT_VERT = {52, 174, 96};
const color_t TFT_HEADER_VERT = {20, 90, 50};

const color_t TFT_FRONT_ROUGE = {231, 76, 60};
const color_t TFT_HEADER_ROUGE = {120, 40, 31};

const color_t TFT_FRONT_VIOLET = {165, 105, 189};
const color_t TFT_HEADER_VIOLET = {91, 44, 111};

const color_t TFT_FRONT_GRIS = {174, 182, 191};
const color_t TFT_HEADER_GRIS = {52, 73, 94};

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
String endpoint_spotnik = SPOTNIK;
String endpoint_hamqsl = "http://www.hamqsl.com/solarxml.php";
String endpoint_rrf[] = {
    "http://rrf.f5nlg.ovh:8080/RRFTracker/RRF-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/TECHNIQUE-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/BAVARDAGE-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/LOCAL-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/INTERNATIONAL-today/rrf_tiny.json",
    "http://rrf.f5nlg.ovh:8080/RRFTracker/FON-today/rrf_tiny.json"};

// Scroll
TFT_eSprite img = TFT_eSprite(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object
String msg = "";
int pos;

// Misceleanous
const char *room[] = {"RRF", "TECHNIQUE", "BAVARDAGE", "LOCAL", "INTERNATIONAL", "FON"};
const int dtmf[] = {96, 98, 100, 101, 99, 97};
const char *menu[] = {"QSY", "RAPTOR", "PERROQUET", "FOLLOW", "COULEUR", "BRIGHTNESS", "SHUTDOWN", "ESCAPE"};

String tmp_str;
String json_data = "", xml_data = "", whereis_data = "";
String json_data_new = "";

String date_str, date_str_old;
String emission_str, emission_str_old;
String link_total_str, link_total_str_old;
String link_actif_str, link_actif_str_old;
String tx_total_str, tx_total_str_old;
String elsewhere_str, elsewhere_str_old;
String whereis_str = "";

int room_current = 0;
int whereis_current = 0;
int brightness_current = 32;
int transmit_on = 0, transmit_off = 0;
int reset = 0;
int alternance = 0;
int refresh = 0;
int type = 0;
int battery_current = -1;
int qsy = 0;
int menu_mode = 0;
int menu_current = 0;
int menu_selected = -1;
int menu_refresh = 0;
int raptor_current = 0;
int follow_current = 0;

unsigned long screensaver;
int screensaver_limit = 5 * 60 * 1000;  // 5 minutes
int screensaver_off = 0;

int btn_a, btn_b, btn_c = 0;

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

// Reset color
void reset_color()
{
  switch(color_current) {
    case 0: TFT_FRONT = TFT_FRONT_BLEU; TFT_HEADER = TFT_HEADER_BLEU; break;
    case 1: TFT_FRONT = TFT_FRONT_ORANGE; TFT_HEADER = TFT_HEADER_ORANGE; break;
    case 2: TFT_FRONT = TFT_FRONT_VERT; TFT_HEADER = TFT_HEADER_VERT; break;
    case 3: TFT_FRONT = TFT_FRONT_ROUGE; TFT_HEADER = TFT_HEADER_ROUGE; break;
    case 4: TFT_FRONT = TFT_FRONT_VIOLET; TFT_HEADER = TFT_HEADER_VIOLET; break;
    case 5: TFT_FRONT = TFT_FRONT_GRIS; TFT_HEADER = TFT_HEADER_GRIS; break;
  }
}

// Clear screen
void clear()
{
  // Reset
  msg = "";
  M5.lcd.clear();
  reset_color();

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
  static int btn_b_last = 0;

  // Manage button bump
  if(btn_b && btn_b_last) {
    btn_b = 0;
  }
  btn_b_last = btn_b;

  // Manage screensaver
  if (screensaver_off == 1)
  {
    if (btn_a || btn_b || btn_c)
    {
      screensaver = millis(); // Screensaver update !!!
    }
  }
  else
  { // Mode menu inactive
    if (menu_mode == 0)
    {
      if(follow_current == 0) {
        if (btn_a && follow_current == 0)
        {
          screensaver = millis(); // Screensaver update !!!
          room_current -= 1;
          refresh = 0;
          reset = 0;
        }
        else if (btn_c && follow_current == 0)
        {
          screensaver = millis(); // Screensaver update !!!
          room_current += 1;
          refresh = 0;
          reset = 0;
        }

        room_current = (room_current < 0) ? 5 : room_current;
        room_current = (room_current > 5) ? 0 : room_current;
        preferences.putUInt("room", room_current);
      }
      if (btn_b)
      {
        screensaver = millis(); // Screensaver update !!!
        menu_mode = 1;
        menu_refresh = 0;
      }
    }
    else
    { // Mode menu active, no selection
      if (menu_selected == -1)
      {
        if (btn_a)
        {
          menu_current -= 1;
        }
        else if (btn_c)
        {
          menu_current += 1;
        }
        else if (btn_b)
        {
          menu_selected = menu_current;
          menu_refresh = 0;
        }

        menu_current = (menu_current < 0) ? 7 : menu_current;
        menu_current = (menu_current > 7) ? 0 : menu_current;
        preferences.putUInt("menu", menu_current);
      }
      else
      {
        // Mode menu active, QSY
        if (menu_selected == 0)
        {
          qsy = dtmf[room_current];
         
          menu_mode = 0;
          reset = 0;
          refresh = 0;        
        }
        // Mode menu active, Raptor
        else if (menu_selected == 1)
        {
          qsy = 200;
          raptor_current = (raptor_current == 0) ? 1 : 0;
          
          menu_mode = 0;
          reset = 0;
          refresh = 0;
        }
        // Mode menu active, Parrot
        else if (menu_selected == 2)
        {
          qsy = 95;

          menu_mode = 0;
          reset = 0;
          refresh = 0;
        }
        // Mode menu active, Follow
        else if (menu_selected == 3)
        {
          follow_current = (follow_current == 0) ? 1 : 0;

          menu_mode = 0;
          reset = 0;
          refresh = 0;

          preferences.putUInt("follow", follow_current);
        }
        // Mode menu active, Color
        else if (menu_selected == 4)
        {
          int change = 0;
          if (btn_a)
          {
            color_current -= 1;
            change = 1;
          }
          else if (btn_c)
          {
            color_current += 1;
            change = 1;
          }
          else if (btn_b)
          {
            menu_mode = 0;
            reset = 0;
            refresh = 0;
          }

          color_current = (color_current < 0) ? 5 : color_current;
          color_current = (color_current > 5) ? 0 : color_current;

          if(change == 1) {
            clear();
            refresh = 0;
            change = 0;
          }

          preferences.putUInt("color", color_current);
        }
        // Mode menu active, Brightness
        else if (menu_selected == 5)
        {
          if (btn_a)
          {
            brightness_current -= 1;
          }
          else if (btn_c)
          {
            brightness_current += 1;
          }
          else if (btn_b)
          {
            menu_mode = 0;
            reset = 0;
            refresh = 0;
          }

          brightness_current = (brightness_current < 10) ? 10 : brightness_current;
          brightness_current = (brightness_current > 128) ? 128 : brightness_current;
          preferences.putUInt("brightness", brightness_current);
          M5.Lcd.setBrightness(brightness_current);
        }
        // Mode menu active, Shutdown
        else if (menu_selected == 6)
        {
          M5.Power.powerOFF();
        }
        // Mode menu active, Escape
        else if (menu_selected == 7)
        {
          menu_mode = 0;
          reset = 0;
          refresh = 0;
        }
      }
    }
  }

  // Clean current value
  btn_a = 0;
  btn_b = 0;
  btn_c = 0;
}

// Build scroll
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

// Scroll
void scroll(int pause)
{
  if(btn_a == 0 && btn_b == 0 && btn_c == 0) {
    M5.update();
    btn_a = M5.BtnA.read();
    btn_b = M5.BtnB.read();
    btn_c = M5.BtnC.read();
  }

  // Sprite for scroll
  build_scroll();
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
        http.begin(client_rrfremote, endpoint_spotnik + String("?dtmf=") + String(qsy));  // Specify the URL
        http.addHeader("Content-Type", "text/plain");                                     // Specify content-type header
        http.setTimeout(500);                                                             // Set timeout
        int httpCode = http.GET();                                                        // Make the request
        if (httpCode == 200)                                                              // Check for the returning code
        {
          qsy = 0;
          refresh = 0;
          menu_mode = 0;
          whereis_current = dtmf[room_current];
          whereis_str = String(room[room_current]);
        }
        http.end(); // Free the resources
      }
 
      http.begin(client_rrftracker, endpoint_rrf[room_current]);    // Specify the URL
      http.addHeader("Content-Type", "text/plain");                 // Specify content-type header
      http.setTimeout(500);                                         // Set Time Out
      int httpCode = http.GET();                                    // Make the request
      if (httpCode == 200)                                          // Check for the returning code
      {
        json_data_new = http.getString(); // Get data
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
  unsigned int limit_short = 1 * 60 * 1000; // Retry all minute
  unsigned int limit_long = 60 * 60 * 1000; // Retry all hours

  for (;;)
  {
    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {
      http.begin(client_hamqsl, endpoint_hamqsl);     // Specify the URL
      http.addHeader("Content-Type", "text/plain");   // Specify content-type header
      http.setTimeout(1000);                          // Set Time Out
      int httpCode = http.GET();                      // Make the request
      if (httpCode == 200)                            // Check for the returning code
      {
        xml_data = http.getString(); // Get data
        http.end(); // Free the resources
        vTaskDelay(pdMS_TO_TICKS(limit_long));
      }
      else {
        http.end(); // Free the resources
        vTaskDelay(pdMS_TO_TICKS(limit_short));
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
      http.begin(client_whereis, endpoint_spotnik + String("?dtmf=0")); // Specify the URL
      http.addHeader("Content-Type", "text/plain");                     // Specify content-type header
      http.setTimeout(1000);                                            // Set Time Out
      int httpCode = http.GET();                                        // Make the request
      if (httpCode == 200)                                              // Check for the returning code
      {
        whereis_data = http.getString(); // Get data

        whereis_current = whereis_data.substring(0, whereis_data.indexOf(", ")).toInt();
        whereis_data = whereis_data.substring(whereis_data.indexOf(", ") + 2);

        whereis_str = whereis_data.substring(0, whereis_data.indexOf(", "));
        whereis_data = whereis_data.substring(whereis_data.indexOf(", ") + 2);

        if(whereis_data.substring(0, whereis_data.indexOf(", ")) == "OFF") {
          raptor_current = 0;
        } else {
          raptor_current = 1;
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