#include "RRFRemote.h"

void loop() {
  DynamicJsonDocument doc(8192);

  char swap[32];

  const char *emission = "", *date = "", *salon = "", *elsewhere = "", *entrant = "", *sortant = "";
  const char *last_c[10], *last_d[10], *last_h[10];
  const char *all_c[10], *all_d[10];
  const char *iptable_c[10], *iptable_t[10];
  const char *legende[] = {"00", "06", "12", "18", "23"};
  const char *room[] = {"RRF", "TECHNIQUE", "BAVARDAGE", "LOCAL", "INTERNATIONAL", "FON"};

  int all_t[10];
  int tot = 0, link_total = 0, link_total_elsewhere = 0, tx_total = 0, max_level = 0, tx[24] = {0};
  int optimize = 0;

  uint8_t i, j, k;

  timer = millis();

  Serial.begin(9600); // For debug
  
  optimize = json_data_new.compareTo(json_data);

  if (optimize == 0) {
    if(DEBUG) {
      Serial.print(">> JSON Equal");
    }
  }
  else {
    DeserializationError error = deserializeJson(doc, json_data_new); // Deserialize the JSON document

    if(error) {
      if(DEBUG) {
        Serial.print(">> JSON Error");
      }
    }
    else {
      if(DEBUG) {
        Serial.print(">> JSON Change");
      }
      json_data = json_data_new;
    }
  }

  deserializeJson(doc, json_data);
  salon = doc["abstract"][0]["Salon"];

  while(strcmp(room[room_current], salon) != 0) {
    json_data = json_data_new;
    DeserializationError error = deserializeJson(doc, json_data);
    if(!error) {
      salon = doc["abstract"][0]["Salon"];
    }
    delay(100);
  }

  if(reset == 0) {
    clear();
  }

  emission = doc["abstract"][0]["Emission cumulée"];
  date = doc["abstract"][0]["Date"];
  link_total = doc["abstract"][0]["Links connectés"];
  tx_total = doc["abstract"][0]["TX total"];
  entrant = doc["abstract"][0]["Links entrants"];
  sortant = doc["abstract"][0]["Links sortants"];

  tot = doc["transmit"][0]["TOT"];

  if (tot > 0 && transmit_on == 0) {
    transmit_on = 1;
  } 
  
  if (tot == 0 && transmit_off == 0) {
    transmit_off = 1;
  }

  for (uint8_t i = 0; i < doc["last"].size(); i++) {
    last_c[i] = doc["last"][i]["Indicatif"];
    last_h[i] = doc["last"][i]["Heure"];
    last_d[i] = doc["last"][i]["Durée"];
  }

  for (uint8_t i = 0; i < doc["all"].size(); i++) {
    all_c[i] = doc["all"][i]["Indicatif"];
    all_t[i] = doc["all"][i]["TX"];
    all_d[i] = doc["all"][i]["Durée"];
  }

  for (uint8_t i = 0; i < doc["iptable"].size(); i++) {
    iptable_c[i] = doc["iptable"][i]["Indicatif"];
    iptable_t[i] = doc["iptable"][i]["Type"];
    if (i == 9) {
      break;
    }
  }

  max_level = 0;
  for (uint8_t i = 0; i < 24; i++) {
    int tmp = doc["activity"][i]["TX"];
    if (tmp > max_level) {
      max_level = tmp;
    }
    tx[i] = tmp;
  }

  if(qsy == 1) {
    msg = "QSY en cours";
  }
  else {
    tmp_str = String(entrant);
    if(tmp_str.length() > 0) {
      msg = "+ " + String(entrant);
      tmp_str = String(sortant);
      if(tmp_str.length() > 0) {
        msg += " - " + String(sortant);
      }
    }
    else {
      tmp_str = String(sortant);
      if(tmp_str.length() > 0) {
        msg = "- " + String(sortant);
      }
    }
  }
  
  // Histogram

  /*
  if(reset == 0) {
    M5.Lcd.fillRect(0, 101, 160, 140, M5.Lcd.color565(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B));
  }
  */

  j = 4;
  k = 0;

  for (uint8_t i = 0; i < 24; i++) {
    scroll();
    delay(10);
    if (tx[i] != 0) {
      uint8_t tmp = map(tx[i], 0, max_level, 0, 48);
      //M5.Lcd.fillRect(j, 153 - 48, 5, tmp, M5.Lcd.color565(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B));
      M5.Lcd.fillRect(j, 153 - tmp, 5, tmp, M5.Lcd.color565(COLOR_FRONT_R - k, COLOR_FRONT_G - k, COLOR_FRONT_B - k));
    }
    M5.Lcd.fillRect(j, 154, 5, 1, TFT_LIGHTGREY);
    j += 6;
    k += 2;
  }

  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B));
  M5.Lcd.setTextDatum(CL_DATUM);

  for (uint8_t i = 0; i < 5; i++) {
    M5.Lcd.drawString(legende[i], 4 + (i * 34), 162);
  }

  // Elswhere

  M5.Lcd.setFreeFont(0);

  i = 0;
  j = 8;
  k = 0;

  while( k <= 4) {
    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));
    
    if (strcmp(room[i], salon) != 0) {
      M5.Lcd.setTextDatum(CC_DATUM);

      tmp_str = String(room[i]);
      M5.Lcd.drawString(tmp_str.substring(0, 3), 14, 169 + j);

      elsewhere = doc["elsewhere"][3][room[i]];
      M5.Lcd.drawString(elsewhere, 53, 169 + j);

      elsewhere = doc["elsewhere"][1][room[i]];
      if (strcmp(elsewhere, "Aucune émission") != 0) {

        tmp_str = getValue(elsewhere, ' ', 1);
        if (tmp_str == "") {
          tmp_str = "RTFM";
        } else {
          tmp_str = getValue(elsewhere, ' ', 1) + ' ' + getValue(elsewhere, ' ', 2);
        }

        M5.Lcd.fillRoundRect(81, 170 + (14 * k), 68, 12, 4, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
      } else {
        M5.Lcd.fillRoundRect(81, 170 + (14 * k), 68, 12, 4, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));
        M5.Lcd.drawString("         ", 116, 169 + j);
        link_total_elsewhere = doc["elsewhere"][5][room[i]];
        sprintf(swap, "%03d", link_total_elsewhere);
        tmp_str = swap;
        tmp_str += " links";
      }
      M5.Lcd.drawString(tmp_str, 116, 169 + j);

      j += 14;
      k += 1;
    }
    i += 1;
  }

  // Log

  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B));
  M5.Lcd.setTextDatum(CC_DATUM);

  if (type == 1) {
    M5.Lcd.drawString("   RRFRemote   ", 240, 110);

    if(refresh == 0) {
      M5.Lcd.fillRoundRect(161, 118, 158, 120, 4, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));
      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));
      refresh = 1;
    }

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("CPU", 164, 125);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString("ESP32", 315, 125);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("CPU Cores", 164, 137);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString("2", 315, 137);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("CPU Frequency", 164, 149);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString(String(ESP.getCpuFreqMHz()) + " Mhz", 315, 149);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("Chip Revision", 164, 161);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawNumber(ESP.getChipRevision(), 315, 161);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("Flash Speed", 164, 173);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString(String(ESP.getFlashChipSpeed() / 1000000) + " Mhz", 315, 173);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("Flash Size", 164, 185);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString(String(ESP.getFlashChipSize() / (1024 * 1024)) + " Mb", 315, 185);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("Free RAM", 164, 197);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString(String(ESP.getFreeHeap() / 1024) + " kb", 315, 197);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("Min Free Heap", 164, 209);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString(String(esp_get_minimum_free_heap_size() / 1024) + " kb", 315, 209);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("IP", 164, 221);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString((WiFi.localIP().toString()), 315, 221);

    M5.Lcd.setTextDatum(CL_DATUM);
    M5.Lcd.drawString("Version", 164, 233);
    M5.Lcd.setTextDatum(CR_DATUM);
    M5.Lcd.drawString("0.0.1", 315, 233);

  } else if (type == 2 && doc["all"].size() != 0) {
    M5.Lcd.drawString("   TOP LINKS   ", 240, 110);

    if(refresh == 0 && doc["all"].size() < 10) {
      M5.Lcd.fillRoundRect(161, 118 + (doc["all"].size()) * 12, 158, 120 - (doc["all"].size()) * 12, 4, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));
      refresh = 1;
    }

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));

    j = 0;
    k = 125; // 110

    for (uint8_t i = 0; i < doc["all"].size(); i++) {
      M5.Lcd.setTextDatum(CL_DATUM);
      sprintf(swap, "%03d", all_t[i]);
      tmp_str = swap;
      tmp_str += " TX               ";
      M5.Lcd.drawString(tmp_str, 164, k + j);
      M5.Lcd.setTextDatum(CC_DATUM);
      tmp_str = getValue(all_c[i], ' ', 1);
      if (tmp_str == "") {
        tmp_str = "RTFM";
      } else {
        tmp_str = getValue(all_c[i], ' ', 1) + ' ' + getValue(all_c[i], ' ', 2);
      }
      M5.Lcd.drawString(tmp_str, 240, k + j);
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.drawString(all_d[i], 314, k + j);

      j += 12;
    }
  } else if (type == 3 && doc["iptable"].size() != 0) {
    M5.Lcd.drawString("   BLOCAGES    ", 240, 110);

    if(refresh == 0 && doc["iptable"].size() < 10) {
      M5.Lcd.fillRoundRect(161, 118 + (doc["iptable"].size()) * 12, 158, 120 - (doc["iptable"].size()) * 12, 4, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));
      refresh = 1;
    }

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));

    j = 0;
    k = 125; // 110

    for (uint8_t i = 0; i < doc["iptable"].size(); i++) {
      M5.Lcd.setTextDatum(CL_DATUM);
      sprintf(swap, "%s", iptable_t[i]);
      tmp_str = swap;
      tmp_str += "         ";
      M5.Lcd.drawString(tmp_str, 164, k + j);
      M5.Lcd.setTextDatum(CR_DATUM);
      tmp_str = getValue(iptable_c[i], ' ', 1);
      if (tmp_str == "") {
        tmp_str = "RTFM";
      } else {
        tmp_str = getValue(iptable_c[i], ' ', 1) + ' ' + getValue(iptable_c[i], ' ', 2);
      }
      M5.Lcd.drawString(tmp_str, 315, k + j);
  
      if (i == 9) {
        break;
      }

      j += 12;
    }
  } else {
    M5.Lcd.drawString("DERNIERS LINKS", 240, 110);

    if(refresh == 0 && doc["last"].size() < 10) {
      M5.Lcd.fillRoundRect(161, 118 + (doc["last"].size()) * 12, 158, 120 - (doc["last"].size()) * 12, 4, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));
      refresh = 1;
    }

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_FRONT_R, COLOR_FRONT_G, COLOR_FRONT_B));

    j = 0;
    k = 125; // 110

    for (uint8_t i = 0; i < doc["last"].size(); i++) {
      M5.Lcd.setTextDatum(CL_DATUM);
      sprintf(swap, "%s", last_h[i]);
      tmp_str = swap;
      tmp_str = tmp_str.substring(0, 5);
      tmp_str += "                  ";
      M5.Lcd.drawString(tmp_str, 164, k + j);
      M5.Lcd.setTextDatum(CC_DATUM);
      tmp_str = getValue(last_c[i], ' ', 1);
      if (tmp_str == "") {
        tmp_str = "RTFM";
      } else {
        tmp_str = getValue(last_c[i], ' ', 1) + ' ' + getValue(last_c[i], ' ', 2);
      }
      M5.Lcd.drawString(tmp_str, 240, k + j);
      M5.Lcd.setTextDatum(CR_DATUM);
      M5.Lcd.drawString(last_d[i], 314, k + j);

      j += 12;
    }
  } 

  /*
  Serial.println(type);
  Serial.println(alternance);
  Serial.flush();
  */

  // Transmit or no transmit

  if (tot != 0) { // if transmit
    if(DEBUG) {
      Serial.println("-----> Transmit Start");
    }

    if(transmit_on == 1 || reset == 0) {

      if(DEBUG) {
        Serial.println("Transmit > Change");
        Serial.println((String)"Alternance :" + alternance +" / Type :" + type);
      }

      //M5.Lcd.wakeup();
      M5.Lcd.setBrightness(32);
      M5.Lcd.fillRect(0, 41, 320, 38, TFT_BLACK);
      M5.Lcd.fillRect(0, 0, 320, 43, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));

      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
      M5.Lcd.setFreeFont(&dot15pt7b);
      M5.Lcd.setTextDatum(CC_DATUM);
      sprintf(swap, "%s", salon);
      tmp_str = swap;
      M5.Lcd.drawString(tmp_str, 160, 22);

      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
      M5.Lcd.setFreeFont(ICON_FONT);
      M5.Lcd.setTextDatum(CL_DATUM);
      sprintf(swap, "%c", ICON_CALL);
      tmp_str = swap;
      M5.Lcd.drawString(tmp_str, 10, 20);

      M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
      M5.Lcd.setTextDatum(CL_DATUM);
      tmp_str = getValue(last_c[0], ' ', 1);
      if (tmp_str == "") {
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
    M5.Lcd.drawString(last_d[0], 310, 60);
  }
  else { // if no transmit
    if(DEBUG) {
      Serial.println("-----> Transmit Stop");
    }

    if(transmit_off == 1 || reset == 0) {
      if(DEBUG) {
        Serial.println("No Transmit > Change");
        Serial.println((String)"Alternance :" + alternance +" / Type :" + type);
      }

      //M5.Lcd.sleep();
      M5.Lcd.setBrightness(32);
      M5.Lcd.fillRect(0, 41, 320, 38, TFT_BLACK);
      M5.Lcd.fillRect(0, 0, 320, 43, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));

      M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
      M5.Lcd.setFreeFont(&dot15pt7b);
      M5.Lcd.setTextDatum(CC_DATUM);
      sprintf(swap, "%s", salon);
      tmp_str = swap;
      M5.Lcd.drawString(salon, 160, 22);

      transmit_on = 0;
      transmit_off = 2;
      reset = 1;
    }

    if(type == 0) {
      sprintf(swap, "%s", date);
      tmp_str = swap;
      date_str = getValue(date, ' ', 4);

      if(date_str_old != date_str) {
        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
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
        tx_total_str_old = "";
        emission_str_old = "";
      }
    }
    else if(type == 1) {
      sprintf(swap, "%d", link_total);
      tmp_str = swap;
      link_total_str = "Links total " + tmp_str;

      if(link_total_str_old != link_total_str) {
        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
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
        date_str_old = "";
        tx_total_str_old = "";
        emission_str_old = "";
      }
    }
    else if(type == 2) {
      sprintf(swap, "%d", tx_total);
      tmp_str = swap;
      tx_total_str = "TX total " + tmp_str;

      if(tx_total_str_old != tx_total_str) {
        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
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
        date_str_old = "";
        emission_str_old = "";
      }
    }
    else if(type == 3) {
      sprintf(swap, "%s", emission);
      tmp_str = swap;
      if(strlen(emission) == 5) {
        emission_str = "BF 00:" + tmp_str;
      } else {
        emission_str = "BF " + tmp_str;
      }

      if(emission_str_old != emission_str) {
        M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
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
        date_str_old = "";
      }
    }
  }

  // Baterry

  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(COLOR_HEADER_R, COLOR_HEADER_G, COLOR_HEADER_B));
  M5.Lcd.setTextDatum(CR_DATUM);

  if(M5.Power.isCharging()) {
    sprintf(swap, "%c", ICON_CHARGING);
    tmp_str = swap;
    M5.Lcd.setFreeFont(&Battery_Icons21pt7b);
    M5.Lcd.drawString(tmp_str, 310, 18);
  }
  else {
    if(M5.Power.getBatteryLevel() == 100) {
      sprintf(swap, "%c", ICON_BAT100);
    } else if (M5.Power.getBatteryLevel() == 75) {
      sprintf(swap, "%c", ICON_BAT075);
    } else if (M5.Power.getBatteryLevel() == 50) {
      sprintf(swap, "%c", ICON_BAT050);
    } else if (M5.Power.getBatteryLevel() == 25) {
      sprintf(swap, "%c", ICON_BAT025);
    } else {
      sprintf(swap, "%c", ICON_BAT000);
    }
    tmp_str = swap;
    M5.Lcd.setFreeFont(&Battery_Icons21pt7b);
    M5.Lcd.drawString(tmp_str, 310, 18);
  }

  M5.Lcd.drawFastHLine(0, 0, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(0, 43, 320, TFT_WHITE);


  if(alternance % 10 == 0) {
    refresh = 0;
    if (tot == 0) { 
      transmit_off = 1;
    }

    type += 1;

    if(type == 4) {
      type = 0;
    }
  }

  alternance += 1;
  if(alternance == 41) {
    alternance = 1;
  }

  if(DEBUG) {
    Serial.println("--------------------");
    Serial.flush();
  }

  wait = millis() - timer;
  if (wait < limit) {
    uint8_t j = int((limit - wait) / 10);
    for(uint8_t i = 0; i <= j; i ++) {
      scroll();
      button();
      delay(10);
    }
  }
}