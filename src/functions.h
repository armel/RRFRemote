// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
  int right;
  int left;

  if (M5.Lcd.getRotation() == 1) {
    right = 1;
    left = -1;
  } else {
    right = -1;
    left = 1;
  }

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
          roomCurrent += left;
          refresh = 0;
          reset = 0;

          roomCurrent = (roomCurrent < 0) ? 5 : roomCurrent;
          roomCurrent = (roomCurrent > 5) ? 0 : roomCurrent;
          preferences.putUInt("room", roomCurrent);
        }
        else if (btnC && followCurrent == 0)
        {
          screensaver = millis(); // Screensaver update !!!
          roomCurrent += right;
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
          menuCurrent += left;
        }
        else if (btnC)
        {
          menuCurrent += right;
        }
        else if (btnB)
        {
          menuSelected = menuCurrent;
          menuRefresh = 0;
        }

        menuCurrent = (menuCurrent < 0) ? 8 : menuCurrent;
        menuCurrent = (menuCurrent > 8) ? 0 : menuCurrent;
        preferences.putUInt("menu", menuCurrent);
      }
      else
      {
        String option = String(menu[menuCurrent]);

        // Mode menu active, QSY
        if (option == "QSY")
        {
          qsy = dtmf[roomCurrent];
         
          menuMode = 0;
          reset = 0;
          refresh = 0;        
        }
        // Mode menu active, Raptor
        else if (option == "RAPTOR")
        {
          qsy = 200;
          raptorCurrent = (raptorCurrent == 0) ? 1 : 0;
          
          menuMode = 0;
          reset = 0;
          refresh = 0;
        }
        // Mode menu active, Parrot
        else if (option == "PERROQUET")
        {
          qsy = 95;

          menuMode = 0;
          reset = 0;
          refresh = 0;
        }
      // Mode menu active, Reboot
        else if (option == "REBOOT")
        {
          qsy = 1000;

          menuMode = 0;
          reset = 0;
          refresh = 0;
        }
        // Mode menu active, Follow
        else if (option == "FOLLOW")
        {
          followCurrent = (followCurrent == 0) ? 1 : 0;

          menuMode = 0;
          reset = 0;
          refresh = 0;

          preferences.putUInt("follow", followCurrent);
        }
        // Mode menu active, TOT
        else if (option == "TOT")
        {
          totCurrent = (totCurrent == 0) ? 1 : 0;

          menuMode = 0;
          reset = 0;
          refresh = 0;

          preferences.putUInt("tot", totCurrent);
        }
        // Mode menu active, Color
        else if (option == "COULEUR")
        {
          int change = 0;
          if (btnA)
          {
            colorCurrent += left;
            change = 1;
          }
          else if (btnC)
          {
            colorCurrent += right;
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
        else if (option == "LUMINOSITE")
        {
          if (btnA)
          {
            brightnessCurrent += left;
          }
          else if (btnC)
          {
            brightnessCurrent += right;
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
        // Mode menu active, Config
        else if (option == "CONFIG")
        {
          if (btnA)
          {
            configCurrent += left;
          }
          else if (btnC)
          {
            configCurrent += right;
          }
          else if (btnB)
          {
            menuMode = 0;
            reset = 0;
            refresh = 0;
          }

          size_t n = sizeof(config) / sizeof(config[0]);
          n = (n / 6) - 1;

          configCurrent = (configCurrent < 0) ? n : configCurrent;
          configCurrent = (configCurrent > n) ? 0 : configCurrent;
          preferences.putUInt("config", configCurrent);

          //Serial.println(configCurrent);

          if(btnB) {
            WiFi.begin(config[(configCurrent * 6)], config[(configCurrent * 6) + 1]);
            while (WiFi.status() != WL_CONNECTED)
            {
              delay(500);
            }
          }
        }        
        // Mode menu active, Escape
        else if (option == "QUITTER")
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

// Detect rotation
void getAcceleration()
{
  float accX = 0.0F;
  float accY = 0.0F;
  float accZ = 0.0F;

  if(BOARD == GREY || BOARD == CORE2) {
    M5.IMU.getAccelData(&accX,&accY,&accZ);

    if(int(accY * 1000) < -500 && M5.Lcd.getRotation() != 3) {
      reset = 0;
      refresh = 0;
      M5.Lcd.setRotation(3);
    }
    else if(int(accY * 1000) > 500 && M5.Lcd.getRotation() != 1) {
      reset = 0;
      refresh = 0;
      M5.Lcd.setRotation(1);
    }
  }
}

// Compute distance
long computeDistance(float latitudeLink, float longitudeLink) {
    float latitudeUser = atof(config[(configCurrent * 6) + 2]);
    float longitudeUser = atof(config[(configCurrent * 6) + 3]);
    float p, a;
    long r;

    p = PI / 180.0; // Approximation Pi/180
    a = 0.5 - cos((latitudeUser - latitudeLink) * p) / 2.0 + cos(latitudeLink * p) * cos(latitudeUser * p) * (1 - cos((longitudeUser - longitudeLink) * p)) / 2.0;
    r = round(12742 * asin(sqrt(a)));
    
    return r;
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
        http.begin(clientRemote, config[(configCurrent * 6) + 5] + String("?dtmf=") + String(qsy));  // Specify the URL
        http.addHeader("Content-Type", "text/plain");                                     // Specify content-type header
        http.setTimeout(500);                                                             // Set timeout
        int httpCode = http.GET();                                                        // Make the request
        if (httpCode == 200 || qsy == 1000) // Check for the returning code
        {
          qsy = 0;
          refresh = 0;
          menuMode = 0;
          whereisCurrent = dtmf[roomCurrent];
          whereisString = String(room[roomCurrent]);
        }
        http.end(); // Free the resources
      }
 
      http.begin(clientTracker, endpointRRF[roomCurrent]);          // Specify the URL
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
      http.begin(clientWhereis, config[(configCurrent * 6) + 5] + String("?dtmf=0")); // Specify the URL
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