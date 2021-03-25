// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "RRFRemote.h"

// Setup

void setup()
{
  // Debug
  Serial.begin(115200);

  // Init screensaver timer
  screensaver = millis();

  // Init M5
  M5.begin(true, false, false, false);
  power();

  // Init Speaker
  M5.Speaker.setVolume(1);
  M5.Speaker.update();

  // Preferences
  preferences.begin("RRFRemote");

  size_t n = sizeof(config) / sizeof(config[0]);
  n = (n / 4) - 1;

  //preferences.putUInt("config", 0);

  configCurrent = preferences.getUInt("config", 0);

  if(configCurrent > n) {
    configCurrent = 0;
    preferences.putUInt("config", configCurrent);
  }

  roomCurrent = preferences.getUInt("room", 0);
  colorCurrent = preferences.getUInt("color", 0);
  menuCurrent = preferences.getUInt("menu", 0);
  brightnessCurrent = preferences.getUInt("brightness", 128);
  followCurrent = preferences.getUInt("follow", 0);
  totCurrent = preferences.getUInt("tot", 0);

  // LCD
  resetColor();
  M5.Lcd.setBrightness(brightnessCurrent);
  M5.Lcd.fillScreen(M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));

  // Title
  M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.drawString("RRFRemote", 160, 20);
  M5.Lcd.setFreeFont(0);
  M5.Lcd.drawString("Version " + String(VERSION) + " by F4HWN", 160, 50);

  // QRCode
  M5.Lcd.qrcode("https://github.com/armel/RRFRemote", 90, 80, 140, 6);

  // We start by connecting to the WiFi network
  M5.Lcd.drawString(String(config[(configCurrent * 6)]), 160, 60);

  WiFi.begin(config[(configCurrent * 6)], config[(configCurrent * 6) + 1]);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  M5.Lcd.drawString(String(WiFi.localIP().toString().c_str()), 160, 70);

  // Scroll
  pos = M5.Lcd.width();
  img.createSprite(M5.Lcd.width(), 20);

  // Multitasking task for retreive rrf, spotnik and propag data
  xTaskCreatePinnedToCore(
      rrftracker,   /* Function to implement the task */
      "rrftracker", /* Name of the task */
      8192,         /* Stack size in words */
      NULL,         /* Task input parameter */
      1,            /* Priority of the task */
      NULL,         /* Task handle. */
      0);           /* Core where the task should run */

  xTaskCreatePinnedToCore(
      whereis,      /* Function to implement the task */
      "whereis",    /* Name of the task */
      8192,         /* Stack size in words */
      NULL,         /* Task input parameter */
      1,            /* Priority of the task */
      NULL,         /* Task handle. */
      1);           /* Core where the task should run */

  xTaskCreatePinnedToCore(
      hamqsl,       /* Function to implement the task */
      "hamqsl",     /* Name of the task */
      8192,         /* Stack size in words */
      NULL,         /* Task input parameter */
      2,            /* Priority of the task */
      NULL,         /* Task handle. */
      1);           /* Core where the task should run */

  // Accelelerometer
  M5.IMU.Init();

  // Let's go after temporisation
  delay(1000);

  for (uint8_t i = 0; i < 120; i++)
  {
    M5.Lcd.drawFastHLine(0, i, 320, TFT_BLACK);
    M5.Lcd.drawFastHLine(0, 240 - i, 320, TFT_BLACK);
    delay(5);
  }

  clear();
}

