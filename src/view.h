// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// View histogram
void viewHistogram(uint16_t maxLevel, uint16_t tx[]) {
  const char *legende[] = {"00", "06", "12", "18", "23"};
  uint8_t i, j, k, tmp, delta;

  j = 6;
  k = 0;

  for (i = 0; i < 24; i++)
  {
    if (tx[i] != 0)
    {
      tmp = map(tx[i], 0, maxLevel, 0, 34);
      M5.Lcd.fillRect(j, 139 - tmp, 5, tmp, M5.Lcd.color565(TFT_FRONT.r - k, TFT_FRONT.g - k, TFT_FRONT.b - k));
      M5.Lcd.fillRect(j, 139 - 34, 5, 34 - tmp, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    }
    else
    {
      M5.Lcd.fillRect(j, 139 - 34, 5, 34, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    }
    M5.Lcd.fillRect(j, 140, 5, 1, TFT_LIGHTGREY);
    j += 6;
    k += 2;
  }

  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.setTextDatum(CL_DATUM);
  M5.Lcd.setTextPadding(0);

  for (i = 0; i < 5; i++)
  {
    scroll(10);

    delta = String(legende[i]).toInt();

    delta += utc - 1 - dst;
    delta = (delta < 00) ? 24 + delta : delta;
    delta = (delta > 23) ? delta - 24 : delta;

    if(delta < 10)
    {
      M5.Lcd.drawString("0" + String(delta), 4 + (i * 34), 148);
    }
    else
    {
      M5.Lcd.drawString(String(delta), 4 + (i * 34), 148);
    }
  }
}

// View elsewhere
void viewElsewhere(DynamicJsonDocument doc, const char *salon) {
  static String memory[10];
  const char *elsewhere = "";
  uint16_t linkTotalElsewhere = 0;
  uint8_t i, j, k, l;

  JsonObject obj = doc.as<JsonObject>();
  elsewhereString = obj["elsewhere"].as<String>();

  scroll(10);

  M5.Lcd.setFreeFont(&tahoma6pt7b);

  if(reset == 0) {
    for(i = 0; i < 10; i++) {
      memory[i] = "";
    }
  }

  if(elsewhereString != "null") {
    if (elsewhereString != elsewhereStringOld || refresh == 0)
    {
      elsewhereStringOld = elsewhereString;

      i = 0;
      j = 6;
      k = 0;

      while (k <= 5)
      {
        scroll(10);
        if (strcmp(room[i], salon) != 0)
        {
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.drawString(String(room[i]).substring(0, 3), 14, 154 + j);

          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);

          elsewhere = doc["elsewhere"][1][room[i]];

          //elsewhere = "GW-ALLSTAR-40020";
          //elsewhere = "(67) F1ZRZ T10M";
          //elsewhere = "(67) F1ZRZ S";
          //elsewhere = "(76) TM100SHT H";

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

            l = 0;
          }
          else
          {
            linkTotalElsewhere = doc["elsewhere"][5][room[i]];
            sprintf(swap, "%d", linkTotalElsewhere);
            tmpString = swap;
            tmpString += " LINK";
            if(linkTotalElsewhere > 1)
            {
              tmpString += "S";
            }

            l = 1;
          }

          if(tmpString != memory[i])
          {
            memory[i] = tmpString;

            if(l == 0) {
              M5.Lcd.fillRect(28, 155 + (14 * k), 72, 13, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
              M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
            }
            else {
              M5.Lcd.fillRect(28, 155 + (14 * k), 72, 13, TFT_WHITE);
              M5.Lcd.setTextPadding(70);
            }
            M5.Lcd.drawString(tmpString, 64, 154 + j);
          }

          M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.setTextPadding(52);
          elsewhere = doc["elsewhere"][3][room[i]];
          M5.Lcd.drawString(String(elsewhere), 128, 154 + j);

          M5.Lcd.setTextPadding(0);

          j += 14;
          k += 1;
        }
        i += 1;
      }
    }
  }
}

// View settings
void viewSettings() {
  uint8_t i, j;

  if (refresh == 0)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("RRFRemote " + String(VERSION), 240, 108);

    i = 161;
    j = 74;
    M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
    M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
    M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.setTextPadding(0);

    String system[] = {"CPU", "CPU Cores", "CPU Freq", "Chip Rev", "Flash Speed", "Flash Size", "Free RAM", "Free Heap", "IP", "Battery"};

    j = 122;
    for (uint8_t i = 0; i <= 9; i++)
    {
      M5.Lcd.drawString(system[i], 163, j);
      j += 12;
    }

    refresh = 1;
  }

  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.setTextDatum(CR_DATUM);
  M5.Lcd.setTextPadding(70);

  M5.Lcd.drawString("ESP32", 318, 122 + (12 * 0));
  M5.Lcd.drawString("2", 318, 122 + (12 * 1));
  M5.Lcd.drawString(String(ESP.getCpuFreqMHz()) + " Mhz", 318, 122 + (12 * 2));
  M5.Lcd.drawNumber(ESP.getChipRevision(), 318, 122 + (12 * 3));
  M5.Lcd.drawString(String(ESP.getFlashChipSpeed() / 1000000) + " Mhz", 318, 122 + (12 * 4));
  M5.Lcd.drawString(String(ESP.getFlashChipSize() / (1024 * 1024)) + " Mb", 318, 122 + (12 * 5));
  M5.Lcd.drawString(String(ESP.getFreeHeap() / 1024) + " kb", 318, 122 + (12 * 6));
  M5.Lcd.drawString(String(esp_get_minimum_free_heap_size() / 1024) + " kb", 318, 122 + (12 * 7));
  M5.Lcd.drawString((WiFi.localIP().toString()), 318, 122 + (12 * 8));
  M5.Lcd.drawString(String(getBatteryLevel(1)) + " %", 318, 122 + (12 * 9));
}

