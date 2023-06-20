// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Get data from RRFTracker and manage RRFRemote client
void rrfdata(void *pvParameters) {
  HTTPClient http;
  uint32_t timer = 0, wait = 0, limit = 500;  // Retry all 500ms
  uint8_t counter = 10;
  uint16_t httpCode;

  for (;;) {
    // Serial.println("rrfdata");
    timer = millis();
    if ((WiFi.status() == WL_CONNECTED))  // Check the current connection status
    {
      // Manage QSY
      while (qsy > 0) {
        // Serial.println("RRFRemote QSY");
        http.begin(clientRemote, config[(configCurrent * 6) + 5] + String("?cmd=") + String(qsy));  // Specify the URL
        http.addHeader("User-Agent", "M5Stack");                                                    // Specify header
        http.addHeader("Connection", "keep-alive");                                                 // Specify header
        http.setTimeout(500);                                                                       // Set timeout
        httpCode = http.GET();                                                                      // Make the request
        if (httpCode == 200 || qsy == 2000 || qsy == 2001 || qsy == 2002 ||
            qsy == 2003)  // Check for the returning code
        {
          qsy            = 0;
          refresh        = 0;
          menuMode       = 0;
          whereisCurrent = dtmf[roomCurrent];
          whereisString  = String(room[roomCurrent]);
        }
      }

      // RRFTracker GET
      // Serial.println("RRFTracker GET");
      http.begin(clientTracker, endpointRRF[roomCurrent]);  // Specify the URL
      http.addHeader("User-Agent", "M5Stack");              // Specify header
      http.addHeader("Connection", "keep-alive");           // Specify header
      http.setTimeout(500);                                 // Set Time Out
      httpCode = http.GET();                                // Make the request
      if (httpCode == 200)                                  // Check for the returning code
      {
        jsonDataNew = http.getString();                     // Get data
      }

      // RRFRemote GET
      if (counter == 10) {
        // Serial.println("RRFRemote GET");
        http.begin(clientWhereis, config[(configCurrent * 6) + 5] + String("?cmd=0"));  // Specify the URL
        http.addHeader("User-Agent", "M5Stack");                                        // Specify header
        http.addHeader("Connection", "keep-alive");                                     // Specify header
        http.setTimeout(750);                                                           // Set Time Out
        httpCode = http.GET();                                                          // Make the request
        if (httpCode == 200)                                                            // Check for the returning code
        {
          ping        = 0;
          whereisData = http.getString();  // Get data

          whereisCurrent = whereisData.substring(0, whereisData.indexOf(", ")).toInt();

          whereisData   = whereisData.substring(whereisData.indexOf(", ") + 2);
          whereisString = whereisData.substring(0, whereisData.indexOf(", "));

          whereisData = whereisData.substring(whereisData.indexOf(", ") + 2);
          if (whereisData.substring(0, whereisData.indexOf(", ")) == "OFF") {
            raptorCurrent = 0;
          } else {
            raptorCurrent = 1;
          }
          whereisData = whereisData.substring(whereisData.indexOf(", ") + 2);
          tempCurrent = (whereisData.toInt()) / 1000;
          // Serial.println(tempCurrent);
        } else {
          ping = 1;
        }
      }
      http.end();  // Free the resources
      counter = (counter++ < 10) ? counter : 1;
    }

    wait = millis() - timer;
    if (wait < limit) {
      vTaskDelay(pdMS_TO_TICKS(limit - wait));
    }

    // Serial.print("rrfdata ");
    // Serial.println(millis() - timer);
  }
}

