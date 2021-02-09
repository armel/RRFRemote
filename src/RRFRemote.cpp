#include "RRFRemote.h"

// Setup

void setup()
{
  // Debug
  Serial.begin(9600); // For debug

  // Init screensaver timer
  screensaver = millis();

  // Init M5
  M5.begin(true, false, false, false);
  M5.Power.begin();

  // Preferences
  preferences.begin("RRFRemote");

  room_current = preferences.getUInt("room", 0);
  color_current = preferences.getUInt("color", 0);
  menu_current = preferences.getUInt("menu", 0);
  brightness_current = preferences.getUInt("brightness", 128);

  // LCD
  reset_color();
  M5.Lcd.setBrightness(brightness_current);

  M5.Lcd.fillScreen(M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
  M5.Lcd.setTextDatum(CC_DATUM);

  // Title
  M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
  M5.Lcd.drawString("RRFRemote", 160, 20);

  M5.Lcd.setFreeFont(0);
  M5.Lcd.drawString("Version " + String(VERSION) + " by F4HWN", 160, 50);

  // QRCode
  M5.Lcd.qrcode("https://github.com/armel/RRFRemote", 90, 80, 140, 6);

  // We start by connecting to a WiFi network
  M5.Lcd.drawString(String(ssid), 160, 60);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  M5.Lcd.drawString(String(WiFi.localIP().toString().c_str()), 160, 70);

  // Scroll
  pos = M5.Lcd.width();
  img.createSprite(M5.Lcd.width(), 20);

  // Multitasking task for retreive rrf and propag data
  xTaskCreatePinnedToCore(
      rrftracker,   /* Function to implement the task */
      "rrftracker", /* Name of the task */
      8192,         /* Stack size in words */
      NULL,         /* Task input parameter */
      1,            /* Priority of the task */
      NULL,         /* Task handle. */
      0);           /* Core where the task should run */

  xTaskCreatePinnedToCore(
      hamqsl,   /* Function to implement the task */
      "hamqsl", /* Name of the task */
      8192,     /* Stack size in words */
      NULL,     /* Task input parameter */
      2,        /* Priority of the task */
      NULL,     /* Task handle. */
      1);       /* Core where the task should run */

  delay(4000);

  clear();
}

// Main loop
void loop()
{
  DynamicJsonDocument doc(8192);

  char swap[32];

  const char *emission = "", *date = "", *salon = "", *elsewhere = "", *entrant = "", *sortant = "";
  const char *last_c[10], *last_d[10], *last_h[10];
  const char *all_c[10], *all_d[10];
  const char *iptable_c[10], *iptable_t[10];
  const char *legende[] = {"00", "06", "12", "18", "23"};

  int all_t[10];
  int tot = 0, link_total = 0, link_actif = 0, link_total_elsewhere = 0, tx_total = 0, max_level = 0, tx[24] = {0};
  int optimize = 0;

  uint8_t i, j, k;

  int16_t parenthesis_begin = 0;
  int16_t parenthesis_last = 0;

  unsigned long timer = 0, wait = 0, limit = 750;

  // Let's go
  M5.Lcd.setTextPadding(0);
  timer = millis();

  optimize = json_data_new.compareTo(json_data);

  if (optimize == 0)
  {
    if (DEBUG)
    {
      Serial.print(">> JSON Equal");
    }
  }
  else
  {
    DeserializationError error = deserializeJson(doc, json_data_new); // Deserialize the JSON document

    if (error)
    {
      if (DEBUG)
      {
        Serial.print(">> JSON Error");
      }
    }
    else
    {
      if (DEBUG)
      {
        Serial.print(">> JSON Change");
      }
      json_data = json_data_new;
    }
  }

  deserializeJson(doc, json_data);
  salon = doc["abstract"][0]["Salon"];

  while (strcmp(room[room_current], salon) != 0)
  {
    json_data = json_data_new;
    DeserializationError error = deserializeJson(doc, json_data);
    if (!error)
    {
      salon = doc["abstract"][0]["Salon"];
    }
    delay(100);
  }

  if (reset == 0)
  {
    clear();
  }

  emission = doc["abstract"][0]["Emission cumulée"];
  date = doc["abstract"][0]["Date"];
  link_total = doc["abstract"][0]["Links connectés"];
  link_actif = doc["abstract"][0]["Links actifs"];
  tx_total = doc["abstract"][0]["TX total"];
  entrant = doc["abstract"][0]["Links entrants"];
  sortant = doc["abstract"][0]["Links sortants"];

  tot = doc["transmit"][0]["TOT"];

  if (tot > 0 && transmit_on == 0)
  {
    transmit_on = 1;
  }

  if (tot == 0 && transmit_off == 0)
  {
    transmit_off = 1;
  }

  for (uint8_t i = 0; i < doc["last"].size(); i++)
  {
    last_c[i] = doc["last"][i]["Indicatif"];
    last_h[i] = doc["last"][i]["Heure"];
    last_d[i] = doc["last"][i]["Durée"];
  }

  for (uint8_t i = 0; i < doc["all"].size(); i++)
  {
    all_c[i] = doc["all"][i]["Indicatif"];
    all_t[i] = doc["all"][i]["TX"];
    all_d[i] = doc["all"][i]["Durée"];
  }

  for (uint8_t i = 0; i < doc["iptable"].size(); i++)
  {
    iptable_c[i] = doc["iptable"][i]["Indicatif"];
    iptable_t[i] = doc["iptable"][i]["Type"];
    if (i == 9)
    {
      break;
    }
  }

  max_level = 0;
  for (uint8_t i = 0; i < 24; i++)
  {
    int tmp = doc["activity"][i]["TX"];
    if (tmp > max_level)
    {
      max_level = tmp;
    }
    tx[i] = tmp;
  }

  tmp_str = String(entrant);
  if (tmp_str.length() > 0)
  {
    msg = "+ " + String(entrant);
    tmp_str = String(sortant);
    if (tmp_str.length() > 0)
    {
      msg += " - " + String(sortant);
    }
  }
  else
  {
    tmp_str = String(sortant);
    if (tmp_str.length() > 0)
    {
      msg = "- " + String(sortant);
    }
  }

  // Histogram

  j = 4;
  k = 0;

  for (uint8_t i = 0; i < 24; i++)
  {
    scroll(10);
    if (tx[i] != 0)
    {
      uint8_t tmp = map(tx[i], 0, max_level, 0, 48);
      M5.Lcd.fillRect(j, 153 - tmp, 5, tmp, M5.Lcd.color565(TFT_FRONT.r - k, TFT_FRONT.g - k, TFT_FRONT.b - k));
    }
    M5.Lcd.fillRect(j, 154, 5, 1, TFT_LIGHTGREY);
    j += 6;
    k += 2;
  }

  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.setTextDatum(CL_DATUM);

  for (uint8_t i = 0; i < 5; i++)
  {
    M5.Lcd.drawString(String(legende[i]), 4 + (i * 34), 162);
  }

  // Elsewhere

  scroll(10);

  JsonObject obj = doc.as<JsonObject>();
  elsewhere_str = obj["elsewhere"].as<String>();

  if (elsewhere_str != elsewhere_str_old || refresh == 0)
  {
    elsewhere_str_old = elsewhere_str;

    M5.Lcd.setFreeFont(0);

    i = 0;
    j = 8;
    k = 0;

    while (k <= 4)
    {
      scroll(10);
      if (strcmp(room[i], salon) != 0)
      {
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
        M5.Lcd.drawString(String(room[i]).substring(0, 3), 14, 168 + j);

        elsewhere = doc["elsewhere"][1][room[i]];
        if (strcmp(elsewhere, "Aucune émission") != 0)
        {
          screensaver = millis(); // Screensaver update !!!
          tmp_str = getValue(elsewhere, ' ', 1);
          if (tmp_str == "")
          {
            tmp_str = "RTFM";
          }
          else
          {
            tmp_str = getValue(elsewhere, ' ', 1) + ' ' + getValue(elsewhere, ' ', 2);
          }

          M5.Lcd.fillRect(26, 169 + (14 * k), 70, 13, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
        }
        else
        {
          M5.Lcd.fillRect(26, 169 + (14 * k), 70, 13, TFT_WHITE);
          M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
          M5.Lcd.drawString("         ", 55, 168 + j);
          link_total_elsewhere = doc["elsewhere"][5][room[i]];
          sprintf(swap, "%03d", link_total_elsewhere);
          tmp_str = swap;
          tmp_str += " links";
        }
        M5.Lcd.drawString(tmp_str, 62, 168 + j);

        M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
        elsewhere = doc["elsewhere"][3][room[i]];
        M5.Lcd.drawString(String(elsewhere), 124, 168 + j);

        j += 14;
        k += 1;
      }
      i += 1;
    }
  }

  // Log

  scroll(10);

  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.setTextDatum(CC_DATUM);

  if (type == 1)
  {
    M5.Lcd.drawString("   RRFRemote   ", 240, 110);

    if (refresh == 0)
    {
      i = 161;
      j = 72;
      M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
      M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
      M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.setTextDatum(CL_DATUM);
      M5.Lcd.drawString("CPU", 164, 125);
      M5.Lcd.drawString("CPU Cores", 164, 137);
      M5.Lcd.drawString("CPU Freq", 164, 149);
      M5.Lcd.drawString("Chip Rev", 164, 161);
      M5.Lcd.drawString("Flash Speed", 164, 173);
      M5.Lcd.drawString("Flash Size", 164, 185);
      M5.Lcd.drawString("Free RAM", 164, 197);
      M5.Lcd.drawString("Free Heap", 164, 209);
      M5.Lcd.drawString("IP", 164, 221);
      M5.Lcd.drawString("Version", 164, 233);

      refresh = 1;
    }

    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);

    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString("ESP32", 318, 125);
    M5.Lcd.drawString("2", 318, 137);
    M5.Lcd.drawString(String(ESP.getCpuFreqMHz()) + " Mhz", 318, 149);
    M5.Lcd.drawNumber(ESP.getChipRevision(), 318, 161);
    M5.Lcd.drawString(String(ESP.getFlashChipSpeed() / 1000000) + " Mhz", 318, 173);
    M5.Lcd.drawString(String(ESP.getFlashChipSize() / (1024 * 1024)) + " Mb", 318, 185);
    M5.Lcd.drawString(String(ESP.getFreeHeap() / 1024) + " kb", 318, 197);
    M5.Lcd.drawString(String(esp_get_minimum_free_heap_size() / 1024) + " kb", 318, 209);
    M5.Lcd.drawString((WiFi.localIP().toString()), 318, 221);
    M5.Lcd.drawString(VERSION, 318, 233);
  }
  else if (type == 2)
  {
    M5.Lcd.drawString("  PROPAGATION  ", 240, 110);

    if (refresh == 0)
    {
      i = 161;
      j = 70;
      M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
      M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
      M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.setTextDatum(CL_DATUM);
      M5.Lcd.drawString("SFI", 164, 125);
      M5.Lcd.drawString("Sunspots", 164, 137);
      M5.Lcd.drawString("A-Index", 164, 149);
      M5.Lcd.drawString("K-Index", 164, 161);
      M5.Lcd.drawString("X-Ray", 164, 173);
      M5.Lcd.drawString("Ptn Flux", 164, 185);
      M5.Lcd.drawString("Elc Flux", 164, 197);
      M5.Lcd.drawString("Aurora", 164, 209);
      M5.Lcd.drawString("Solar Wind", 164, 221);
      M5.Lcd.drawString("Update", 164, 233);

      refresh = 1;
    }

    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Lcd.setTextDatum(CR_DATUM);

    tmp_str = xml_data;
    tmp_str.replace("<solarflux>", "(");
    tmp_str.replace("</solarflux>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 125);

    tmp_str = xml_data;
    tmp_str.replace("<sunspots>", "(");
    tmp_str.replace("</sunspots>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 137);

    tmp_str = xml_data;
    tmp_str.replace("<aindex>", "(");
    tmp_str.replace("</aindex>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 149);

    tmp_str = xml_data;
    tmp_str.replace("<kindex>", "(");
    tmp_str.replace("</kindex>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 161);

    tmp_str = xml_data;
    tmp_str.replace("<xray>", "(");
    tmp_str.replace("</xray>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 173);

    tmp_str = xml_data;
    tmp_str.replace("<protonflux>", "(");
    tmp_str.replace("</protonflux>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 185);

    tmp_str = xml_data;
    tmp_str.replace("<electonflux>", "(");
    tmp_str.replace("</electonflux>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 197);

    tmp_str = xml_data;
    tmp_str.replace("<aurora>", "(");
    tmp_str.replace("</aurora>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 209);

    tmp_str = xml_data;
    tmp_str.replace("<solarwind>", "(");
    tmp_str.replace("</solarwind>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str, 318, 221);

    tmp_str = xml_data;
    tmp_str.replace("<updated>", "(");
    tmp_str.replace("</updated>", ")");
    parenthesis_begin = tmp_str.indexOf("(");
    parenthesis_last = tmp_str.indexOf(")");
    if (parenthesis_begin > 0)
    {
      tmp_str = tmp_str.substring(parenthesis_begin + 13, parenthesis_last);
    }
    M5.Lcd.drawString(tmp_str.substring(1, 3) + ":" + tmp_str.substring(3), 318, 233);
  }
  else if (type == 3 && doc["all"].size() != 0)
  {
    M5.Lcd.drawString("   TOP LINKS   ", 240, 110);

    if (refresh == 0)
    {
      i = 161;
      j = 44;
      M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
      M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
      M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

      refresh = 1;
    }

    j = 0;
    k = 125; // 110

    for (uint8_t i = 0; i < doc["all"].size(); i++)
    {
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.setTextDatum(CL_DATUM);
      sprintf(swap, "%03d", all_t[i]);
      tmp_str = swap;
      tmp_str += " TX";
      M5.Lcd.drawString(tmp_str, 164, k + j);
      M5.Lcd.setTextPadding(74);
      M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
      M5.Lcd.setTextDatum(CC_DATUM);
      tmp_str = getValue(all_c[i], ' ', 1);
      if (tmp_str == "")
      {
        tmp_str = "RTFM";
      }
      else
      {
        tmp_str = getValue(all_c[i], ' ', 1) + ' ' + getValue(all_c[i], ' ', 2);
      }
      M5.Lcd.drawString(tmp_str, 240, k + j);
      M5.Lcd.setTextPadding(0);
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.drawString(String(all_d[i]), 318, k + j);

      j += 12;
    }
  }
  else if (type == 4 && doc["iptable"].size() != 0)
  {
    M5.Lcd.drawString("   BLOCAGES    ", 240, 110);

    if (refresh == 0)
    {
      i = 161;
      j = 79;
      M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
      M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
      M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

      refresh = 1;
    }

    j = 0;
    k = 125; // 110

    for (uint8_t i = 0; i < doc["iptable"].size(); i++)
    {
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.setTextDatum(CL_DATUM);
      sprintf(swap, "%s", iptable_t[i]);
      tmp_str = swap;

      parenthesis_begin = tmp_str.indexOf('(');
      parenthesis_last = tmp_str.indexOf(')');

      if (parenthesis_begin > 0)
      {
        tmp_str = tmp_str.substring(parenthesis_begin + 1, parenthesis_last);
      }

      M5.Lcd.drawString(tmp_str, 164, k + j);
      M5.Lcd.setTextPadding(74);
      M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
      M5.Lcd.setTextDatum(CR_DATUM);
      tmp_str = getValue(iptable_c[i], ' ', 1);
      if (tmp_str == "")
      {
        tmp_str = "RTFM";
      }
      else
      {
        tmp_str = getValue(iptable_c[i], ' ', 1) + ' ' + getValue(iptable_c[i], ' ', 2);
      }
      M5.Lcd.drawString(tmp_str, 318, k + j);
      M5.Lcd.setTextPadding(0);

      if (i == 9)
      {
        break;
      }

      j += 12;
    }
  }
  else
  {
    M5.Lcd.drawString("DERNIERS LINKS", 240, 110);

    if (refresh == 0)
    {
      i = 161;
      j = 38;
      M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
      M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
      M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);
      refresh = 1;
    }

    j = 0;
    k = 125; // 110

    for (uint8_t i = 0; i < doc["last"].size(); i++)
    {
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.setTextDatum(CL_DATUM);
      sprintf(swap, "%s", last_h[i]);
      tmp_str = swap;
      tmp_str = tmp_str.substring(0, 5);
      M5.Lcd.drawString(tmp_str, 164, k + j);
      M5.Lcd.setTextPadding(87);
      M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
      M5.Lcd.setTextDatum(CC_DATUM);
      tmp_str = getValue(last_c[i], ' ', 1);
      if (tmp_str == "")
      {
        tmp_str = "RTFM";
      }
      else
      {
        tmp_str = getValue(last_c[i], ' ', 1) + ' ' + getValue(last_c[i], ' ', 2);
      }
      M5.Lcd.drawString(tmp_str, 240, k + j);
      M5.Lcd.setTextPadding(0);
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.drawString(String(last_d[i]), 318, k + j);

      j += 12;
    }
  }

  if (menu_mode == 0)
  {
    // Transmit or no transmit

    if (tot != 0)
    {                         // if transmit
      screensaver = millis(); // Screensaver update !!!
      scroll(10);
      if (DEBUG)
      {
        Serial.println("-----> Transmit Start");
      }

      if (transmit_on == 1 || reset == 0)
      {

        if (DEBUG)
        {
          Serial.println("Transmit > Change");
          Serial.println((String) "Alternance :" + alternance + " / Type :" + type);
        }

        if (!M5.Power.isCharging() && screensaver_off != 1)
        {
          M5.Lcd.setBrightness(brightness_current);
        }

        M5.Lcd.fillRect(0, 41, 320, 38, TFT_BLACK);
        M5.Lcd.fillRect(0, 0, 320, 43, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));

        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
        M5.Lcd.setFreeFont(&dot15pt7b);
        M5.Lcd.setTextDatum(CC_DATUM);
        sprintf(swap, "%s", salon);
        tmp_str = swap;
        M5.Lcd.drawString(tmp_str, 160, 22);

        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
        M5.Lcd.setFreeFont(ICON_FONT);
        M5.Lcd.setTextDatum(CL_DATUM);
        sprintf(swap, "%c", ICON_CALL);
        tmp_str = swap;
        M5.Lcd.drawString(tmp_str, 10, 20);

        M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
        M5.Lcd.setTextDatum(CL_DATUM);
        tmp_str = getValue(last_c[0], ' ', 1);
        if (tmp_str == "")
        {
          tmp_str = "RTFM";
        }
        M5.Lcd.drawString(tmp_str, 10, 60);

        transmit_on = 2;
        transmit_off = 0;
        reset = 1;
      }

      M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.drawString(String(last_d[0]), 310, 60);
    }
    else
    { // if no transmit
      scroll(10);
      if (DEBUG)
      {
        Serial.println("-----> Transmit Stop");
      }

      if (transmit_off == 1 || reset == 0)
      {
        if (DEBUG)
        {
          Serial.println("No Transmit > Change");
          Serial.println((String) "Alternance :" + alternance + " / Type :" + type);
        }

        if (!M5.Power.isCharging() && screensaver_off != 1)
        {
          M5.Lcd.setBrightness(brightness_current);
        }

        M5.Lcd.fillRect(0, 41, 320, 38, TFT_BLACK);
        M5.Lcd.fillRect(0, 0, 320, 43, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));

        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
        M5.Lcd.setFreeFont(&dot15pt7b);
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.drawString(String(salon), 160, 22);

        transmit_on = 0;
        transmit_off = 2;
        reset = 1;
      }

      if (type == 0)
      {
        sprintf(swap, "%s", date);
        tmp_str = swap;
        date_str = getValue(date, ' ', 4);

        if (date_str_old != date_str)
        {
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
          M5.Lcd.setFreeFont(ICON_FONT);
          M5.Lcd.setTextDatum(CL_DATUM);
          sprintf(swap, "%c", ICON_CLOCK);
          tmp_str = swap;
          M5.Lcd.drawString(tmp_str, 10, 18);

          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(date_str, 160, 60);
          date_str_old = date_str;
          link_total_str_old = "";
          link_actif_str_old = "";
          tx_total_str_old = "";
          emission_str_old = "";
        }
      }
      else if (type == 1)
      {
        sprintf(swap, "%d", link_total);
        tmp_str = swap;
        link_total_str = "Links total " + tmp_str;

        if (link_total_str_old != link_total_str)
        {
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
          M5.Lcd.setFreeFont(ICON_FONT);
          M5.Lcd.setTextDatum(CL_DATUM);
          sprintf(swap, "%c", ICON_STAT);
          tmp_str = swap;
          M5.Lcd.drawString(tmp_str, 10, 18);

          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(link_total_str, 160, 60);
          link_total_str_old = link_total_str;
          link_actif_str_old = "";
          date_str_old = "";
          tx_total_str_old = "";
          emission_str_old = "";
        }
      }
      else if (type == 2)
      {
        sprintf(swap, "%d", link_actif);
        tmp_str = swap;
        link_actif_str = "Links actifs " + tmp_str;

        if (link_actif_str_old != link_actif_str)
        {
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
          M5.Lcd.setFreeFont(ICON_FONT);
          M5.Lcd.setTextDatum(CL_DATUM);
          sprintf(swap, "%c", ICON_STAT);
          tmp_str = swap;
          M5.Lcd.drawString(tmp_str, 10, 18);

          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(link_actif_str, 160, 60);
          link_actif_str_old = link_actif_str;
          date_str_old = "";
          tx_total_str_old = "";
          emission_str_old = "";
          link_total_str_old = "";
        }
      }
      else if (type == 3)
      {
        sprintf(swap, "%d", tx_total);
        tmp_str = swap;
        tx_total_str = "TX total " + tmp_str;

        if (tx_total_str_old != tx_total_str)
        {
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
          M5.Lcd.setFreeFont(ICON_FONT);
          M5.Lcd.setTextDatum(CL_DATUM);
          sprintf(swap, "%c", ICON_STAT);
          tmp_str = swap;
          M5.Lcd.drawString(tmp_str, 10, 18);

          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(tx_total_str, 160, 60);
          tx_total_str_old = tx_total_str;
          link_total_str_old = "";
          link_actif_str_old = "";
          date_str_old = "";
          emission_str_old = "";
        }
      }
      else if (type == 4)
      {
        sprintf(swap, "%s", emission);
        tmp_str = swap;
        if (strlen(emission) == 5)
        {
          emission_str = "BF 00:" + tmp_str;
        }
        else
        {
          emission_str = "BF " + tmp_str;
        }

        if (emission_str_old != emission_str)
        {
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
          M5.Lcd.setFreeFont(ICON_FONT);
          M5.Lcd.setTextDatum(CL_DATUM);
          sprintf(swap, "%c", ICON_CLOCK);
          tmp_str = swap;
          M5.Lcd.drawString(tmp_str, 10, 18);

          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(emission_str, 160, 60);
          emission_str_old = emission_str;
          tx_total_str_old = "";
          link_total_str_old = "";
          link_actif_str_old = "";
          date_str_old = "";
        }
      }
    }

    // Baterry

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
    M5.Lcd.setTextDatum(CR_DATUM);

    if (M5.Power.isCharging() && screensaver_off != 1)
    {
      sprintf(swap, "%c", ICON_CHARGING);
      tmp_str = swap;
      M5.Lcd.setFreeFont(&Battery_Icons21pt7b);
      M5.Lcd.drawString(tmp_str, 310, 18);
      M5.Lcd.setBrightness(128);
    }
    else
    {
      if (M5.Power.getBatteryLevel() == 100)
      {
        sprintf(swap, "%c", ICON_BAT100);
      }
      else if (M5.Power.getBatteryLevel() == 75)
      {
        sprintf(swap, "%c", ICON_BAT075);
      }
      else if (M5.Power.getBatteryLevel() == 50)
      {
        sprintf(swap, "%c", ICON_BAT050);
      }
      else if (M5.Power.getBatteryLevel() == 25)
      {
        sprintf(swap, "%c", ICON_BAT025);
      }
      else
      {
        sprintf(swap, "%c", ICON_BAT000);
      }
      tmp_str = swap;
      M5.Lcd.setFreeFont(&Battery_Icons21pt7b);
      M5.Lcd.drawString(tmp_str, 310, 18);
    }
  }
  else
  {
    if (menu_refresh == 0)
    {

      M5.Lcd.setBrightness(brightness_current);
      M5.Lcd.fillRect(0, 41, 320, 38, TFT_BLACK);
      M5.Lcd.fillRect(0, 0, 320, 43, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));

      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
      M5.Lcd.setFreeFont(ICON_FONT);
      M5.Lcd.setTextDatum(CL_DATUM);
      sprintf(swap, "%c", ICON_LEFT);
      tmp_str = swap;
      M5.Lcd.drawString(tmp_str, 10, 20);

      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
      M5.Lcd.setFreeFont(ICON_FONT);
      M5.Lcd.setTextDatum(CR_DATUM);
      sprintf(swap, "%c", ICON_RIGHT);
      tmp_str = swap;
      M5.Lcd.drawString(tmp_str, 310, 20);

      menu_refresh = 1;
    }

    if (menu_selected == -1)
    {
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
      M5.Lcd.setFreeFont(&dot15pt7b);
      M5.Lcd.setTextDatum(CC_DATUM);
      M5.Lcd.setTextPadding(220);
      M5.Lcd.drawString(String("MODE MENU"), 160, 22);
      M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
      M5.Lcd.setTextDatum(CC_DATUM);
      M5.Lcd.setTextPadding(320);
      M5.Lcd.drawString(String(menu[menu_current]), 160, 60);
    }
    else
    {
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
      M5.Lcd.setFreeFont(&dot15pt7b);
      M5.Lcd.setTextDatum(CC_DATUM);
      M5.Lcd.setTextPadding(220);
      M5.Lcd.drawString(String(menu[menu_selected]), 160, 22);
      M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
      M5.Lcd.setTextDatum(CC_DATUM);
      M5.Lcd.setTextPadding(320);

      if (menu_selected == 0)
      {
        if (alternance % 2 == 0)
        {
          M5.Lcd.drawString("QSY en cours", 160, 60);
        }
        else
        {
          M5.Lcd.drawString("            ", 160, 60);
        }
      }
      else if (menu_selected == 1)
      {
        if (alternance % 2 == 0)
        {
          M5.Lcd.drawString("Patientez", 160, 60);
        }
        else
        {
          M5.Lcd.drawString("         ", 160, 60);
        }
      }
      else if (menu_selected == 2)
      {
        if (alternance % 2 == 0)
        {
          M5.Lcd.drawString("Patientez", 160, 60);
        }
        else
        {
          M5.Lcd.drawString("         ", 160, 60);
        }
      }
      else if (menu_selected == 4)
      {
        M5.Lcd.drawString("THEME " + String(color[color_current]), 160, 60);
      }
      else if (menu_selected == 5)
      {
        M5.Lcd.drawString("LEVEL " + String(brightness_current), 160, 60);
      }
    }
  }

  M5.Lcd.drawFastHLine(0, 0, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(0, 43, 320, TFT_WHITE);

  if (alternance % 10 == 0)
  {
    refresh = 0;
    if (tot == 0)
    {
      transmit_off = 1;
    }

    type += 1;

    if (type == 5)
    {
      type = 0;
    }
  }

  alternance += 1;
  if (alternance == 51)
  {
    alternance = 1;
  }

  if (DEBUG)
  {
    Serial.println("--------------------");
    Serial.flush();
  }

  wait = millis() - timer;
  if (wait < limit)
  {
    uint8_t j = int((limit - wait) / 10);
    for (uint8_t i = 0; i <= j; i++)
    {
      scroll(0);
      button();
      delay(8);
    }
  }

  // Manage screensaver

  if (screensaver_off == 0 && millis() - screensaver > screensaver_limit)
  {
    M5.Lcd.sleep();
    screensaver_off = 1;
    M5.Lcd.setBrightness(0);
  }
  else if (screensaver_off == 1 && millis() - screensaver < screensaver_limit)
  {
    M5.Lcd.wakeup();
    screensaver_off = 0;
    M5.Lcd.setBrightness(brightness_current);
  }
}