// View propagation
void viewPropagation() {
  uint8_t i, j;
  int16_t parenthesisBegin = 0;
  int16_t parenthesisLast = 0;

  if (refresh == 0)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("Propagation", 240, 108);

    i = 161;
    j = 70;
    M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
    M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
    M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.setTextPadding(0);

    String solar[] = {"SFI", "Sunspots", "A-Index", "K-Index", "X-Ray", "Ptn Flux", "Elc Flux", "Aurora", "Solar Wind", "Update"};

    j = 122;
    for (uint8_t i = 0; i <= 9; i++)
    {
      M5.Lcd.drawString(solar[i], 163, j);
      j += 12;
    }

    refresh = 1;
  }

  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.setTextDatum(CR_DATUM);

  String solar[] = {"solarflux", "sunspots", "aindex", "kindex", "xray", "protonflux", "electonflux", "aurora", "solarwind"};

  j = 122;
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
  M5.Lcd.drawString(tmpString.substring(1, 3) + ":" + tmpString.substring(3), 318, 230);
}

// View Top Links
void viewTopLinks(uint8_t stop, uint16_t allTx[10], const char *allIndicatif[10], const char *allDuree[10], const char *salon) {
  uint8_t i, j, k;

  if (refresh == 0)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("Top Links sur " + String(salon).substring(0, 3), 240, 108);

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
  k = 122; // 110

  for (uint8_t i = 0; i < stop; i++)
  {
    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.setTextPadding(0);

    if(allTx[i] > 99)
    {
      allTx[i] = 99;
    }

    sprintf(swap, "%02d", allTx[i]);
    tmpString = swap;
    tmpString += " TX";
    M5.Lcd.drawString(tmpString, 163, k + j);

    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(70);

    tmpString = String(allIndicatif[i]);
    if(tmpString.substring(0, 3) == "GW-") {
      tmpString = "GATEWAY";
    }
    else {
      tmpString = getValue(allIndicatif[i], ' ', 1);
      tmpString = (tmpString == "") ? "RTFM" : getValue(allIndicatif[i], ' ', 1) + ' ' + getValue(allIndicatif[i], ' ', 2);
    }

    M5.Lcd.drawString(tmpString, 234, k + j);

    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.setTextPadding(0);
    M5.Lcd.drawString(String(allDuree[i]), 318, k + j);

    j += 12;
  }
}

// View blocages