// Main loop
void loop()
{
  DynamicJsonDocument doc(8192);

  char swap[32];

  const char *emission = "", *date = "", *salon = "", *elsewhere = "", *entrant = "", *sortant = "";
  const char *lastIndicatif[10], *lastDuree[10], *lastHeure[10], *lastLocator[10], *lastRegion[10];
  const char *allIndicatif[10], *allDuree[10];
  const char *iptableIndicatif[10], *iptableType[10];
  const char *legende[] = {"00", "06", "12", "18", "23"};

  float lastLatitude[10], lastLongitude[10];
  int allTx[10];
  int tot = 0, linkTotal = 0, linkActif = 0, linkTotalElsewhere = 0, txTotal = 0, maxLevel = 0, tx[24] = {0};
  int optimize = 0;

  uint8_t i, j, k;
  uint8_t distance = 0;

  static uint8_t lengthData = 0;
  static uint8_t centerData = 0;

  int16_t parenthesisBegin = 0;
  int16_t parenthesisLast = 0;

  unsigned long timer = 0, wait = 0, limit = 750;

  // Let's go
  M5.Lcd.setTextPadding(0);
  timer = millis();

  // Continue
  optimize = jsonDataNew.compareTo(jsonData);

  if (optimize != 0) // If change
  {
    DeserializationError error = deserializeJson(doc, jsonDataNew); // Deserialize the JSON document

    if (!error) // And no error
    {
      jsonData = jsonDataNew;
    }
  }

  deserializeJson(doc, jsonData);
  salon = doc["abstract"][0]["Salon"];

  while (strcmp(room[roomCurrent], salon) != 0)
  {
    jsonData = jsonDataNew;
    DeserializationError error = deserializeJson(doc, jsonData);
    if (!error)
    {
      salon = doc["abstract"][0]["Salon"];
    }
  }

  if (reset == 0)
  {
    clear();
  }

  emission = doc["abstract"][0]["Emission cumulée"];
  date = doc["abstract"][0]["Date"];
  linkTotal = doc["abstract"][0]["Links connectés"];
  linkActif = doc["abstract"][0]["Links actifs"];
  txTotal = doc["abstract"][0]["TX total"];
  entrant = doc["abstract"][0]["Links entrants"];
  sortant = doc["abstract"][0]["Links sortants"];
  tot = doc["transmit"][0]["TOT"];

  for (uint8_t i = 0; i < doc["last"].size(); i++)
  {
    lastIndicatif[i] = doc["last"][i]["Indicatif"];
    lastHeure[i] = doc["last"][i]["Heure"];
    lastDuree[i] = doc["last"][i]["Durée"];
    lastLatitude[i] = doc["last"][i]["Latitude"];
    lastLongitude[i] = doc["last"][i]["Longitude"];
    lastLocator[i] = doc["last"][i]["Locator"];
    lastRegion[i] = doc["last"][i]["Region"];
  }

  for (uint8_t i = 0; i < doc["all"].size(); i++)
  {
    allIndicatif[i] = doc["all"][i]["Indicatif"];
    allTx[i] = doc["all"][i]["TX"];
    allDuree[i] = doc["all"][i]["Durée"];
  }

  for (uint8_t i = 0; i < doc["iptable"].size(); i++)
  {
    iptableIndicatif[i] = doc["iptable"][i]["Indicatif"];
    iptableType[i] = doc["iptable"][i]["Type"];
    if (i == 9)
    {
      break;
    }
  }

  maxLevel = 0;
  for (uint8_t i = 0; i < 24; i++)
  {
    int tmp = doc["activity"][i]["TX"];
    if (tmp > maxLevel)
    {
      maxLevel = tmp;
    }
    tx[i] = tmp;
  }

  // Prepare message
  if(tot == 0 || lastLatitude[0] == 0)
  {
    tmpString = String(entrant);
    if (tmpString.length() > 0)
    {
      message = "+ " + tmpString.substring(0, tmpString.indexOf(","));
      tmpString = String(sortant);
      if (tmpString.length() > 0)
      {
        message += " - " + tmpString.substring(0, tmpString.indexOf(","));
      }
    }
    else
    {
      tmpString = String(sortant);
      if (tmpString.length() > 0)
      {
        message = "- " + tmpString.substring(0, tmpString.indexOf(","));
      }
    }
  }
  else 
  {
    distance = computeDistance(lastLatitude[0], lastLongitude[0]);
    message = String(lastRegion[0]) + " - "  + String(lastLocator[0]) + " - " + String(distance) + " Km";
  }

  // Transmit or not transmit

  if ((tot > 0 && transmitOn == 0) || (tot > 0 && String(lastIndicatif[0]) != indicatifString))
  {
    transmitOn = 1;
  }

  if (tot == 0 && transmitOff == 0)
  {
    transmitOff = 1;
  }

  // Histogram
  scroll(10);

  j = 4;
  k = 0;

  for (uint8_t i = 0; i < 24; i++)
  {
    if (tx[i] != 0)
    {
      uint8_t tmp = map(tx[i], 0, maxLevel, 0, 48);
      M5.Lcd.fillRect(j, 153 - tmp, 5, tmp, M5.Lcd.color565(TFT_FRONT.r - k, TFT_FRONT.g - k, TFT_FRONT.b - k));
      M5.Lcd.fillRect(j, 153 - 48, 5, 48 - tmp, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    }
    else
    {
      M5.Lcd.fillRect(j, 153 - 48, 5, 48, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    }
    M5.Lcd.fillRect(j, 154, 5, 1, TFT_LIGHTGREY);
    j += 6;
    k += 2;
  }

  scroll(10);

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
  elsewhereString = obj["elsewhere"].as<String>();

  if(elsewhereString != "null") {
    if (elsewhereString != elsewhereStringOld || refresh == 0)
    {
      elsewhereStringOld = elsewhereString;

      M5.Lcd.setFreeFont(0);

      i = 0;
      j = 8;
      k = 0;

      while (k <= 4)
      {
        if (strcmp(room[i], salon) != 0)
        {
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(String(room[i]).substring(0, 3), 14, 168 + j);

          elsewhere = doc["elsewhere"][1][room[i]];
          if (strcmp(elsewhere, "Aucune émission") != 0)
          {
            screensaver = millis(); // Screensaver update !!!
            tmpString = String(elsewhere);
            if(tmpString.substring(0, 3) == "GW-") {
              tmpString = "GATEWAY";
            }
            else {
              tmpString = getValue(elsewhere, ' ', 1);
              tmpString = (tmpString == "") ? "RTFM" : getValue(elsewhere, ' ', 1) + ' ' + getValue(elsewhere, ' ', 2);
            }

            M5.Lcd.fillRect(26, 169 + (14 * k), 70, 13, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
            M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
          }
          else
          {
            M5.Lcd.fillRect(26, 169 + (14 * k), 70, 13, TFT_WHITE);
            M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
            M5.Lcd.drawString("         ", 55, 168 + j);
            linkTotalElsewhere = doc["elsewhere"][5][room[i]];
            sprintf(swap, "%03d", linkTotalElsewhere);
            tmpString = swap;
            tmpString += " links";
          }
          M5.Lcd.drawString(tmpString, 62, 168 + j);

          M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
          M5.Lcd.setTextPadding(50);
          elsewhere = doc["elsewhere"][3][room[i]];
          M5.Lcd.drawString(String(elsewhere), 124, 168 + j);

          M5.Lcd.setTextPadding(0);

          j += 14;
          k += 1;
        }
        i += 1;
      }
    }
  }

  // Log
  scroll(10);

  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.setTextDatum(CC_DATUM);

  if (tot > 0) 
  {
    if (type != 0) {
      refresh = 0;
    }

    alternance = 5;
    type = 0;
  }

  if (type == 1)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("RRFRemote " + String(VERSION), 240, 110);
    M5.Lcd.setTextPadding(0);

    if (refresh == 0)
    {
      i = 161;
      j = 74;
      M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
      M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
      M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.setTextDatum(CL_DATUM);

      String system[] = {"CPU", "CPU Cores", "CPU Freq", "Chip Rev", "Flash Speed", "Flash Size", "Free RAM", "Free Heap", "IP", "Battery"};

      j = 125;
      for (uint8_t i = 0; i <= 9; i++)
      {
        M5.Lcd.drawString(system[i], 164, j);
        j += 12;
      }

      refresh = 1;
    }

    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Lcd.setTextPadding(70);
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
    M5.Lcd.drawString(String(getBatteryLevel(1)) + " %", 318, 233);
  }
  else if (type == 2)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("PROPAGATION", 240, 110);
    M5.Lcd.setTextPadding(0);

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

      String solar[] = {"SFI", "Sunspots", "A-Index", "K-Index", "X-Ray", "Ptn Flux", "Elc Flux", "Aurora", "Solar Wind", "Update"};

      j = 125;
      for (uint8_t i = 0; i <= 9; i++)
      {
        M5.Lcd.drawString(solar[i], 164, j);
        j += 12;
      }

      refresh = 1;
    }

    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Lcd.setTextDatum(CR_DATUM);

    String solar[] = {"solarflux", "sunspots", "aindex", "kindex", "xray", "protonflux", "electonflux", "aurora", "solarwind"};
    //size_t n = sizeof(solar) / sizeof(solar[0]);

    j = 125;
    for (uint8_t i = 0; i <= 8; i++)
    {
      tmpString = xmlData;
      tmpString.replace("<" + solar[i] + ">", "(");
      tmpString.replace("</" + solar[i] + ">", ")");
      parenthesisBegin = tmpString.indexOf("(");
      parenthesisLast = tmpString.indexOf(")");
      if (parenthesisBegin > 0)
      {
        tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
      }
      M5.Lcd.drawString(tmpString, 318, j);
      j += 12;
    }

    tmpString = xmlData;
    tmpString.replace("<updated>", "(");
    tmpString.replace("</updated>", ")");
    parenthesisBegin = tmpString.indexOf("(");
    parenthesisLast = tmpString.indexOf(")");
    if (parenthesisBegin > 0)
    {
      tmpString = tmpString.substring(parenthesisBegin + 13, parenthesisLast);
    }
    M5.Lcd.drawString(tmpString.substring(1, 3) + ":" + tmpString.substring(3), 318, 233);
  }
  else if (type == 3 && doc["all"].size() != 0)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("TOP LINKS", 240, 110);
    M5.Lcd.setTextPadding(0);

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
      sprintf(swap, "%03d", allTx[i]);
      tmpString = swap;
      tmpString += " TX";
      M5.Lcd.drawString(tmpString, 164, k + j);
      M5.Lcd.setTextPadding(74);
      M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
      M5.Lcd.setTextDatum(CC_DATUM);

      tmpString = String(allIndicatif[i]);
      if(tmpString.substring(0, 3) == "GW-") {
        tmpString = "GATEWAY";
      }
      else {
        tmpString = getValue(allIndicatif[i], ' ', 1);
        tmpString = (tmpString == "") ? "RTFM" : getValue(allIndicatif[i], ' ', 1) + ' ' + getValue(allIndicatif[i], ' ', 2);
      }
 
      M5.Lcd.drawString(tmpString, 240, k + j);
      M5.Lcd.setTextPadding(0);
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.drawString(String(allDuree[i]), 318, k + j);

      j += 12;
    }
  }
  else if (type == 4 && doc["iptable"].size() != 0)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("BLOCAGES", 240, 110);
    M5.Lcd.setTextPadding(0);

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
      tmpString = String(iptableType[i]);

      parenthesisBegin = tmpString.indexOf('(');
      parenthesisLast = tmpString.indexOf(')');

      if (parenthesisBegin > 0)
      {
        tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
      }

      M5.Lcd.drawString(tmpString, 164, k + j);
      M5.Lcd.setTextPadding(74);
      M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
      M5.Lcd.setTextDatum(CR_DATUM);

      tmpString = String(iptableIndicatif[i]);
      if(tmpString.substring(0, 3) == "GW-") {
        tmpString = "GATEWAY";
      }
      else {
        tmpString = getValue(iptableIndicatif[i], ' ', 1);
        tmpString = (tmpString == "") ? "RTFM" : getValue(iptableIndicatif[i], ' ', 1) + ' ' + getValue(iptableIndicatif[i], ' ', 2);
      }

      M5.Lcd.drawString(tmpString, 318, k + j);
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
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("DERNIERS LINKS", 240, 110);
    M5.Lcd.setTextPadding(0);

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
      tmpString = String(lastHeure[i]);
      tmpString = tmpString.substring(0, 5);
      M5.Lcd.drawString(tmpString, 164, k + j);
      M5.Lcd.setTextPadding(87);
      M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
      M5.Lcd.setTextDatum(CC_DATUM);

      tmpString = String(lastIndicatif[i]);
      if(tmpString.substring(0, 3) == "GW-") {
        tmpString = "GATEWAY";
      }
      else {
        tmpString = getValue(lastIndicatif[i], ' ', 1);
        tmpString = (tmpString == "") ? "RTFM" : getValue(lastIndicatif[i], ' ', 1) + ' ' + getValue(lastIndicatif[i], ' ', 2);
      }

      M5.Lcd.drawString(tmpString, 240, k + j);
      M5.Lcd.setTextPadding(0);
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.drawString(String(lastDuree[i]), 318, k + j);

      j += 12;
    }
  }

  // Transmit or no transmit

  scroll(10);

  if (menuMode == 0)
  {
    menuSelected = -1;
    if (tot != 0)
    {                         // If transmit
      screensaver = millis(); // Screensaver update !!!
      if (transmitOn == 1 || reset == 0)
      {
        if (!isCharging() && screensaverMode == 0)
        {
          M5.Lcd.setBrightness(brightnessCurrent);
        }

        M5.Lcd.fillRect(4, 2, 36, 42, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));

        M5.Lcd.setFreeFont(&dot15pt7b);
        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.setTextPadding(240);
        M5.Lcd.drawString(String(salon), 160, 16);

        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
        M5.Lcd.setFreeFont(ICON_FONT);
        M5.Lcd.setTextDatum(CL_DATUM);
        M5.Lcd.setTextPadding(20);
        sprintf(swap, "%c", ICON_CALL);
        tmpString = swap;
        M5.Lcd.drawString(tmpString, 10, 20);

        M5.Lcd.fillRect(0, 46, 320, 32, TFT_BLACK);

        M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
        M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Lcd.setTextDatum(CL_DATUM);

        tmpString = String(lastIndicatif[0]);
        indicatifString = tmpString;

        if(tmpString.substring(0, 3) == "GW-") {
          tmpString = "GW";
        }
        else {
          departmentString = getValue(lastIndicatif[0], ' ', 0);
          tmpString = getValue(lastIndicatif[0], ' ', 1);
          tmpString = (tmpString == "") ? "RTFM" : tmpString;
        }

        tmpString += " 0:00";
        lengthData = M5.Lcd.textWidth(tmpString);
        centerData = (320 - lengthData) / 2;

        M5.Lcd.drawString(tmpString, centerData, 60);

        transmitOn = 2;
        transmitOff = 0;
      }

      M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
      M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Lcd.setTextDatum(CL_DATUM);
      dureeString = String(lastDuree[0]);
      if (dureeStringOld != dureeString) 
      {
        dureeStringOld = dureeString;
        M5.Lcd.drawString(dureeString.substring(1), (centerData + lengthData - 75), 60);
      }

      if(totCurrent) {
        if((String(salon) == "RRF" && tot > 135) || (tot > 285))
        {
          M5.Speaker.tone(2000);
          scroll(10);
          M5.Speaker.tone(2000);
          scroll(10);
          M5.Speaker.mute();
        }
      }
    }
    else
    { // If no transmit
      if (transmitOff == 1 || reset == 0)
      {
        if (!isCharging() && screensaverMode == 0)
        {
          M5.Lcd.setBrightness(brightnessCurrent);
        }

        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
        M5.Lcd.setFreeFont(&dot15pt7b);
        M5.Lcd.setTextDatum(CC_DATUM);
        M5.Lcd.drawString(String(salon), 160, 16);

        dateStringOld = "";
        linkTotalStringOld = "";
        linkActifStringOld = "";
        txTotalStringOld = "";
        emissionStringOld = "";

        transmitOn = 0;
        transmitOff = 2;
      }

      M5.Lcd.setFreeFont(ICON_FONT);
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
      M5.Lcd.setTextDatum(CL_DATUM);
      M5.Lcd.setTextPadding(0);

      if (type == 0)
      {
        dateString = getValue(date, ' ', 4);

        if (dateStringOld != dateString)
        {
          M5.Lcd.fillRect(4, 2, 36, 42, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
          sprintf(swap, "%c", ICON_CLOCK);
          tmpString = swap;
          M5.Lcd.drawString(tmpString, 10, 18);

          M5.Lcd.setTextPadding(320);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(dateString, 160, 60);
          dateStringOld = dateString;
          linkTotalStringOld = "";
          linkActifStringOld = "";
          txTotalStringOld = "";
          emissionStringOld = "";
        }
      }
      else if (type == 1)
      {
        sprintf(swap, "%d", linkTotal);
        tmpString = swap;
        linkTotalString = "Links total " + tmpString;

        if (linkTotalStringOld != linkTotalString)
        {
          sprintf(swap, "%c", ICON_STAT);
          tmpString = swap;
          M5.Lcd.drawString(tmpString, 10, 20);

          M5.Lcd.setTextPadding(320);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(linkTotalString, 160, 60);
          linkTotalStringOld = linkTotalString;
          linkActifStringOld = "";
          dateStringOld = "";
          txTotalStringOld = "";
          emissionStringOld = "";
        }
      }
      else if (type == 2)
      {
        sprintf(swap, "%d", linkActif);
        tmpString = swap;
        linkActifString = "Links actifs " + tmpString;

        if (linkActifStringOld != linkActifString)
        {
          sprintf(swap, "%c", ICON_STAT);
          tmpString = swap;
          M5.Lcd.drawString(tmpString, 10, 20);

          M5.Lcd.setTextPadding(320);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(linkActifString, 160, 60);
          linkActifStringOld = linkActifString;
          dateStringOld = "";
          txTotalStringOld = "";
          emissionStringOld = "";
          linkTotalStringOld = "";
        }
      }
      else if (type == 3)
      {
        sprintf(swap, "%d", txTotal);
        tmpString = swap;
        txTotalString = "TX total " + tmpString;

        if (txTotalStringOld != txTotalString)
        {
          sprintf(swap, "%c", ICON_STAT);
          tmpString = swap;
          M5.Lcd.drawString(tmpString, 10, 20);

          M5.Lcd.setTextPadding(320);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(txTotalString, 160, 60);
          txTotalStringOld = txTotalString;
          linkTotalStringOld = "";
          linkActifStringOld = "";
          dateStringOld = "";
          emissionStringOld = "";
        }
      }
      else if (type == 4)
      {
        tmpString = String(emission);
        emissionString = (strlen(emission) == 5) ? "BF 00:" + tmpString : "BF " + tmpString;

        if (emissionStringOld != emissionString)
        {
          M5.Lcd.fillRect(4, 2, 36, 42, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
          sprintf(swap, "%c", ICON_CLOCK);
          tmpString = swap;
          M5.Lcd.drawString(tmpString, 10, 18);

          M5.Lcd.setTextPadding(320);
          M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(emissionString, 160, 60);
          emissionStringOld = emissionString;
          txTotalStringOld = "";
          linkTotalStringOld = "";
          linkActifStringOld = "";
          dateStringOld = "";
        }
      }
    }

    // Baterry

    if(reset == 0 || batteryLevelCurrent != getBatteryLevel(0) || batteryChargeCurrent != isCharging())
    {
      scroll(10);

      reset = (reset == 0) ? 1 : 1;
      batteryLevelCurrent = getBatteryLevel(0);
      batteryChargeCurrent = isCharging();

      M5.Lcd.setFreeFont(&Battery_Icons21pt7b);
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.setTextPadding(0);

      if (isCharging() && screensaverMode == 0)
      {
        sprintf(swap, "%c", ICON_CHARGING);
        tmpString = swap;
        M5.Lcd.drawString(tmpString, 310, 18);
        M5.Lcd.setBrightness(128);
      }
      else
      {
        i = getBatteryLevel(0);
        switch(i)
        {
          case 100: sprintf(swap, "%c", ICON_BAT100); break;
          case  75: sprintf(swap, "%c", ICON_BAT075); break;
          case  50: sprintf(swap, "%c", ICON_BAT050); break;
          case  25: sprintf(swap, "%c", ICON_BAT025); break;
          default:  sprintf(swap, "%c", ICON_BAT000); break;
        }
        tmpString = swap;
        M5.Lcd.drawString(tmpString, 310, 18);
      }
    }
  }
  else
  {
    if (menuRefresh == 0)
    {
      M5.Lcd.setBrightness(brightnessCurrent);

      M5.Lcd.fillRect(4, 4, 316, 40, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));

      M5.Lcd.setFreeFont(ICON_FONT);
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
      M5.Lcd.setTextDatum(CL_DATUM);
      M5.Lcd.setTextPadding(40);
      sprintf(swap, "%c", ICON_LEFT);
      tmpString = swap;
      M5.Lcd.drawString(tmpString, 10, 18);

      M5.Lcd.setFreeFont(ICON_FONT);
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.setTextPadding(40);
      sprintf(swap, "%c", ICON_RIGHT);
      tmpString = swap;
      M5.Lcd.drawString(tmpString, 310, 18);

      menuRefresh = 1;
    }

    reset = (reset == 0) ? 1 : 1;
    
    M5.Lcd.setFreeFont(&dot15pt7b);
    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(220);

    String title = "";
    String option = String(menu[menuCurrent]);

    if (menuSelected == -1)
    {
      title = "MODE MENU";
    }
    else
    {
      title = String(menu[menuSelected]);

      if(option == "COULEUR") 
      {
        option = String(color[colorCurrent]);
      }
      else if(option == "LUMINOSITE") 
      {
        option = "LEVEL " + String(brightnessCurrent);      
      }
      else if(option == "CONFIG") 
      {
        option = String(config[(configCurrent * 6) + 4]);   
      }
      else if(option == "TOT") 
      {
        if (totCurrent == 0)
        {
          option = "TOT ON";
        } 
        else 
        {
          option = "TOT OFF";
        }   
      }
    }

    M5.Lcd.drawString(title, 160, 16);

    M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(320);
    M5.Lcd.drawString(option, 160, 60);
  }

  // Status line
  scroll(10);
  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextPadding(220);

  tmpString = config[(configCurrent * 6) + 4];

  if (dtmf[roomCurrent] != whereisCurrent && followCurrent == 0)
  {
    tmpString += " / " + whereisString;
  }

  if(raptorCurrent == 1) {
    tmpString += " / RAPTOR ON" ;
  }

  if(followCurrent == 1) {
    tmpString += " / FOLLOW ON";
  }

  M5.Lcd.drawString(tmpString, 160, 36);
  M5.Lcd.drawFastHLine(0, 0, 320, TFT_WHITE);

  // Manage follow
  if(followCurrent == 1) {
    for (uint8_t i = 0; i <= 5; i++) {
      if (dtmf[i] == whereisCurrent) {
        if (i != roomCurrent) {
          roomCurrent = i;
          preferences.putUInt("room", roomCurrent);
          reset = 0;
          refresh = 0;
        }
        break;
      }
    }
  }

  // Alternance and type
  scroll(10);

  alternance++;
  if(alternance == 10) {
    refresh = 0;
    type = (type++ < 4) ? type : 0;
    alternance = 0;
  }

  // Temporisation
  wait = millis() - timer;
  if (wait < limit)
  {
    uint8_t j = int((limit - wait) / 10);
    for (uint8_t i = 0; i <= j; i++)
    {
      scroll(10);
    }
  }

  // Manage button
  button();

  // Manage screensaver
  scroll(10);

  if (screensaverMode == 0 && millis() - screensaver > screensaverLimit)
  {
    M5.Lcd.sleep();
    screensaverMode = 1;
    M5.Lcd.setBrightness(0);
  }
  else if (screensaverMode == 1 && millis() - screensaver < screensaverLimit)
  {
    M5.Lcd.wakeup();
    screensaverMode = 0;
    M5.Lcd.setBrightness(brightnessCurrent);
  }

  // Manage rotation
  getAcceleration();
}