// Get data from ISS and HamSQL
void hamdata(void *pvParameters) {
  HTTPClient http;
  uint32_t timer = 0, wait = 0, limit = 1 * 10 * 1000;  // Retry 10 secondes
  uint16_t counter = 360;
  uint16_t httpCode;

  for (;;) {
    // Serial.println("hamdata");
    timer = millis();
    if (WiFi.status() == WL_CONNECTED)  // Check the current connection status
    {
      // ISS Get
      if (issCurrent == 1) {
        // Serial.println("ISS GET");
        http.begin(clientISS, endpointISS);          // Specify the URL
        http.addHeader("User-Agent", "M5Stack");     // Specify header
        http.addHeader("Connection", "keep-alive");  // Specify header
        http.setTimeout(1000);                       // Set Time Out
        httpCode = http.GET();                       // Make the request
        if (httpCode == 200)                         // Check for the returning code
        {
          issData = http.getString();                // Get data
        }
      }

      // HamSQL GET (every hour)
      if (counter == 360) {
        // Serial.println("HamSQL GET");
        http.begin(clientHamSQL, endpointHamQSL);    // Specify the URL
        http.addHeader("User-Agent", "M5Stack");     // Specify header
        http.addHeader("Connection", "keep-alive");  // Specify header
        http.setTimeout(1000);                       // Set Time Out
        httpCode = http.GET();                       // Make the request
        if (httpCode == 200)                         // Check for the returning code
        {
          xmlData = http.getString();                // Get data
        }
      }

      http.end();  // Free the resources
      counter = (counter++ < 360) ? counter : 1;
    }

    wait = millis() - timer;
    if (wait < limit) {
      vTaskDelay(pdMS_TO_TICKS(limit - wait));
    }
  }
}