void viewBlocage(uint8_t stop, const char *iptableIndicatif[10], const char *iptableType[10], const char *salon) {
  uint8_t i, j, k;
  int16_t parenthesisBegin = 0;
  int16_t parenthesisLast = 0;

  if (refresh == 0)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("Blocages sur " + String(salon).substring(0, 3), 240, 108);

    i = 161;
    j = 50;

    for (uint8_t k = 0; k < stop; k++) {
      tmpString = String(iptableType[k]);
      if(tmpString == "INTEMPESTIF") {
        j = 80;
        break;
      }
    }

    M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
    M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
    M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

    refresh = 1;
  }

  j = 0;
  k = 122; // 110

  for (uint8_t i = 0; i < stop; i++)
  {
    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.setTextPadding(0);

    tmpString = String(iptableType[i]);

    parenthesisBegin = tmpString.indexOf('(');
    parenthesisLast = tmpString.indexOf(')');
    if (parenthesisBegin > 0)
    {
      tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
    }

    M5.Lcd.drawString(tmpString, 163, k + j);

    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.setTextPadding(74);

    tmpString = String(iptableIndicatif[i]);
    if(tmpString.substring(0, 3) == "GW-") {
      tmpString = "GATEWAY";
    }
    else {
      tmpString = getValue(iptableIndicatif[i], ' ', 1);
      tmpString = (tmpString == "") ? "RTFM" : getValue(iptableIndicatif[i], ' ', 1) + ' ' + getValue(iptableIndicatif[i], ' ', 2);
    }

    M5.Lcd.drawString(tmpString, 318, k + j);

    if (i == 9)
    {
      break;
    }

    j += 12;
  }
}

