// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Ini error messages
void printErrorMessage(uint8_t e, bool eol = true)
{
  char error[64];

  switch (e) {
  case IniFile::errorNoError:
    strcpy(error, "No error");
    break;
  case IniFile::errorFileNotFound:
    strcpy(error, "File not found");
    break;
  case IniFile::errorFileNotOpen:
    strcpy(error, "File not open");
    break;
  case IniFile::errorBufferTooSmall:
    strcpy(error, "Buffer too small");
    break;
  case IniFile::errorSeekError:
    strcpy(error, "Seek error");
    break;
  case IniFile::errorSectionNotFound:
    strcpy(error, "Section not found");
    break;
  case IniFile::errorKeyNotFound:
    strcpy(error, "Key not found");
    break;
  case IniFile::errorEndOfFile:
    strcpy(error, "End of file");
    break;
  case IniFile::errorUnknownError:
    strcpy(error, "Unknown error");
    break;
  default:
    strcpy(error, "Unknown error value");
    break;
  }

  Serial.println(error);

  if (eol)
    Serial.println();
}

// Ini log message
void iniLogMessage(char* message, uint16_t x = 160, uint16_t y = 50, boolean verbose = true)
{
  if(verbose == true)
  {
    M5.Displays(display).drawString(message, x, y);
  }
  else
  {
    Serial.println(message);
  }
}

