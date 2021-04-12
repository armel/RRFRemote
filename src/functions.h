// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Parse data
String getValue(String data, char separator, uint8_t index)
{
  int8_t strIndex[] = {0, -1};
  int8_t found = 0;
  int8_t maxIndex = data.length() - 1;

  for (uint8_t i = 0; i <= maxIndex && found <= index; i++)
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

// Reset color
void resetColor()
{
  switch(colorCurrent) {
    case 0: TFT_FRONT = TFT_FRONT_ROUGE; TFT_HEADER = TFT_HEADER_ROUGE; TFT_BACK = TFT_BACK_ROUGE; break;
    case 1: TFT_FRONT = TFT_FRONT_ORANGE; TFT_HEADER = TFT_HEADER_ORANGE; TFT_BACK = TFT_BACK_ORANGE; break;
    case 2: TFT_FRONT = TFT_FRONT_VERT; TFT_HEADER = TFT_HEADER_VERT; TFT_BACK = TFT_BACK_VERT; break;
    case 3: TFT_FRONT = TFT_FRONT_TURQUOISE; TFT_HEADER = TFT_HEADER_TURQUOISE; TFT_BACK = TFT_BACK_TURQUOISE; break;
    case 4: TFT_FRONT = TFT_FRONT_BLEU; TFT_HEADER = TFT_HEADER_BLEU; TFT_BACK = TFT_BACK_BLEU; break;
    case 5: TFT_FRONT = TFT_FRONT_ROSE; TFT_HEADER = TFT_HEADER_ROSE; TFT_BACK = TFT_BACK_ROSE; break;
    case 6: TFT_FRONT = TFT_FRONT_VIOLET; TFT_HEADER = TFT_HEADER_VIOLET; TFT_BACK = TFT_BACK_VIOLET; break;
    case 7: TFT_FRONT = TFT_FRONT_GRIS; TFT_HEADER = TFT_HEADER_GRIS; TFT_BACK = TFT_BACK_GRIS; break;
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
  M5.Lcd.drawFastHLine(  0,   0, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(  0,  44, 320, TFT_WHITE);

  // Grey zone
  M5.Lcd.drawFastHLine(  0, 100, 320, TFT_WHITE);
  M5.Lcd.fillRect(0, 101, 320, 239, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

  // Elsewhere
  M5.Lcd.fillRoundRect(0, 168, 155, 71, 4, TFT_WHITE);
  M5.Lcd.fillRoundRect(1, 169, 26, 69, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));

  M5.Lcd.drawFastVLine(24, 169, 69, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
  M5.Lcd.drawFastVLine(25, 169, 69, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
  M5.Lcd.drawFastVLine(26, 169, 69, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));

  M5.Lcd.drawFastVLine(27, 169, 69, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastVLine(100, 169, 69, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

  M5.Lcd.drawFastHLine(2, 182, 152, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastHLine(2, 196, 152, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastHLine(2, 210, 152, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastHLine(2, 224, 152, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

  // Log
  M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
}

// Build scroll
void buildScroll()
{
  int16_t h = 20;
  int16_t w = M5.Lcd.width();

  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    img.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  img.setTextSize(1);          // Font size scaling is x1
  img.setTextFont(2);          // Font 2 selected
  //img.setFreeFont(&tahoma6pt7b);
  
  img.setTextColor(TFT_WHITE); // White text, no background colour
  img.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  img.setCursor(pos, 2); // Print text at pos
  img.print(message);

  img.setCursor(pos - w, 2); // Print text at pos - sprite width
  img.print(message);
}

// Scroll
void scroll(uint8_t pause)
{
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
      menuRefresh = 0;
      M5.Lcd.setRotation(3);
    }
    else if(int(accY * 1000) > 500 && M5.Lcd.getRotation() != 1) {
      reset = 0;
      refresh = 0;
      menuRefresh = 0;
      M5.Lcd.setRotation(1);
    }
  }
}

// Compute distance
int computeDistance(float latitudeLink, float longitudeLink) {
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
        http.begin(clientRemote, config[(configCurrent * 6) + 5] + String("?cmd=") + String(qsy));  // Specify the URL
        http.addHeader("Content-Type", "text/plain");                                     // Specify content-type header
        http.setTimeout(500);                                                             // Set timeout
        int httpCode = http.GET();                                                        // Make the request
        if (httpCode == 200 || qsy == 2000 || qsy == 2001 || qsy == 2002 || qsy == 2003)  // Check for the returning code
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
      http.setTimeout(500);                          // Set Time Out
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
    if ((WiFi.status() == WL_CONNECTED) && ((String)config[(configCurrent * 6) + 5] != "")) // Check the current connection status
    {
      http.begin(clientWhereis, config[(configCurrent * 6) + 5] + String("?cmd=0")); // Specify the URL
      http.addHeader("Content-Type", "text/plain");                     // Specify content-type header
      http.setTimeout(500);                                            // Set Time Out
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

// Get data from ISS
void iss(void *pvParameters)
{
  HTTPClient http;
  unsigned int limit = 1 * 30 * 1000; // Retry 30 secondes
    
  for (;;)
  {
    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {
      clientISS.setInsecure();
      http.begin(clientISS, endpointISS);     // Specify the URL
      http.addHeader("Content-Type", "text/plain");   // Specify content-type header
      http.setTimeout(500);                           // Set Time Out
      int httpCode = http.GET();                      // Make the request
      if (httpCode == 200)                            // Check for the returning code
      {
        issData = http.getString(); // Get data
      }
      http.end(); // Free the resources
      vTaskDelay(pdMS_TO_TICKS(limit));
    }
  }
}

// Manage buttons
void button(void *pvParameters)
{
  int8_t right;
  int8_t left;
  int8_t change;

  for (;;)
  {
    getButton();

    /*
    if (btnA || btnB || btnC)
    {
      Serial.println(".");
      Serial.print(btnA);
      Serial.print("-");
      Serial.print(btnB);
      Serial.print("-");
      Serial.println(btnC);
    }
    else
    {
      Serial.print(".");
    }
    */

    if (M5.Lcd.getRotation() == 1) {
      right = 1;
      left = -1;
    } else {
      right = -1;
      left = 1;
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
    { 
      if ((btnA || btnB || btnC))
      {
        screensaver = millis(); // Screensaver update !!!

        // Mode menu inactive
        if (menuMode == 0)
        {
          if(followCurrent == 0)
          {
            change = roomCurrent;

            if (btnA)
            {
              change += left;

              change = (change < 0) ? 5 : change;
              change = (change > 5) ? 0 : change;
              roomCurrent = change;
              preferences.putUInt("room", roomCurrent);
            }
            else if (btnC)
            {
              change += right;
          
              change = (change < 0) ? 5 : change;
              change = (change > 5) ? 0 : change;
              roomCurrent = change;
              preferences.putUInt("room", roomCurrent);
            }
            action = 2;
          }
          if (btnB)
          {
            //Serial.println("Click A");
            vTaskDelay(pdMS_TO_TICKS(250));
            menuMode = 1;
            btnB = 0;
            action = 1;
          }
        }
        // Mode menu active
        else if (menuMode == 1 && menuSelected == -1)
        {
          // If no selection
          if (menuSelected == -1)
          {
            change = menuCurrent;

            if (btnA)
            {
              change += left;
            }
            else if (btnC)
            {
              change += right;
            }
            else if (btnB)
            {
              //Serial.println("Click B");
              vTaskDelay(pdMS_TO_TICKS(250));
              menuSelected = menuCurrent;
              btnB = 0;
            }

            int n = menuSize/sizeof(char *);
            n -= 1;

            change = (change < 0) ? n : change;
            change = (change > n) ? 0 : change;
            menuCurrent = change;
            preferences.putUInt("menu", menuCurrent);
          }
        }
      }

      if (menuMode == 1 && menuSelected != -1)
      {
        String option = String(menu[menuCurrent]);
        // Mode menu active, QSY
        if (option == "QSY")
        {
          qsy = dtmf[roomCurrent];
          menuMode = 2;
        }
        // Mode menu active, Raptor
        else if (option == "RAPTOR")
        {
          change = raptorCurrent;
          change = (change == 0) ? 1 : 0;
          raptorCurrent = change;
          qsy = 200;
          vTaskDelay(pdMS_TO_TICKS(1000));
          menuMode = 2;
        }
        // Mode menu active, Parrot
        else if (option == "PERROQUET")
        {
          qsy = 95;
          menuMode = 2;
        }
        // Mode menu active, Follow
        else if (option == "FOLLOW")
        {
          change = followCurrent;
          change = (change == 0) ? 1 : 0;
          followCurrent = change;
          preferences.putUInt("follow", followCurrent);
          vTaskDelay(pdMS_TO_TICKS(1000));
          menuMode = 2;
        }
        // Mode menu active, TOT
        else if (option == "TOT")
        {
          change = totCurrent;
          change = (change == 0) ? 1 : 0;
          totCurrent = change;
          preferences.putUInt("tot", totCurrent);
          vTaskDelay(pdMS_TO_TICKS(1000));
          menuMode = 2;
        }
        // Mode menu active, Color
        else if (option == "COULEUR")
        {
          change = colorCurrent;

          if (btnA)
          {
            change += left;
          }
          else if (btnC)
          {
            change += right;
          }
          else if (btnB)
          {
            menuMode = 2;
          }

          size_t n = sizeof(color) / sizeof(color[0]);
          n -= 1;

          change = (change < 0) ? n : change;
          change = (change > n) ? 0 : change;

          if(change != colorCurrent) 
          {
            colorCurrent = change;
            action = 3;
            preferences.putUInt("color", colorCurrent);
          }
        }
        // Mode menu active, Brightness
        else if (option == "LUMINOSITE")
        {
          change = brightnessCurrent;
          if (btnA)
          {
            change += left;
          }
          else if (btnC)
          {
            change += right;
          }
          else if (btnB)
          {
            menuMode = 2;
          }

          change = (change < 10) ? 10 : change;
          change = (change > 128) ? 128 : change;

          if(change != colorCurrent) 
          {
            brightnessCurrent = change;
            preferences.putUInt("brightness", brightnessCurrent);
            M5.Lcd.setBrightness(brightnessCurrent);
          }
        }
        // Mode menu active, Special
        else if (option == "SYSOP")
        {
          change = sysopCurrent;

          if (btnA)
          {
            change += left;
          }
          else if (btnC)
          {
            change += right;
          }
          else if (btnB)
          {
            qsy = 2000;
            qsy += sysopCurrent;
            menuMode = 2;
          }

          size_t n = sizeof(sysop) / sizeof(sysop[0]);
          n -= 1;

          change = (change < 0) ? n : change;
          change = (change > n) ? 0 : change;

          if(change != sysopCurrent) 
          {
            sysopCurrent = change;
            preferences.putUInt("sysop", sysopCurrent);
          }
        }
        // Mode menu active, Config
        else if (option == "CONFIG")
        {
          change = configCurrent;

          if (btnA)
          {
            change += left;
          }
          else if (btnC)
          {
            change += right;
          }
          else if (btnB)
          {
            WiFi.begin(config[(configCurrent * 6)], config[(configCurrent * 6) + 1]);
            while (WiFi.status() != WL_CONNECTED)
            {
              vTaskDelay(pdMS_TO_TICKS(250));
            }
            menuMode = 2;
          }

          size_t n = sizeof(config) / sizeof(config[0]);
          n = (n / 6) - 1;

          change = (change < 0) ? n : change;
          change = (change > n) ? 0 : change;

          if(change != configCurrent) 
          {
            configCurrent = change;
            preferences.putUInt("config", configCurrent);
          }
        }        
        // Mode menu active, Escape
        else if (option == "QUITTER")
        {
          menuMode = 2;
        }      
      }

      // Escape game...
      if (menuMode == 2)
      {
        action = 4;
      }

      // Manage temporisation
      if(btnA || btnC)
      {
        if(menuMode == 0)
        {
          //Serial.print("-");
          vTaskDelay(pdMS_TO_TICKS(200));
        }
        else if(menuMode == 1)
        {
          //Serial.print("+");
          vTaskDelay(pdMS_TO_TICKS(200));
        }
      }
      else
      {
        //Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(150));
      }
    }
  }
}