// View ISS
void viewISS(StaticJsonDocument<512> docISS) {
  uint8_t i, j;
  const char *issDataString;
  float issDataNum;

  if (refresh == 0)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("ISS", 240, 108);

    i = 161;
    j = 70;
    M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
    M5.Lcd.fillRoundRect(i, 118, j, 120, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.drawFastVLine(i + j - 3, 118, 120, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.drawFastVLine(i + j - 2, 118, 120, TFT_WHITE);
    M5.Lcd.drawFastVLine(i + j - 1, 118, 120, TFT_WHITE);

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.setTextPadding(0);

    String iss[] = {"Distance", "Units", "Visibility", "Latitude", "Longitude", "Altitude", "Velocity", "Footprint", "Solar Lat", "Solar Long"};

    j = 122;
    for (uint8_t i = 0; i <= 9; i++)
    {
      M5.Lcd.drawString(iss[i], 163, j);
      j += 12;
    }

    refresh = 1;
  }

  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.setTextDatum(CR_DATUM);

  String iss[] = {"distance", "units", "visibility", "latitude", "longitude", "altitude", "velocity",  "footprint", "solar_lat", "solar_lon"};

  DeserializationError error = deserializeJson(docISS, issData); // Deserialize the JSON document
  if (!error) // And no error
  {
    j = 122;
    for (uint8_t i = 0; i <= 9; i++)
    {
      if(i == 0) {
        issDistance = computeDistance((float)(docISS["latitude"]), (float)(docISS["longitude"]));
        tmpString = String(issDistance);
      }
      else if (i > 0 && i < 3) 
      { 
        issDataString = docISS[iss[i]];
        tmpString = String(issDataString);
        tmpString[0] = toupper(tmpString[0]);
      }
      else 
      {
        issDataNum = docISS[iss[i]];
        tmpString = String(issDataNum);
      }
      M5.Lcd.drawString(tmpString, 318, j);
      j += 12;
    }
  }
}

// View last links
void viewLastLinks(uint8_t stop, const char *lastHeure[10], const char *lastIndicatif[10], const char *lastDuree[10], const char *salon) {
  uint8_t i, j, k;

  if (refresh == 0)
  {
    M5.Lcd.setTextPadding(160);
    M5.Lcd.drawString("Derniers TX sur " + String(salon).substring(0, 3), 240, 108);

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
  k = 122; // 110

  for (uint8_t i = 0; i < stop; i++)
  {
    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.setTextPadding(0);

    tmpString = String(lastHeure[i]);
    tmpString = tmpString.substring(0, 5);

    int8_t delta = tmpString.substring(0, 2).toInt();
    
    delta += utc - 1 - dst;
    delta = (delta < 00) ? 24 + delta : delta;
    delta = (delta > 23) ? delta - 24 : delta;

    if(delta < 10)
    {
      tmpString = "0" + String(delta) + tmpString.substring(2, 5);
    }
    else
    {
      tmpString = String(delta) + tmpString.substring(2, 5);
    }

    M5.Lcd.drawString(tmpString, 163, k + j);

    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(80);

    tmpString = String(lastIndicatif[i]);
    if(tmpString.substring(0, 3) == "GW-") {
      tmpString = "GATEWAY";
    }
    else {
      tmpString = getValue(lastIndicatif[i], ' ', 1);
      tmpString = (tmpString == "") ? "RTFM" : getValue(lastIndicatif[i], ' ', 1) + ' ' + getValue(lastIndicatif[i], ' ', 2);
    }

    M5.Lcd.drawString(tmpString, 242, k + j);

    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.setTextPadding(0);
    M5.Lcd.drawString(String(lastDuree[i]), 318, k + j);

    j += 12;
  }
}

// View data
String viewData(uint8_t icon, String data, String dataOld) {
  if (dataOld != data)
  {
    M5.Lcd.fillRect(4, 2, 36, 42, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
    Serial.println(data);

    sprintf(swap, "%c", icon);
    tmpString = swap;
    M5.Lcd.drawString(tmpString, 10, 20);

    M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(320);
    M5.Lcd.drawString(data, 160, 60);

    dateStringOld = "";
    linkTotalStringOld = "";
    linkActifStringOld = "";
    txTotalStringOld = "";
    emissionStringOld = "";
    issStringOld = "";

    return data;
  }
  return dataOld;
}

// view menu
void viewMenu() {
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

    titleStringOld = "";
    optionStringOld = "";

    menuRefresh = 1;
  }
  
  M5.Lcd.setFreeFont(&dot15pt7b);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextPadding(220);

  titleString = "";
  optionString = String(menu[menuCurrent]);

  if (menuSelected == -1)
  {
    titleString = "MODE MENU";
  }
  else
  {
    titleString = String(menu[menuSelected]);

    if(optionString == "SYSOP") 
    {
      optionString = String(sysop[sysopCurrent]);
    }
    else if(optionString == "COULEUR") 
    {
      optionString = String(color[colorCurrent]);
    }
    else if(optionString == "LUMINOSITE") 
    {
      optionString = "LEVEL " + String(brightnessCurrent);      
    }
    else if(optionString == "CONFIG") 
    {
      optionString = String(config[(configCurrent * 6) + 4]);   
    }
    else if(optionString == "RAPTOR") 
    {
      optionString = (raptorCurrent == 0) ? "RAPTOR OFF" : "RAPTOR ON";
    }
    else if(optionString == "ISS") 
    {
      optionString = (issCurrent == 0) ? "ISS OFF" : "ISS ON";
    }
    else if(optionString == "TOT") 
    {
      optionString = (totCurrent == 0) ? "TOT OFF" : "TOT ON";
    }
    else if(optionString == "MODE") 
    {
    optionString = (modeCurrent == 0) ? "MODE EXPERT" : "MODE BASIC";
    }
    else if(optionString == "FOLLOW") 
    {
      optionString = (followCurrent == 0) ? "FOLLOW OFF" : "FOLLOW ON";
    }
  }

  if(titleString != titleStringOld) 
  {
    titleStringOld = titleString;
    M5.Lcd.drawString(titleString, 160, 16);
  }

  if(optionString != optionStringOld)
  {
    optionStringOld = optionString;
    M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(320);
    M5.Lcd.drawString(optionString, 160, 60);
  }
}

// View baseline
void viewBaseline() {
  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextPadding(220);

  baselineString = config[(configCurrent * 6) + 4];

  if ((String)config[(configCurrent * 6) + 5] != "")
  {
    if (dtmf[roomCurrent] != whereisCurrent && followCurrent == 0)
    {
      baselineString += " / " + whereisString;
    }

    if(raptorCurrent == 1) {
      baselineString += " / RAPTOR ON" ;
    }

    if(followCurrent == 1) {
      baselineString += " / FOLLOW ON";
    }
  }

  M5.Lcd.drawString(baselineString, 160, 36);
  M5.Lcd.drawFastHLine(0, 0, 320, TFT_WHITE);
}

// View battery
void viewBattery()
{
  if(reset == 0 || batteryLevelCurrent != getBatteryLevel(0) || batteryChargeCurrent != isCharging())
  {
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
      switch(getBatteryLevel(0))
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

// View elsewhere Big
void viewElsewhereBig(DynamicJsonDocument doc, const char *salon) {
  static String memory[10];
  const char *elsewhere = "";
  uint16_t linkTotalElsewhere = 0;
  uint8_t i, j, k, l;

  JsonObject obj = doc.as<JsonObject>();
  elsewhereString = obj["elsewhere"].as<String>();

  scroll(10);

  M5.Lcd.setFreeFont(&tahoma8pt7b);

  if(reset == 0) {
    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.setTextPadding(0);
    M5.Lcd.drawString("Trafic en cours", 75, 112);

    for(i = 0; i < 10; i++) {
      memory[i] = "";
    }
  }

  if(elsewhereString != "null") {
    if (elsewhereString != elsewhereStringOld || refresh == 0)
    {
      elsewhereStringOld = elsewhereString;

      i = 0;
      j = 6;
      k = 0;

      while (k <= 5)
      {
        scroll(10);
        if (strcmp(room[i], salon) != 0)
        {
          M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
          M5.Lcd.setTextDatum(CC_DATUM);
          M5.Lcd.setTextPadding(45);
          M5.Lcd.drawString(String(room[i]).substring(0, 3), 25, 128 + j);

          elsewhere = doc["elsewhere"][1][room[i]];

          //elsewhere = "GW-ALLSTAR-40020";
          //elsewhere = "(67) F1ZRZ T10M";
          //elsewhere = "(67) F1ZRZ S";
          //elsewhere = "(76) TM100SHT H";

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
            l = 0;
          }
          else
          {
            linkTotalElsewhere = doc["elsewhere"][5][room[i]];
            sprintf(swap, "%d", linkTotalElsewhere);
            tmpString = swap;
            tmpString += " LINK";
            if(linkTotalElsewhere > 1)
            {
              tmpString += "S";
            }
            l = 1;
          }

          if(tmpString != memory[i])
          {
            memory[i] = tmpString;

            if(l == 0) {
              M5.Lcd.fillRect(51, 126 + (18 * k) + k, 98, 18, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
              M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
            }
            else {
              M5.Lcd.fillRect(51, 126 + (18 * k) + k, 98, 18, TFT_WHITE);
              M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
            }

            M5.Lcd.setTextDatum(CC_DATUM);
            M5.Lcd.setTextPadding(96);
            M5.Lcd.drawString(tmpString, 100, 128 + j);
          }

          j += 19;
          k += 1;
        }
        i += 1;
      }
    }
  }
}

// View last links Big
void viewLastLinksBig(uint8_t stop, const char *lastHeure[10], const char *lastIndicatif[10], const char *lastDuree[10], const char *salon) {
  uint8_t i, j, k;
  static String memory[10];

  j = 0;
  k = 134; // 110

  scroll(10);

  M5.Lcd.setFreeFont(&tahoma8pt7b);

  if(reset == 0) {
    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.setTextPadding(0);
    M5.Lcd.drawString("Derniers TX sur " + String(salon).substring(0, 3), 245, 112);

    for(i = 0; i < 10; i++) {
      memory[i] = "";
    }
  }

  for (i = 0; i < 6; i++)
  {
    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(45);

    tmpString = String(lastHeure[i]);
    tmpString = tmpString.substring(0, 5);

    int8_t delta = tmpString.substring(0, 2).toInt();
    
    delta += utc - 1 - dst;
    delta = (delta < 00) ? 24 + delta : delta;
    delta = (delta > 23) ? delta - 24 : delta;

    if(delta < 10)
    {
      tmpString = "0" + String(delta) + tmpString.substring(2, 5);
    }
    else
    {
      tmpString = String(delta) + tmpString.substring(2, 5);
    }

    M5.Lcd.drawString(tmpString, 195, k + j);

    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(96);

    if(String(lastIndicatif[i]) != memory[i])
    {
      memory[i] = String(lastIndicatif[i]);
    
      tmpString = String(lastIndicatif[i]);
      if(tmpString.substring(0, 3) == "GW-") {
        tmpString = "GATEWAY";
      }
      else {
        tmpString = getValue(lastIndicatif[i], ' ', 1);
        tmpString = (tmpString == "") ? "RTFM" : getValue(lastIndicatif[i], ' ', 1) + ' ' + getValue(lastIndicatif[i], ' ', 2);
      }

      M5.Lcd.drawString(tmpString, 270, k + j);
    }

    j += 19;
  }
}