void iniParser(String fileNameSelected, char* media, boolean verbose)
{
  const size_t bufferLen = 128;
  char buffer[bufferLen];

  IniFile ini(fileNameSelected.c_str(), FILE_READ, false, media);

  Serial.println(fileNameSelected.c_str());

  if (strcmp(media, "SPIFFS") == 0)
  {
    if (SPIFFS.begin())
    {
      if(!SPIFFS.exists(fileNameSelected.c_str()))
      {
        iniLogMessage((char *) "Ini file does not exist.", 160, 50, verbose);
        vTaskDelay(pdMS_TO_TICKS(500));
        return;
      }
      else if (!ini.open()) {
        iniLogMessage((char *) "Ini file does not exist.", 160, 50, verbose);
        vTaskDelay(pdMS_TO_TICKS(500));
        return;
      }
      iniLogMessage((char*) "Find ini file.", 160, 50, verbose);
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
  else
  {
    if (SD.begin(GPIO_NUM_4, SPI, 25000000))
    {
      if(!SD.exists(fileNameSelected.c_str()))
      {
        iniLogMessage((char *) "Ini file does not exist.", 160, 50, verbose);
        vTaskDelay(pdMS_TO_TICKS(500));
        return;
      }
      else if (!ini.open()) {
        iniLogMessage((char *) "Ini file does not exist.", 160, 50, verbose);
        vTaskDelay(pdMS_TO_TICKS(500));
        return;
      }
      iniLogMessage((char*) "Find ini file.", 160, 50, verbose);
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }

  if (!ini.validate(buffer, bufferLen)) {
    iniLogMessage((char*) "Ini file is not valid.", 160, 50, verbose);
    vTaskDelay(pdMS_TO_TICKS(500));
    printErrorMessage(ini.getError());
    return;
  }
  else
  {
    iniLogMessage((char*) "Ini file is valid.", 160, 50, verbose);
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  // Common settings

  // Wifi settings

  // Fetch wifi_ssid
  if (ini.getValue("wifi", "wifi_ssid", buffer, bufferLen)) {
    strncpy(myConfig.wifiSSID, buffer, 64);
    Serial.println(myConfig.wifiSSID);
  }
  else {
    Serial.print("Could not read 'wifi_ssid' from section 'wifi', error was ");
    printErrorMessage(ini.getError());
    iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
    return;
  }

  // Fetch wifi_password
  if (ini.getValue("wifi", "wifi_password", buffer, bufferLen)) {
    strncpy(myConfig.wifiPassword, buffer, 64);
    Serial.println(myConfig.wifiPassword);
  }
  else {
    Serial.print("Could not read 'wifi_password' from section 'wifi', error was ");
    printErrorMessage(ini.getError());
    iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
    return;
  }

  if(strcmp(NAME, "DXTracker") != 0)
  {
    // Icom settings

    // Fetch icom_model
    if (ini.getValue("icom", "icom_model", buffer, bufferLen)) {
      myConfig.icModel = atoi(buffer);
      Serial.printf("%lu\n", myConfig.icModel);
    }
    else {
      Serial.print("Could not read 'icom_model' from section 'icom', error was ");
      printErrorMessage(ini.getError());
      iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
      return;
    }

    // Fetch icom_address
    if (ini.getValue("icom", "icom_address", buffer, bufferLen)) {
      myConfig.icAddress = strtol(buffer, 0, 16);
      Serial.printf("%02X\n", myConfig.icAddress);
    }
    else {
      Serial.print("Could not read 'icom_address' from section 'icom', error was ");
      printErrorMessage(ini.getError());
      iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
      return;
    }

    // Fetch icom_connect
    if (ini.getValue("icom", "icom_connect", buffer, bufferLen)) {
      if(String(buffer) == "USB")
      {
        myConfig.icConnect = USB;
      }
      else
      {
        myConfig.icConnect = BT;
      }
      Serial.println(myConfig.icConnect);
    }
    else {
      Serial.print("Could not read 'icom_connect' from section 'icom', error was ");
      printErrorMessage(ini.getError());
      iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
      return;
    }

    if(myConfig.icConnect == USB)
    {
      if (ini.getValue("icom", "icom_device", buffer, bufferLen)) {
        strncpy(myConfig.icDevice, buffer, 64);
        Serial.println(myConfig.icDevice);
      }
      else {
        Serial.print("Could not read 'icom_device' from section 'icom', error was ");
        printErrorMessage(ini.getError());
        iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
        return;
      }
    }
    else
    {
      if (ini.getValue("icom", "icom_device", buffer, bufferLen)) {
        strncpy(myConfig.icDevice, buffer, 64);
        uint8_t i = 0;
        while(i <= 15)
        {
          myConfig.icMAC[i/3] = strtol(String(myConfig.icDevice).substring(i, i + 2).c_str(), 0, 16);
          Serial.printf("%02X ", myConfig.icMAC[i/3]);
          i += 3;
        }
        Serial.println("");
      }
      else {
        Serial.print("Could not read 'icom_device' from section 'icom', error was ");
        printErrorMessage(ini.getError());
        iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
        return;
      }
    }

    // Proxy settings

    // Fetch baud rate
    if (ini.getValue("proxy", "baud_rate", buffer, bufferLen)) {
      myConfig.baudRate = atoi(buffer);
      Serial.println(myConfig.baudRate);
    }
    else {
      Serial.print("Could not read 'baud_rate' from section 'proxy', error was ");
      printErrorMessage(ini.getError());
      iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
      return;
    }

    // Fetch proxy url
    if (ini.getValue("proxy", "proxy_url", buffer, bufferLen)) {
      strncpy(myConfig.proxyURL, buffer, 128);
      Serial.println(myConfig.proxyURL);
    }
    else {
      Serial.print("Could not read 'proxy_url' from section 'proxy', error was ");
      printErrorMessage(ini.getError());
      iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
      return;
    }

    // Fetch proxy port
    if (ini.getValue("proxy", "proxy_port", buffer, bufferLen)) {
      myConfig.proxyPort = atoi(buffer);
      Serial.println(myConfig.proxyPort);
    }
    else {
      Serial.print("Could not read 'proxy_port' from section 'proxy', error was ");
      printErrorMessage(ini.getError());
      iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
      return;
    }

    for(uint8_t i = 1; i <= 5; i++)
    {
      // Fetch transverter_lo_
      String tmp = String("transverter_lo_") + String(i);
      if (ini.getValue("transverter", tmp.c_str(), buffer, bufferLen)) {
        myConfig.lo[i] = atoll(buffer);
        Serial.println(myConfig.lo[i]);
      }
      else {
        Serial.printf("Could not read 'transverter_lo_%d' from section 'transverter', error was ", i);
        printErrorMessage(ini.getError());
        iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
        return;
      }
    }
  }

  if(strcmp(NAME, "DXTracker") == 0)
  {
    // Geolocation settings

    // Fetch latitude
    if (ini.getValue("geolocation", "latitude", buffer, bufferLen)) {
      myConfig.latitude = atof(buffer);
      Serial.println(myConfig.latitude);
    }
    else {
      Serial.print("Could not read 'latitude' from section 'geolocation', error was ");
      printErrorMessage(ini.getError());
      iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
      return;
    }

    // Fetch longitude
    if (ini.getValue("geolocation", "longitude", buffer, bufferLen)) {
      myConfig.longitude = atof(buffer);
      Serial.println(myConfig.longitude);
    }
    else {
      Serial.print("Could not read 'longitude' from section 'geolocation', error was ");
      printErrorMessage(ini.getError());
      iniLogMessage((char*) "Ini file parsing error.", 160, 50, verbose);
      return;
    }
  }

  iniLogMessage((char*) "Ini file parsing is finished.", 160, 50, verbose);
  vTaskDelay(pdMS_TO_TICKS(500));
  iniValid = true;
  SD.end(); // If not Bluetooth doesn't work !!!
}

// Save Backup
void backupSave(String fileNameSelected, char* media, boolean verbose)
{
  File sourceFile, destFile;
  String tmpName;
  const size_t bufferLen = 128;
  uint8_t buffer[bufferLen];

  iniLogMessage((char*) "Saving config file...", 160, 50, verbose);

  if (strcmp(media, "SPIFFS") == 0)
  {
    SPIFFS.begin();
    sourceFile = SPIFFS.open(fileNameSelected.c_str());
    destFile = SPIFFS.open("/.backup.ini", FILE_WRITE);
  }
  else
  {
    if (SD.begin(GPIO_NUM_4, SPI, 25000000))
    {
      SPIFFS.begin();
      sourceFile = SD.open(fileNameSelected.c_str());
      destFile = SPIFFS.open("/.backup.ini", FILE_WRITE);     
    }
  }

  while( sourceFile.read(buffer, bufferLen)) {
    destFile.write(buffer, bufferLen);
  }
  destFile.close();
  sourceFile.close();

  SD.end();
  SPIFFS.end();
}

// Load Backup
void backupLoad(String fileNameSelected, boolean verbose)
{
  String tmpName;

  M5.Displays(display).setTextColor(TFT_DARKGRAY, TFT_BLACK);
  iniLogMessage((char*) "Loading config file...", 160, 50, verbose);

  iniParser("/.backup.ini", (char*) "SPIFFS", verbose);
  
  if(iniValid == false)
  {
    M5.Displays(display).drawString("Please, add a valid config file", 160, 110);
    M5.Displays(display).drawString("on SPIFFS or SD Card.", 160, 130);
  }

  while(iniValid == false)
  {
    M5.Displays(display).drawString("Failed to load config file.", 160, 70);
    vTaskDelay(pdMS_TO_TICKS(500));
    M5.Displays(display).drawString(" ", 160, 70);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  return;
}

// Ini loader
void iniLoader() {
  String tmpName;

  char version[] = "Ini Loader V0.4";

  boolean click = false;
  boolean blink = false;

  int8_t pos = 0;
  int8_t cursor = 0;
  int8_t start = 0;
  int8_t stop = 0;
  int8_t limit = 8;
  int8_t change = 255;

  long gauge = 0;

  fileIndex = 0;

  root = SPIFFS.open("/");
  getFileList(root, (char*)"SPIFFS", (char*)".ini");

  if (SD.begin(GPIO_NUM_4, SPI, 25000000))
  {
    root = SD.open("/");
    getFileList(root, (char*)"SD", (char*)".ini");
    SD.end(); // If not Bluetooth doesn't work !!!
  }

  M5.Displays(display).fillScreen(TFT_BLACK);

  M5.Displays(display).setFont(&tahoma8pt7b);
  M5.Displays(display).setTextColor(TFT_DARKGRAY, TFT_BLACK);
  M5.Displays(display).setTextDatum(CC_DATUM);
  M5.Displays(display).setTextPadding(320);

  if (fileIndex != 0)
  {
    M5.Displays(display).drawString(version, 160, 20);
    M5.Displays(display).drawRect(20, 28, 280, 10, TFT_DARKGRAY);

    for (uint16_t i = 0; i < TIMEOUT_INI_LOADER * 10; i++)
    {
      gauge = map(i, 0, (TIMEOUT_INI_LOADER * 10) - 1, 0, 276);
      if(gauge != 0)
      {
        for (uint8_t j = 0; j <= 5; j++)
        {
          M5.Displays(display).drawGradientHLine(22, 30 + j, gauge, TFT_BLACK, TFT_SKYBLUE);
        }

        for (uint16_t j = 1; j < gauge; j++)
        {
          if(j % 23 == 0)
          {
            M5.Displays(display).drawFastVLine(22 + j, 30, 6, TFT_BLACK);
          }
        }
      }

      getButton();

      if (i % 5 == 0)
      {
        if(blink == false)
        {
          M5.Displays(display).drawString("Push middle button to enter", 160, 50);
          blink = true;
        }
        else
        {
          M5.Displays(display).drawString("", 160, 50);
          blink = false;
        }
      }

      if (btnA || btnC)
      {
        backupLoad("/.backup.ini", true);
        return;
      }
      else if (btnB)
      {
        click = true;
        M5.Displays(display).fillScreen(TFT_BLACK);
        break;
      }

      vTaskDelay(100);
    }

    while (click == true)
    {
      while (btnB != 0)
      {
        getButton();
        vTaskDelay(100);
      }

      M5.Displays(display).setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Displays(display).drawString(version, 160, 20);

      getButton();

      if (btnA)
      {
        cursor--;
      }
      else if (btnC)
      {
        cursor++;
      }
      else if (btnB)
      {
        pos = String(fileName[cursor]).indexOf('/');
        switch (pos)
        {
        case 6:
          iniParser(String(fileName[cursor]).substring(pos), (char*) "SPIFFS", true);
          if(iniValid == true)
          {
            backupSave(String(fileName[cursor]).substring(pos), (char*) "SPIFFS", true);
            return;
          }
          break;
        case 2:
          iniParser(String(fileName[cursor]).substring(pos), (char*) "SD", true);
          if(iniValid == true)
          {
            backupSave(String(fileName[cursor]).substring(pos), (char*) "SD", true);
            return;
          }
          break;  
        }
      }

      cursor = (cursor < 0) ? fileIndex - 1 : cursor;
      cursor = (cursor > fileIndex - 1) ? 0 : cursor;

      start = cursor / limit;
      stop = (start * limit) + limit;

      if(stop > fileIndex)
      {
        stop = fileIndex;
      }

      if (change != cursor)
      {
        change = cursor;
        M5.Displays(display).setTextPadding(320);

        uint8_t i = 0;
        for (uint8_t j = (start * limit); j < stop; j++)
        {
          pos = String(fileName[j]).indexOf('/');
          switch (pos)
          {
          case 6:
            tmpName = String(fileName[j]).substring(pos + 1);
            if(j == cursor)
            {
              tmpName = ">> " + tmpName + " <<";
              M5.Displays(display).drawString("SPI Flash File Storage", 160, 50);
            }
            break;
          case 2:
            tmpName = String(fileName[j]).substring(pos + 1);
            if(j == cursor)
            {
              tmpName = ">> " + tmpName + " <<";
              M5.Displays(display).drawString("SD Card Storage", 160, 50);
            }
            break;  
          }

          M5.Displays(display).drawString(tmpName, 160, 80 + i * 20);
          i++;
        }
      }
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    // Else try to load previous
    backupLoad("/.backup.ini", true);
  }
  else
  {
    // Else try to load previous
    backupLoad("/.backup.ini", false);
  }
}