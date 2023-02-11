// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Add insensitive function to prevent case problem
char* stristr(const char* haystack, const char* needle) {
  do {
    const char* h = haystack;
    const char* n = needle;
    while (tolower((unsigned char) *h) == tolower((unsigned char ) *n) && *n) {
      h++;
      n++;
    }
    if (*n == 0) {
      return (char *) haystack;
    }
  } while (*haystack++);
  return 0;
}

// List files on SPIFFS or SD
void getFileList(File dir, char* media, char* extension)
{
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }

    if (String(entry.name()).substring(0, 1) != "." && stristr(entry.name(), extension) != NULL)
    {
      String tmp = String(media) + "/" + String(entry.name());
      Serial.println(tmp);
      strcpy(fileName[fileIndex], tmp.c_str());
      fileIndex++;
    }

    entry.close();
  }
  Serial.println("----------");
}

void myProgressFunction( int state, int size )
{
  static int SD_UI_Progress;
  static boolean blink = false;
  uint16_t gauge;

  int percent = ( state * 100 ) / size;
  if( percent == SD_UI_Progress ) {
    // don't render twice the same value
    return;
  }
  SD_UI_Progress = percent;

  gauge = map(percent, 0, 100, 0, 276);
  if(gauge != 0)
  {
    for (uint8_t j = 0; j <= 5; j++)
    {
      M5.Displays(display).drawGradientHLine(22, 30 + j, gauge, TFT_BLACK, TFT_RED);
    }

    for (uint16_t j = 1; j < gauge; j++)
    {
      if(j % 23 == 0)
      {
        M5.Displays(display).drawFastVLine(22 + j, 30, 6, TFT_BLACK);
      }
    }
  }

  M5.Displays(display).drawString(String(percent) + " %", 160, 70);

  if (percent % 2 == 0)
  {
    if(blink == false)
    {
      M5.Displays(display).drawString("Loading in progress, please wait...", 160, 50);
      blink = true;
    }
    else
    {
      M5.Displays(display).drawString("", 160, 50);
      blink = false;
    }
  }
}

static int myActionTrigger(char* labelLoad,  char* labelSkip, char* labelSave, unsigned long waitdelay)
{
  return 1;
}

// Bin Loader
void binLoader()
{
  String tmpName;

  char version[] = "Bin Loader V0.4";

  boolean click = false;
  boolean blink = false;

  int8_t pos = 0;
  int8_t cursor = 0;
  int8_t start = 0;
  int8_t stop = 0;
  int8_t limit = 8;
  int8_t change = 255;

  uint16_t gauge = 0;

  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");

    M5.Displays(display).fillScreen(TFT_BLACK);

    M5.Displays(display).setFont(&tahoma8pt7b);
    M5.Displays(display).setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Displays(display).setTextDatum(CC_DATUM);
    M5.Displays(display).drawString("BUILD SPIFFS", 160, 20);

    M5.Displays(display).drawString("SPI Flash File System", 160, 80);
    M5.Displays(display).drawString("needs to be formated.", 160, 100);
    M5.Displays(display).drawString("It takes around 4 minutes.", 160, 140);
    M5.Displays(display).drawString("Please, wait until ", 160, 180);
    M5.Displays(display).drawString("the application starts !", 160, 200);

    Serial.println("SPIFFS Formating...");

    SPIFFS.format(); // Format SPIFFS...
  }

  fileIndex = 0;
  root = SPIFFS.open("/");
  getFileList(root, (char*)"SPIFFS", (char*)".bin");

  if (SD.begin(GPIO_NUM_4, SPI, 25000000))
  {
    root = SD.open("/");
    getFileList(root, (char*)"SD", (char*)".bin");
  }

  if (fileIndex != 0)
  {
    M5.Displays(display).fillScreen(TFT_BLACK);

    M5.Displays(display).setFont(&tahoma8pt7b);
    M5.Displays(display).setTextColor(TFT_DARKGRAY, TFT_BLACK);
    M5.Displays(display).setTextDatum(CC_DATUM);
    M5.Displays(display).setTextPadding(320);
    M5.Displays(display).drawString(version, 160, 20);
    M5.Displays(display).drawRect(20, 28, 280, 10, TFT_DARKGRAY);
    
    for (uint16_t i = 0; i < TIMEOUT_BIN_LOADER * 10; i++)
    {
      gauge = map(i, 0, (TIMEOUT_BIN_LOADER * 10) - 1, 0, 276);
      if(gauge != 0)
      {
        for (uint8_t j = 0; j <= 5; j++)
        {
          M5.Displays(display).drawGradientHLine(22, 30 + j, gauge, TFT_BLACK, TFT_GREEN);
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
        M5.Displays(display).setTextFont(1);
        M5.Displays(display).setTextSize(1);
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
        SD.end(); // If not Bluetooth doesn't work !!!
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
  }

  while (click == true)
  {
    while (btnB != 0)
    {
      getButton();
      vTaskDelay(100);
    }

    M5.Displays(display).setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Displays(display).setTextDatum(CC_DATUM);
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
      M5.Displays(display).fillScreen(TFT_BLACK);
      M5.Displays(display).setFont(&tahoma8pt7b);
      M5.Displays(display).setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Displays(display).setTextDatum(CC_DATUM);
      M5.Displays(display).setTextPadding(320);
      M5.Displays(display).drawString(version, 160, 20);

      M5.Displays(display).drawRect(20, 28, 280, 10, TFT_WHITE);

      SDUCfg.setProgressCb  ( myProgressFunction );
      SDUCfg.setWaitForActionCb( myActionTrigger );

      pos = String(fileName[cursor]).indexOf('/');
      switch (pos)
      {
      case 6:
        M5.Displays(display).drawString(String(fileName[cursor]).substring(pos + 1), 160, 100);
        checkSDUpdater(
          SPIFFS,                                   // filesystem (default=SD)
          String(fileName[cursor]).substring(pos),  // path to binary (default=/menu.bin, empty string=rollback only)
          10000,                                    // wait delay, (default=0, will be forced to 2000 upon ESP.restart() )
          TFCARD_CS_PIN                             // (usually default=4 but your mileage may vary)
        );
        break;
      case 2:
        M5.Displays(display).drawString(String(fileName[cursor]).substring(pos + 1), 160, 100);
        checkSDUpdater(
          SD,                                       // filesystem (default=SD)
          String(fileName[cursor]).substring(pos),  // path to binary (default=/menu.bin, empty string=rollback only)
          1000,                                     // wait delay, (default=0, will be forced to 2000 upon ESP.restart() )
          TFCARD_CS_PIN                             // (usually default=4 but your mileage may vary)
        );
        break;  
      }
      ESP.restart();
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

      Serial.printf("%d %d %d %d\n", start, stop, cursor, limit);

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
    vTaskDelay(100);
  }
  SD.end(); // If not Bluetooth doesn't work !!!
}