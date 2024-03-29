// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "settings.h"
#include "RRFRemote.h"
#include "font.h"
#include "tools.h"
#include "webIndex.h"
#include "functions.h"
#include "menu.h"
#include "binLoader.h"
#include "tasks.h"
#include "view.h"

// Setup
void setup()
{
  // Init M5
  auto cfg = M5.config();

  cfg.clear_display = true;  // default=true. clear the screen when begin.
  //cfg.output_power  = false;  // default=true. use external port 5V output.
  cfg.internal_imu  = true;  // default=true. use internal IMU.
  cfg.internal_rtc  = true;  // default=true. use internal RTC.
  cfg.internal_spk  = true;  // default=true. use internal speaker.
  cfg.internal_mic  = true;  // default=true. use internal microphone.
  cfg.external_imu  = false;  // default=false. use Unit Accel & Gyro.
  cfg.external_rtc  = false;  // default=false. use Unit RTC.

  cfg.external_display.module_display = true;  // default=true. use ModuleDisplay
  cfg.external_display.atom_display   = true;  // default=true. use AtomDisplay
  cfg.external_display.unit_oled      = false;  // default=true. use UnitOLED
  cfg.external_display.unit_lcd       = false;  // default=true. use UnitLCD
  cfg.external_display.unit_rca       = false; // default=false. use UnitRCA VideoOutput
  cfg.external_display.module_rca     = false; // default=false. use ModuleRCA VideoOutput

  M5.begin(cfg);

  Serial.printf("On start %d\n", M5.getDisplayCount());

  // Manage external buttons for atom only
  if(atom == 1) 
  {  
    pinMode(32, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
  }

  // Preferences
  preferences.begin(NAME);

  size_t n = sizeof(config) / sizeof(config[0]);
  n = (n / 6) - 1;

  // preferences.putUInt("config", 0);

  configCurrent = preferences.getUInt("config", 0);
  if (configCurrent < 0 || configCurrent > n)
  {
    configCurrent = 0;
    preferences.putUInt("config", configCurrent);
  }

  roomCurrent = preferences.getUInt("room", 0);
  if (roomCurrent < 0 || roomCurrent > 6)
  {
    roomCurrent = 0;
    preferences.putUInt("room", roomCurrent);
  }

  colorCurrent = preferences.getUInt("color", 0);
  menuCurrent = preferences.getUInt("menu", 0);
  brightnessCurrent = preferences.getUInt("brightness", 32);
  beepCurrent = preferences.getUInt("beep", 32);
  followCurrent = preferences.getUInt("follow", 0);
  issCurrent = preferences.getUInt("iss", 0);
  totCurrent = preferences.getUInt("tot", 0);
  sysopCurrent = preferences.getUInt("sysop", 0);
  modeCurrent = preferences.getUInt("mode", 0);
  screensaverCurrent = preferences.getUInt("screensaver", 5);
  hdmiCurrent = preferences.getUInt("hdmi", 0);

  // Init Display
  if(M5.getDisplayCount() == 1)
  {
    display = 0;
  }
  else
  {
    display = hdmiCurrent;
  }
  M5.setPrimaryDisplay(display);

  offsetX = (M5.Displays(display).width() - 320) / 2; 
  offsetY = (M5.Displays(display).height() - 240) / 2;

  // Init screensaver timer
  screensaver = millis();

  // Init Led
  if (M5.getBoard() == m5::board_t::board_M5Stack)
  {
    FastLED.addLeds<NEOPIXEL, 15>(leds, NUM_LEDS); // GRB ordering is assumed
  }
  else if (M5.getBoard() == m5::board_t::board_M5StackCore2)
  {
    FastLED.addLeds<NEOPIXEL, 25>(leds, NUM_LEDS); // GRB ordering is assumed
  }

  // Bin Loader
  binLoader();

  // LCD
  resetColor();
  M5.Displays(display).setBrightness(map(brightnessCurrent, 1, 100, 1, 254));
  M5.Displays(display).fillScreen(TFT_HEADER);

  // Title
  M5.Displays(display).setFont(&rounded_led_board10pt7b);
  M5.Displays(display).setTextColor(TFT_WHITE, TFT_HEADER);
  M5.Displays(display).setTextDatum(CC_DATUM);
  M5.Displays(display).drawString(String(NAME), 160 + offsetX, 20 + offsetY);
  M5.Displays(display).setFont(0);
  M5.Displays(display).drawString("Version " + String(VERSION) + " par " + String(AUTHOR), 160 + offsetX, 50 + offsetY);

  // QRCode
  M5.Displays(display).qrcode("https://github.com/armel/" + String(NAME), 90 + offsetX, 80 + offsetY, 140, 6);

  // We start by connecting to the WiFi network
  M5.Displays(display).setTextPadding(320);

  uint32_t color = ((BMP_HEADER.r & 0xff) << 16) + ((BMP_HEADER.g & 0xff) << 8) + (BMP_HEADER.b & 0xff);

  WiFi.onEvent(callbackWifiOn, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(callbackWifiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(callbackWifiOff, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  while (true)
  {
    uint8_t attempt = 1;
    M5.Displays(display).drawString(String(config[(configCurrent * 6)]), 160 + offsetX, 60 + offsetY);
    WiFi.begin(config[(configCurrent * 6)], config[(configCurrent * 6) + 1]);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      if (attempt % 2 == 0)
      {
        M5.Displays(display).drawString("Connexion en cours", 160 + offsetX, 70 + offsetY);
        for (uint8_t j = 0; j <= 4; j++)
        {
          leds[j] = color;
          leds[9 - j] = color;
        }
        FastLED.setBrightness(16);
        FastLED.show();
      }
      else
      {
        M5.Displays(display).drawString(" ", 160 + offsetX, 70 + offsetY);
        for (uint8_t j = 0; j <= 4; j++)
        {
          leds[j] = CRGB::Black;
          leds[9 - j] = CRGB::Black;
        }
        FastLED.setBrightness(16);
        FastLED.show();
      }
      attempt++;
      if (attempt > 10)
      {
        break;
      }
    }
    if (WiFi.status() != WL_CONNECTED)
    {
      menuCurrent = 0;
      preferences.putUInt("menu", menuCurrent);

      configCurrent += 1;
      if (configCurrent > n)
      {
        configCurrent = 0;
        preferences.putUInt("config", configCurrent);
      }
    }
    else
    {
      preferences.putUInt("config", configCurrent);
      break;
    }
  }

  clientISS.setInsecure(); // For ISS

  M5.Displays(display).drawString(String(WiFi.localIP().toString().c_str()), 160 + offsetX, 70 + offsetY);

  // Create menu
  if ((String)config[(configCurrent * 6) + 5] != "")
  {
    menuSize = sizeof(menuSpotnikOn);
    settingsMenu = (char **)malloc(menuSize);
    memcpy(settingsMenu, menuSpotnikOn, menuSize);
    menuSize = sizeof(menuSpotnikOn) / sizeof(menuSpotnikOn[0]);
  }
  else
  {
    menuSize = sizeof(menuSpotnikOff);
    settingsMenu = (char **)malloc(menuSize);
    memcpy(settingsMenu, menuSpotnikOff, menuSize);
    menuSize = sizeof(menuSpotnikOff) / sizeof(menuSpotnikOff[0]);

    followCurrent = (followCurrent == 1) ? 0 : 0;
    preferences.putUInt("follow", followCurrent);
  }

  // Scroll
  pos = 320;
  Sprite.setTextSize(1); // Font size scaling is x1
  Sprite.setTextFont(2); // Font 2 selected
  Sprite.createSprite(pos, 20);

  // Init and get time
  configTzTime(ntpTimeZone, ntpServer);
  updateLocalTime();

  // Start server (for Web site Screen Capture)
  httpServer.begin();

  // Multitasking task for retreive rrf, spotnik and propag data
  xTaskCreatePinnedToCore(
      rrfdata,        // Function to implement the task
      "rrfdata",      // Name of the task
      8192,           // Stack size in words
      NULL,           // Task input parameter
      18,             // Priority of the task
      &rrfdataHandle, // Task handle
      1);             // Core where the task should run

  xTaskCreatePinnedToCore(
      button,        // Function to implement the task
      "button",      // Name of the task
      8192,          // Stack size in words
      NULL,          // Task input parameter
      4,             // Priority of the task
      &buttonHandle, // Task handle
      1);            // Core where the task should run

  xTaskCreatePinnedToCore(
      hamdata,   // Function to implement the task
      "hamdata", // Name of the task
      8192,      // Stack size in words
      NULL,      // Task input parameter
      1,         // Priority of the task
      NULL,      // Task handle
      1);        // Core where the task should run

  // Let's go after temporisation
  delay(1000);

  for (uint8_t i = 0; i <= 120; i++)
  {
    M5.Displays(display).drawFastHLine(0 + offsetX, i + offsetY, 320, TFT_BLACK);
    M5.Displays(display).drawFastHLine(0 + offsetX, 240 - i + offsetY, 320, TFT_BLACK);
    delay(5);
  }

  M5.Displays(display).fillScreen(TFT_BLACK);
  clear();
}

// Main loop
void loop()
{
  static DynamicJsonDocument doc(8192);
  DynamicJsonDocument docNew(8192);
  StaticJsonDocument<512> docISS;

  const char *emission = "", *salon = "", *entrant = "", *sortant = "";
  const char *lastIndicatif[10], *lastDuree[10], *lastHeure[10], *lastLocator[10], *lastRegion[10], *lastPrenom[10];
  const char *allIndicatif[10], *allDuree[10];
  const char *iptableIndicatif[10], *iptableType[10];

  float lastLatitude[10], lastLongitude[10];

  uint16_t allTx[10];
  uint16_t tot = 0, linkTotal = 0, linkActif = 0, txTotal = 0, maxLevel = 0, tx[24] = {0};
  uint16_t distance = 0;

  int8_t optimize = 0;

  static uint8_t lengthData = 0;
  static uint8_t centerData = 0;

  uint32_t timer = 0;

  // Let's go
  settingLock = false;
  //display = hdmiCurrent;

  if (reset == 0)
  {
    clear();
  }

  timer = millis();

  // Continue
  optimize = jsonDataNew.compareTo(jsonData);
  if (optimize != 0) // If change
  {
    DeserializationError error = deserializeJson(docNew, jsonDataNew);
    if (!error) // JSON is valid
    {
      doc = docNew;
      jsonData = jsonDataNew;
    }
  }

  salon = doc["abstract"][0]["Salon"];

  if (strcmp(room[roomCurrent], salon) != 0)
  {
    while (strcmp(room[roomCurrent], salon) != 0)
    {
      jsonData = jsonDataNew;
      DeserializationError error = deserializeJson(doc, jsonData);
      if (!error)
      {
        salon = doc["abstract"][0]["Salon"];
      }
    }
  }

  emission = doc["abstract"][0]["Emission cumulée"];
  linkTotal = doc["abstract"][0]["Links connectés"];
  linkActif = doc["abstract"][0]["Links actifs"];
  txTotal = doc["abstract"][0]["TX total"];
  entrant = doc["abstract"][0]["Links entrants"];
  sortant = doc["abstract"][0]["Links sortants"];
  dst = doc["abstract"][0]["DST"];
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
    lastPrenom[i] = doc["last"][i]["Prenom"];
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
  if (tot == 0 || lastLatitude[0] == 0)
  {
    tmpString = String(entrant);
    if (tmpString.length() > 0)
    {
      message = "+ " + tmpString;
      tmpString = String(sortant);
      if (tmpString.length() > 0)
      {
        message += " - " + tmpString;
      }
    }
    else
    {
      tmpString = String(sortant);
      if (tmpString.length() > 0)
      {
        message = "- " + tmpString;
      }
    }
  }
  else
  {
    distance = computeDistance(lastLatitude[0], lastLongitude[0]);
    message = "";
    if (String(lastPrenom[0]) != "")
    {
      message += String(lastPrenom[0]) + " - ";
    }
    message += String(lastRegion[0]) + " - " + String(lastLocator[0]) + " - " + String(distance) + " Km";
  }

  // Check network

  if (ping != 0)
  {
    switch (ping)
    {
    case 1:
      message = "Perte de connexion Spotnik";
      break;
    case 2:
      message = "Perte de connexion Wifi";
      break;
    }
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

  if(settingsMode == false) // Lock menu operation...
  {
    settingLock = true;

    if (modeCurrent == 0)
    {
      // View histogram
      viewHistogram(maxLevel, tx);

      // View elsewhere
      viewElsewhere(doc, salon);

      // View many datas
      M5.Displays(display).setFont(&tahoma6pt7b);
      M5.Displays(display).setTextColor(TFT_WHITE, TFT_BACK);
      M5.Displays(display).setTextDatum(CC_DATUM);

      if (tot > 0)
      {
        if (type != 0)
        {
          refresh = 0;
        }

        alternance = 5;
        type = 0;
      }

      scroll(20);
      
      switch (type)
      {
      case 1:
        viewSettings();
        break;

      case 2:
        viewPropagation();
        break;

      case 3:
        if (doc["all"].size() != 0)
        {
          viewTopLinks(doc["all"].size(), allTx, allIndicatif, allDuree, salon);
        }
        else
        {
          viewLastLinks(doc["last"].size(), lastHeure, lastIndicatif, lastDuree, salon);
        }
        break;

      case 4:
        if (doc["iptable"].size() != 0)
        {
          viewBlocage(doc["iptable"].size(), iptableIndicatif, iptableType, salon);
        }
        else
        {
          viewLastLinks(doc["last"].size(), lastHeure, lastIndicatif, lastDuree, salon);
        }
        break;

      case 5:
        if (issCurrent == 1)
        {
          viewISS(docISS);
        }
        else
        {
          viewLastLinks(doc["last"].size(), lastHeure, lastIndicatif, lastDuree, salon);
        }
        break;

      default:
        if (doc["last"].size() != 0)
        {
          viewLastLinks(doc["last"].size(), lastHeure, lastIndicatif, lastDuree, salon);
        }
      }
    }
    else if (modeCurrent == 1)
    {
      viewElsewhereBig(doc, salon);
      if (doc["last"].size() != 0)
      {
        viewLastLinksBig(doc["last"].size(), lastHeure, lastIndicatif, lastDuree, salon);
      }
    }
    else
    {
      viewDTMF();
    }

    // Transmit or no transmit
    scroll(20);
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
            M5.Displays(display).setBrightness(map(brightnessCurrent, 1, 100, 1, 254));
          }

          M5.Displays(display).fillRect(4 + offsetX, 2 + offsetY, 36, 42, TFT_HEADER);

          M5.Displays(display).setTextColor(TFT_WHITE, TFT_HEADER);
          M5.Displays(display).setFont(&dot15pt7b);
          M5.Displays(display).setTextDatum(CC_DATUM);
          M5.Displays(display).setTextPadding(240);
          M5.Displays(display).drawString(String(salon), 160 + offsetX, 16 + offsetY);

          M5.Displays(display).setTextColor(TFT_WHITE, TFT_HEADER);
          M5.Displays(display).setFont(ICON_FONT);
          M5.Displays(display).setTextDatum(CL_DATUM);
          M5.Displays(display).setTextPadding(20);
          sprintf(swap, "%c", ICON_CALL);
          tmpString = swap;
          M5.Displays(display).drawString(tmpString, 10 + offsetX, 22 + offsetY);

          M5.Displays(display).fillRect(0 + offsetX, 46 + offsetY, 320, 32, TFT_INFO);

          M5.Displays(display).setFont(&rounded_led_board10pt7b);
          M5.Displays(display).setTextColor(TFT_WHITE, TFT_INFO);
          M5.Displays(display).setTextDatum(CL_DATUM);

          tmpString = String(lastIndicatif[0]);
          indicatifString = tmpString;

          ledAlert(true);

          if (tmpString.substring(0, 3) == "GW-")
          {
            tmpString = "GW";
          }
          else
          {
            tmpString = getValue(lastIndicatif[0], ' ', 1);
            if (tmpString.length() > 6)
            {
              tmpString = tmpString.substring(0, 6);
            }
            tmpString = (tmpString == "") ? "RTFM" : tmpString;
          }

          tmpString += " 0:00";
          lengthData = M5.Displays(display).textWidth(tmpString);
          centerData = (320 - lengthData) / 2;

          M5.Lcd.drawString(tmpString, centerData + offsetX, 64 + offsetY);

          transmitOn = 2;
          transmitOff = 0;
        }

        M5.Displays(display).setFont(&rounded_led_board10pt7b);
        M5.Displays(display).setTextColor(TFT_WHITE, TFT_INFO);
        M5.Displays(display).setTextDatum(CL_DATUM);
        dureeString = String(lastDuree[0]);
        if (dureeStringOld != dureeString) 
        {
          dureeStringOld = dureeString;
          M5.Displays(display).drawString(dureeString.substring(1), (centerData + lengthData - 80) + offsetX, 64 + offsetY);
        }

        if (totCurrent)
        {
          if ((String(salon) == "RRF" && tot > TIMEOUT_TOT_RRF) || (tot > TIMEOUT_TOT_ELSEWHERE))
          {
            M5.Displays(display).setBrightness(0);
            M5.Speaker.tone(1000, 50);
            delay(10);
            M5.Speaker.tone(1000, 50);
            delay(80);
            M5.Displays(display).setBrightness(map(brightnessCurrent, 1, 100, 1, 254));
          }
        }
      }
      else
      { // If no transmit
        if (transmitOff == 1 || reset == 0)
        {
          ledAlert(false);

          if (!isCharging() && screensaverMode == 0)
          {
            M5.Displays(display).setBrightness(map(brightnessCurrent, 1, 100, 1, 254));
          }

          M5.Displays(display).setTextColor(TFT_WHITE, TFT_HEADER);
          M5.Displays(display).setFont(&dot15pt7b);
          M5.Displays(display).setTextDatum(CC_DATUM);
          M5.Displays(display).setTextPadding(240);
          M5.Displays(display).drawString(String(salon), 160 + offsetX, 16 + offsetY);

          dateStringOld = "";
          linkTotalStringOld = "";
          linkActifStringOld = "";
          txTotalStringOld = "";
          emissionStringOld = "";
          issStringOld = "";

          transmitOn = 0;
          transmitOff = 2;
        }

        M5.Displays(display).setFont(ICON_FONT);
        M5.Displays(display).setTextColor(TFT_WHITE, TFT_HEADER);
        M5.Displays(display).setTextDatum(CL_DATUM);
        M5.Displays(display).setTextPadding(0);

        switch (type)
        {
        case 0:
          updateLocalTime();
          dateStringOld = viewData(ICON_CLOCK, dateString, dateStringOld);
          break;

        case 1:
          sprintf(swap, "%d", linkTotal);
          tmpString = swap;
          linkTotalString = "Links total " + tmpString;
          linkTotalStringOld = viewData(ICON_STAT, linkTotalString, linkTotalStringOld);
          break;

        case 2:
          sprintf(swap, "%d", linkActif);
          tmpString = swap;
          linkActifString = "Links actifs " + tmpString;
          linkActifStringOld = viewData(ICON_STAT, linkActifString, linkActifStringOld);
          break;

        case 3:
          sprintf(swap, "%d", txTotal);
          tmpString = swap;
          txTotalString = "TX total " + tmpString;
          txTotalStringOld = viewData(ICON_STAT, txTotalString, txTotalStringOld);
          break;

        case 4:
          tmpString = String(emission);
          emissionString = (strlen(emission) == 5) ? "BF 00:" + tmpString : "BF " + tmpString;
          emissionStringOld = viewData(ICON_CLOCK, emissionString, emissionStringOld);
          break;

        case 5:
          if (issCurrent == 1)
          {
            tmpString = String(issDistance);
            issString = "ISS " + tmpString + " Km";
            issStringOld = viewData(ICON_ISS, issString, issStringOld);
          }
          else
          {
            tmpString = String(emission);
            emissionString = (strlen(emission) == 5) ? "BF 00:" + tmpString : "BF " + tmpString;
            emissionStringOld = viewData(ICON_CLOCK, emissionString, emissionStringOld);
          }
          break;
        }
      }

      // Baterry
      viewBattery();
    }

    // View Baseline
    scroll(20);
    viewBaseline();
 

    // Manage alternance and type
    if (menuMode == 0)
    {
      alternance++;
      if (alternance == 10)
      {
        refresh = 0;
        if (issCurrent == 1 && modeCurrent == 0)
        {
          type = (type++ < 5) ? type : 0;
        }
        else
        {
          type = (type++ < 4) ? type : 0;
        }
        alternance = 0;
      }
    }

    // Manage screensaver
    if (screensaverMode == 0 && millis() - screensaver > screensaverCurrent * 60 * 1000)
    {
      for (uint8_t i = brightnessCurrent; i >= 1; i--)
      {
        M5.Displays(display).setBrightness(map(i, 1, 100, 1, 254));
        scroll(0);
        delay(50);
      }
      screensaverMode = 1;
      M5.Displays(display).sleep();
    }
    else if (screensaverMode == 1 && millis() - screensaver < screensaverCurrent * 60 * 1000)
    {
      M5.Displays(display).wakeup();
      screensaverMode = 0;
      for (uint8_t i = 1; i <= brightnessCurrent; i++)
      {
        M5.Displays(display).setBrightness(map(i, 1, 100, 1, 254));
        scroll(0);
        delay(50);
      }
    }

    // Manage action
    reset = (reset == 0) ? 1 : 1;

    switch (action)
    {
    case 1:
      menuRefresh = 0;
      break;
    case 2:
      reset = 0;
      refresh = 0;
      break;
    case 3:
      reset = 0;
      refresh = 0;
      menuRefresh = 0;
      break;
    case 4:
      reset = 0;
      refresh = 0;
      menuMode = 0;
      menuSelected = -1;
      break;
    case 5:
      reset = 0;
      refresh = 0;
      modeCurrent = modeNew;
      break;
    }

    action = 0;

    // Manage rotation
    if (menuMode != 1)
    {
      checkAcceleration();
    }

    // Manage Wifi
    checkWifi();

    // Manage follow
    if (followCurrent == 1 && menuMode != 1)
    {

      size_t n = sizeof(room) / sizeof(room[0]);
      n -= 1;

      for (uint8_t i = 0; i <= n; i++)
      {
        if (dtmf[i] == whereisCurrent)
        {
          if (i != roomCurrent)
          {
            roomCurrent = i;
            preferences.putUInt("room", roomCurrent);
            reset = 0;
            refresh = 0;
          }
          break;
        }
      }
    }

    // Manage temporisation

    // Serial.print(millis() - timer);
    // Serial.print(" - ");

    if (menuMode == 0 && htmlGetRefresh != 2)
    {
      while (millis() - timer < REFRESH)
      {
        scroll(20);
      }
    }

    // Serial.println(millis() - timer);

    // Manage Web site Screen Capture

    if (htmlGetRefresh == 2)
    {
      htmlGetRefresh = 3;
    }
    else if (htmlGetRefresh == 3)
    {
      getScreenshot();
    }
  }
  else
  {
      getScreenshot();
  }
}