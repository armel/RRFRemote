// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Get data from RRFTracker and manage RRFRemote client
void rrfdata(void *pvParameters)
{
  HTTPClient http;
  uint32_t timer = 0, wait = 0, limit = 750; // Retry all 750ms
  uint8_t counter = 10;
  uint16_t httpCode;

  for (;;)
  {
    timer = millis();
    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {
      // Manage QSY
      while (qsy > 0)
      {
        //Serial.println("RRFRemote QSY");
        http.begin(clientRemote, config[(configCurrent * 6) + 5] + String("?cmd=") + String(qsy));  // Specify the URL
        http.addHeader("User-Agent","M5Stack");                                                     // Specify header
        http.addHeader("Connection","keep-alive");                                                  // Specify header
        http.setTimeout(500);                                                                       // Set timeout
        httpCode = http.GET();                                                                      // Make the request
        if (httpCode == 200 || qsy == 2000 || qsy == 2001 || qsy == 2002 || qsy == 2003)            // Check for the returning code
        {
          qsy = 0;
          refresh = 0;
          menuMode = 0;
          whereisCurrent = dtmf[roomCurrent];
          whereisString = String(room[roomCurrent]);
        }
      }
 
      // RRFTracker GET
      //Serial.println("RRFTracker GET");
      http.begin(clientTracker, endpointRRF[roomCurrent]);  // Specify the URL
      http.addHeader("User-Agent","M5Stack");               // Specify header
      http.addHeader("Connection","keep-alive");            // Specify header
      http.setTimeout(750);                                 // Set Time Out
      httpCode = http.GET();                                // Make the request
      if (httpCode == 200)                                  // Check for the returning code
      {
        jsonDataNew = http.getString(); // Get data
      } 

      // RRFTracker GET
      if(counter == 10)
      {
        //Serial.println("RRFRemote GET");
        http.begin(clientWhereis, config[(configCurrent * 6) + 5] + String("?cmd=0"));  // Specify the URL
        http.addHeader("User-Agent","M5Stack");                                         // Specify header
        http.addHeader("Connection","keep-alive");                                      // Specify header
        http.setTimeout(500);                                                           // Set Time Out
        httpCode = http.GET();                                                          // Make the request
        if (httpCode == 200)                                                            // Check for the returning code
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
      }
      http.end(); // Free the resources
      counter = (counter++ < 10) ? counter : 1;
    }

    wait = millis() - timer;
    if (wait < limit)
    {
      vTaskDelay(pdMS_TO_TICKS(limit - wait));
    }
  }
}

// Get data from ISS and HamSQL
void hamdata(void *pvParameters)
{
  HTTPClient http;
  uint32_t timer = 0, wait = 0, limit = 1 * 10 * 1000; // Retry 10 secondes
  uint16_t counter = 360;
  uint16_t httpCode;

  for (;;)
  {
    timer = millis();
    if (WiFi.status() == WL_CONNECTED) // Check the current connection status
    {
      // ISS Get
      if(issCurrent == 1) {
        //Serial.println("ISS GET");
        http.begin(clientISS, endpointISS);               // Specify the URL
        http.addHeader("User-Agent","M5Stack");           // Specify header
        http.addHeader("Connection","keep-alive");        // Specify header
        http.setTimeout(1000);                            // Set Time Out
        httpCode = http.GET();                            // Make the request
        if (httpCode == 200)                              // Check for the returning code
        {
          issData = http.getString(); // Get data
        }
      }
      
      // HamSQL GET (every hour)
      if(counter == 360)
      {
        //Serial.println("HamSQL GET");
        http.begin(clientHamSQL, endpointHamQSL);       // Specify the URL
        http.addHeader("User-Agent","M5Stack");         // Specify header
        http.addHeader("Connection","keep-alive");      // Specify header
        http.setTimeout(1000);                          // Set Time Out
        httpCode = http.GET();                          // Make the request
        if (httpCode == 200)                            // Check for the returning code
        {
          xmlData = http.getString(); // Get data
        }
      }

      http.end(); // Free the resources
      counter = (counter++ < 360) ? counter : 1;
    }

    wait = millis() - timer;
    if (wait < limit)
    {
      vTaskDelay(pdMS_TO_TICKS(limit - wait));
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

    if(buttonLeftPressed) {
      btnA = true;
      buttonLeftPressed = false;
    }
    if(buttonCenterPressed) {
      btnB = true;
      buttonCenterPressed = false;
    }
    if(buttonRightPressed) {
      btnC = true;
      buttonRightPressed = false;
    }

    if(htmlGetRefresh == 1) {
      htmlGetRefresh = 2;
    }

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

    if ((btnA || btnB || btnC))
    {
      screensaver = millis(); // Screensaver update !!!
      if(screensaverMode == 1)
      {
        //Serial.println("Wake up");
        btnA = 0;
        btnB = 0;
        btnC = 0;
      }

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
      // Mode menu active, ISS
      else if (option == "ISS")
      {
        change = issCurrent;
        change = (change == 0) ? 1 : 0;
        issCurrent = change;
        preferences.putUInt("iss", issCurrent);
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
      else if (option == "MODE")
      {
        change = modeCurrent;
        change = (change == 0) ? 1 : 0;
        modeCurrent = change;
        preferences.putUInt("mode", modeCurrent);
        //vTaskDelay(pdMS_TO_TICKS(1000));
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

        if(change != brightnessCurrent) 
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

          // Create menu
          if ((String)config[(configCurrent * 6) + 5] != "")
          {
            menuSize = sizeof(menuSpotnikOn);
            menu = (char **)malloc(menuSize);
            memcpy(menu, menuSpotnikOn, menuSize);
          }
          else
          {
            menuSize = sizeof(menuSpotnikOff);
            menu = (char **)malloc(menuSize);
            memcpy(menu, menuSpotnikOff, menuSize);
            
            followCurrent = (followCurrent == 1) ? 0 : 0;
            preferences.putUInt("follow", followCurrent);
          }
          menuMode = 2;
        }

        size_t n = sizeof(config) / sizeof(config[0]);
        n = (n / 6) - 1;

        change = (change < 0) ? n : change;
        change = (change > n) ? 0 : change;

        if(change != configCurrent) 
        {
          whereisString = "-";
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