// Get Button
void button(void *pvParameters) {
  int8_t right;
  int8_t left;
  int16_t change = 0;

  static uint32_t timer = 0;

  static int8_t settingsChoice  = 0;
  static boolean settingsSelect = false;

  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;

  for (;;) {
    // Position
    x = 44 + offsetX;
    y = 4 + offsetY;
    w = M5.Displays(display).width() - (x * 2);
    h = 185;

    // Get screenshot
    // getScreenshot();

    // Get button
    getButton(modeCurrent, menuMode);

    if (btnA || btnB || btnC) {
      screensaver = millis();  // Screensaver update !!!
      timer       = millis();
    }

    if (M5.Displays(display).getRotation() == 1) {
      right = 1;
      left  = -1;
    } else {
      right = -1;
      left  = 1;
    }

    if (btnD == 1) {
      screensaver = millis();  // Screensaver update !!!
      timer       = millis();

      if (modeCurrent == 2) {
        modeNew = modeOld;
        btnA = btnB = btnC = 0;
        action             = 5;
      } else {
        modeOld = modeCurrent;
        modeNew = 2;
        btnA = btnB = btnC = 0;
        action             = 5;
      }
    }

    // Gestion CORE2 QSY rapide
    if (modeNew == 2) {
      // Sortie automatique si aucune action
      if ((millis() - timer) > TIMEOUT_MENU) {
        modeNew        = modeOld;
        action         = 5;
        settingsSelect = false;
        settingsMode   = false;
        vTaskDelay(pdMS_TO_TICKS(150));
      }

      // Manage DTMF on CORE2
      if (btnDTMF1 == 1) {
        // Serial.println("QSY RRF");
        qsy     = dtmf[0];
        modeNew = modeOld;
        action  = 5;
      } else if (btnDTMF2 == 1) {
        // Serial.println("QSY TEC");
        qsy     = dtmf[1];
        modeNew = modeOld;
        action  = 5;
      } else if (btnDTMF3 == 1) {
        // Serial.println("QSY BAV");
        qsy     = dtmf[2];
        modeNew = modeOld;
        action  = 5;
      } else if (btnDTMF4 == 1) {
        // Serial.println("QSY LOC");
        qsy     = dtmf[3];
        modeNew = modeOld;
        action  = 5;
      } else if (btnDTMF5 == 1) {
        // Serial.println("QSY INT");
        qsy     = dtmf[4];
        modeNew = modeOld;
        action  = 5;
      } else if (btnDTMF6 == 1) {
        // Serial.println("QSY EXP");
        qsy     = dtmf[5];
        modeNew = modeOld;
        action  = 5;
      } else if (btnDTMF7 == 1) {
        // Serial.println("QSY FON");
        qsy     = dtmf[6];
        modeNew = modeOld;
        action  = 5;
      } else if (btnDTMF8 == 1) {
        if (whereisString == "PERROQUET") {
          qsy = dtmf[roomCurrent];
        } else {
          qsy = 95;
        }
        modeNew = modeOld;
        action  = 5;
      } else if (btnDTMF9 == 1) {
        // Serial.println("RAPTOR");
        qsy     = 200;
        modeNew = modeOld;
        action  = 5;
      }
    }

    if (settingsMode == false) {
      if (followCurrent == 0) {
        change = roomCurrent;

        int n = sizeof(room) / sizeof(room[0]);
        n -= 1;

        if (btnA) {
          change += left;

          change      = (change < 0) ? n : change;
          change      = (change > n) ? 0 : change;
          roomCurrent = change;
          preferences.putUInt("room", roomCurrent);
          action = 2;
        } else if (btnC) {
          change += right;

          change      = (change < 0) ? n : change;
          change      = (change > n) ? 0 : change;
          roomCurrent = change;
          preferences.putUInt("room", roomCurrent);
          action = 2;
        }
      }
      // Enter settings
      if (btnB) {
        settingsMode = true;
        while (settingLock == true) {
          vTaskDelay(10);
        }
        viewMenu(x, y, w, h);
        viewOption(settingsChoice, settingsSelect, x, y, w);
        vTaskDelay(300);
      }
      vTaskDelay(100);
    }
    // Select settings
    else if (settingsMode == true) {
      // Sortie automatique si aucune action
      if ((millis() - timer) > TIMEOUT_MENU) {
        reset   = 0;
        refresh = 0;
        clear();
        settingsSelect = false;
        settingsMode   = false;
        vTaskDelay(pdMS_TO_TICKS(150));
      }

      if (settingsSelect == false) {
        if (btnA || btnC) {
          if (btnA) {
            settingsChoice--;
          } else if (btnC) {
            settingsChoice++;
          }

          /*
          uint8_t stop = menuSize;
          stop--;
          */

          // If settings from SD
          /*
          if(iniValid == true)
          {
            stop--;
          }
          */

          size_t stop = settingsLength - 1;

          settingsChoice = (settingsChoice < 0) ? stop : settingsChoice;
          settingsChoice = (settingsChoice > stop) ? 0 : settingsChoice;

          viewOption(settingsChoice, settingsSelect, x, y, w);
        } else if (btnB) {
          Serial.printf(">>>>>>> ecran %d\n", M5.getDisplayCount());

          settingsSelect = true;
          viewOption(settingsChoice, settingsSelect, x, y, w);

          String settingsString = String(settings[settingsChoice]);

          if (settingsString == "Config")
            change = configCurrent;
          else if (settingsString == "Sysop")
            change = sysopCurrent;
          else if (settingsString == "QSY")
            change = roomCurrent;
          else if (settingsString == "Follow")
            change = followCurrent;
          else if (settingsString == "Raptor")
            change = raptorCurrent;
          else if (settingsString == "TOT")
            change = totCurrent;
          else if (settingsString == "ISS")
            change = issCurrent;
          else if (settingsString == "Affichage")
            change = modeCurrent;
          else if (settingsString == "Themes")
            change = colorCurrent;
          else if (settingsString == "Luminosite")
            change = brightnessCurrent;
          else if (settingsString == "Beep")
            change = beepCurrent;
          else if (settingsString == "Screensaver")
            change = screensaverCurrent;
          else if (settingsString == "Eteindre")
            shutdown();
          else if (settingsString == "Perroquet") {
            if (whereisString == "PERROQUET") {
              change = 1;
            } else {
              change = 0;
            }
          } else if (settingsString == "HDMI") {
            if (M5.getDisplayCount() == 1) {
              reset   = 0;
              refresh = 0;
              clear();
              settingsSelect = false;
              settingsMode   = false;
              vTaskDelay(pdMS_TO_TICKS(150));
            } else {
              change = hdmiCurrent;
            }
          } else if (settingsString == "Exit") {
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
        }
        vTaskDelay(pdMS_TO_TICKS(150));
      }
      // Manage settings
      else if (settingsSelect == true) {
        String settingsString = String(settings[settingsChoice]);

        M5.Displays(display).setTextDatum(CC_DATUM);
        M5.Displays(display).setFont(&YELLOWCRE8pt7b);
        M5.Displays(display).setTextPadding(w - 2);
        M5.Displays(display).setTextColor(TFT_MENU_SELECT, TFT_BACK);

        // Config
        if (settingsString == "Config") {
          String tmp = (String)config[(change * 6) + 4];
          tmp.toUpperCase();

          M5.Displays(display).drawString(tmp, 160 + offsetX, h - 6 + offsetY);
          size_t stop = sizeof(config) / sizeof(config[0]);
          stop        = (stop / 6) - 1;

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? stop : change;
            change = (change > stop) ? 0 : change;
          } else if (btnB == 1) {
            if (configCurrent != change) {
              configCurrent = change;
              preferences.putUInt("config", configCurrent);
              WiFi.begin(config[(configCurrent * 6)], config[(configCurrent * 6) + 1]);
              while (WiFi.status() != WL_CONNECTED) {
                vTaskDelay(pdMS_TO_TICKS(250));
              }

              // Create menu
              settings = cleanSettings();

              /*
              if ((String)config[(configCurrent * 6) + 5] != "") {
                menuSize     = sizeof(menuSpotnikOn);
                settingsMenu = (char **)malloc(menuSize);
                memcpy(settingsMenu, menuSpotnikOn, menuSize);
                menuSize = sizeof(menuSpotnikOn) / sizeof(menuSpotnikOn[0]);
              } else {
                menuSize     = sizeof(menuSpotnikOff);
                settingsMenu = (char **)malloc(menuSize);
                memcpy(settingsMenu, menuSpotnikOff, menuSize);
                menuSize = sizeof(menuSpotnikOff) / sizeof(menuSpotnikOff[0]);

                followCurrent = (followCurrent == 1) ? 0 : 0;
                preferences.putUInt("follow", followCurrent);
              }
              */
            }
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Sysop
        else if (settingsString == "Sysop") {
          M5.Displays(display).drawString(String(choiceSysop[change]), 160 + offsetX, h - 6 + offsetY);
          size_t stop = sizeof(choiceSysop) / sizeof(choiceSysop[0]);
          stop--;

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? stop : change;
            change = (change > stop) ? 0 : change;
          } else if (btnB == 1) {
            if (change != sysopCurrent) {
              sysopCurrent = change;
              preferences.putUInt("sysop", sysopCurrent);
            }
            qsy = 2000;
            qsy += sysopCurrent;

            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Perroquet
        if (settingsString == "Perroquet") {
          M5.Displays(display).drawString(String(choicePerroquet[change]), 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? 1 : change;
            change = (change > 1) ? 0 : change;
          } else if (btnB == 1) {
            if (whereisString == "PERROQUET" && change == 0) {
              qsy = dtmf[roomCurrent];
            } else if (whereisString != "PERROQUET" && change == 1) {
              qsy = 95;
            }
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // QSY
        if (settingsString == "QSY") {
          M5.Displays(display).drawString(String(room[change]), 160 + offsetX, h - 6 + offsetY);
          size_t stop = sizeof(room) / sizeof(room[0]);
          stop--;

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? stop : change;
            change = (change > stop) ? 0 : change;
          } else if (btnB == 1) {
            roomCurrent = change;
            preferences.putUInt("room", roomCurrent);
            qsy     = dtmf[roomCurrent];
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Follow
        if (settingsString == "Follow") {
          M5.Displays(display).drawString(String(choiceFollow[change]), 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? 1 : change;
            change = (change > 1) ? 0 : change;
          } else if (btnB == 1) {
            if (followCurrent != change) {
              followCurrent = change;
              preferences.putUInt("follow", followCurrent);
            }
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Raptor
        if (settingsString == "Raptor") {
          M5.Displays(display).drawString(String(choiceRaptor[change]), 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? 1 : change;
            change = (change > 1) ? 0 : change;
          } else if (btnB == 1) {
            if (raptorCurrent != change) {
              raptorCurrent = change;
              preferences.putUInt("raptor", raptorCurrent);
              qsy = 200;
            }
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // TOT
        if (settingsString == "TOT") {
          M5.Displays(display).drawString(String(choiceTOT[change]), 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? 1 : change;
            change = (change > 1) ? 0 : change;
          } else if (btnB == 1) {
            if (totCurrent != change) {
              totCurrent = change;
              preferences.putUInt("tot", totCurrent);
            }
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // ISS
        if (settingsString == "ISS") {
          M5.Displays(display).drawString(String(choiceISS[change]), 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? 1 : change;
            change = (change > 1) ? 0 : change;
          } else if (btnB == 1) {
            if (issCurrent != change) {
              issCurrent = change;
              preferences.putUInt("iss", issCurrent);
            }
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Themes
        if (settingsString == "Themes") {
          M5.Displays(display).drawString(String(choiceThemes[change]), 160 + offsetX, h - 6 + offsetY);
          size_t stop = sizeof(choiceThemes) / sizeof(choiceThemes[0]);
          stop--;

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change       = (change < 0) ? stop : change;
            change       = (change > stop) ? 0 : change;
            colorCurrent = change;
          } else if (btnB == 1) {
            preferences.putUInt("color", colorCurrent);
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // HDMI
        if (settingsString == "HDMI") {
          M5.Displays(display).drawString(String(choiceHDMI[change]), 160 + offsetX, h - 6 + offsetY);
          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 0) ? 1 : change;
            change = (change > 1) ? 0 : change;
          } else if (btnB == 1) {
            if (hdmiCurrent != change) {
              M5.Displays(display).fillScreen(TFT_BLACK);
              hdmiCurrent = change;
              preferences.putUInt("hdmi", hdmiCurrent);
              display = hdmiCurrent;
              M5.setPrimaryDisplay(display);
              offsetX = (M5.Displays(display).width() - 320) / 2;
              offsetY = (M5.Displays(display).height() - 240) / 2;
            }
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Brightness
        if (settingsString == "Luminosite") {
          M5.Displays(display).drawString(String(change) + "%", 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 1) ? 100 : change;
            change = (change > 100) ? 1 : change;

            brightnessCurrent = change;
            M5.Lcd.setBrightness(map(brightnessCurrent, 1, 100, 1, 254));
          } else if (btnB) {
            preferences.putUInt("brightness", brightnessCurrent);
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Beep
        else if (settingsString == "Beep") {
          M5.Displays(display).drawString(String(change) + "%", 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change      = (change < 0) ? 100 : change;
            change      = (change > 100) ? 0 : change;
            beepCurrent = change;
          } else if (btnB == 1) {
            preferences.putUInt("beep", beepCurrent);
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Screensaver
        if (settingsString == "Screensaver") {
          M5.Displays(display).drawString(String(change) + " MIN", 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change = (change < 1) ? 60 : change;
            change = (change > 60) ? 1 : change;
          } else if (btnB == 1) {
            if (screensaverCurrent != change) {
              screensaverCurrent = change;
              preferences.putUInt("screensaver", change);
            }
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Mode
        else if (settingsString == "Affichage") {
          M5.Displays(display).drawString(String(choiceAffichage[change]), 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA) {
              change += left;
            } else if (btnC) {
              change += right;
            }
            change      = (change < 0) ? 1 : change;
            change      = (change > 1) ? 0 : change;
            modeCurrent = change;
          } else if (btnB == 1) {
            preferences.putUInt("mode", change);
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // IP Address
        else if (settingsString == "Addresse IP") {
          M5.Displays(display).drawString(String(WiFi.localIP().toString().c_str()), 160 + offsetX, h - 6 + offsetY);

          if (btnB == 1) {
            reset   = 0;
            refresh = 0;
            clear();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}