// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

uint8_t btnA, btnB, btnC, btnD, btnE = 0;
uint8_t btnDTMF1, btnDTMF2, btnDTMF3 = 0;
uint8_t btnDTMF4, btnDTMF5, btnDTMF6 = 0;
uint8_t btnDTMF7, btnDTMF8, btnDTMF9 = 0;

struct Button
{
  String name;    // name
  int x;          // x
  int y;          // y
  int w;          // width
  int h;          // height
  boolean active; // active, if true, check this button, else bypass
  boolean read;   // read, if true, button is push, else false
};

Button myBtn[] = {
    {"myBtnA", 0, 200, 100, 80, true, false},
    {"myBtnB", 110, 200, 100, 80, true, false},
    {"myBtnC", 220, 200, 100, 80, true, false},
    {"myBtnD", 0, 0, 320, 80, true, false},

    {"myBtnDTMF1", 10 + (110 * 0), 109 + (48 * 0), 80, 30, false, false},
    {"myBtnDTMF2", 10 + (110 * 1), 109 + (48 * 0), 80, 30, false, false},
    {"myBtnDTMF3", 10 + (110 * 2), 109 + (48 * 0), 80, 30, false, false},
    {"myBtnDTMF4", 10 + (110 * 0), 109 + (48 * 1), 80, 30, false, false},
    {"myBtnDTMF5", 10 + (110 * 1), 109 + (48 * 1), 80, 30, false, false},
    {"myBtnDTMF6", 10 + (110 * 2), 109 + (48 * 1), 80, 30, false, false},
    {"myBtnDTMF7", 10 + (110 * 0), 109 + (48 * 2), 80, 30, false, false},
    {"myBtnDTMF8", 10 + (110 * 1), 109 + (48 * 2), 80, 30, false, false},
    {"myBtnDTMF9", 10 + (110 * 2), 109 + (48 * 2), 80, 30, false, false},
};

uint8_t limit = sizeof myBtn / sizeof myBtn[0];

// Shutdown
void shutdown()
{
  M5.Power.powerOff();
}

// Get Battery level
uint8_t getBatteryLevel(bool type)
{
  return M5.Power.getBatteryLevel();
}

// Control if charging
bool isCharging()
{
  return M5.Power.isCharging();
}

// Get button binloader startup
void getButtonBinLoaderStartup()
{
  if (M5.getBoard() == m5::board_t::board_M5Stack)
  {
    M5.update();

    btnA = M5.BtnA.wasReleased();
    btnB = M5.BtnB.wasReleased();
    btnC = M5.BtnC.wasReleased();

    btnE = M5.BtnB.pressedFor(500);
  }
}

// Get button
void getButton(uint8_t modeCurrent = 1, uint8_t menuMode = 0)
{
  static uint8_t modeChange = 10;

  if (M5.getBoard() == m5::board_t::board_M5Stack)
  {
    M5.update();

    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.isPressed();
    btnC = M5.BtnC.isPressed();
  }
  else if (M5.getBoard() == m5::board_t::board_M5ATOM)
  {
    M5.update();
    btnB = M5.BtnA.isPressed();
    btnA = 0;
    btnC = 0;
    if(digitalRead(32) == 0) btnA = 1;
    else if(digitalRead(26) == 0) btnC = 1;
  }
  else if (M5.getBoard() == m5::board_t::board_M5StackCore2)
  {
    if (menuMode == 0)
    {
      myBtn[3].active = true; // active bntD
      if (modeChange != modeCurrent)
      {
        modeChange = modeCurrent;

        if (modeChange != 2)
        {
          Serial.println("A");
          for (uint8_t i = 0; i < 3; i++)
            myBtn[i].active = true; // active btnA, btnB, btnC
          for (uint8_t i = 4; i < limit; i++)
            myBtn[i].active = false; // inactive others
        }
        else
        {
          Serial.println("B");
          for (uint8_t i = 0; i < 3; i++)
            myBtn[i].active = false; // active btnA, btnB, btnC
          for (uint8_t i = 4; i < limit; i++)
            myBtn[i].active = true; // inactive others
        }
      }
    }
    else
    {
      for (uint8_t i = 0; i < 3; i++)
        myBtn[i].active = true; // active btnA, btnB, btnC
      for (uint8_t i = 3; i < limit; i++)
        myBtn[i].active = false; // inactive others
    }

    M5.update();

    auto t = M5.Touch.getDetail();
    uint8_t limit = sizeof myBtn / sizeof myBtn[0];

    for (uint8_t i = 0; i < limit; i++)
    {
      myBtn[i].read = false;

      if (t.state == 1 || t.state == 5 || t.state == 9 || t.state == 13)
      {
        if (
            myBtn[i].active == true && (t.x >= myBtn[i].x) && (t.x <= (myBtn[i].x + myBtn[i].w)) && (t.y >= myBtn[i].y) && (t.y <= (myBtn[i].y + myBtn[i].h)))
        {
          myBtn[i].read = true;
        }
      }
    }

    btnA = myBtn[0].read;
    btnB = myBtn[1].read;
    btnC = myBtn[2].read;
    btnD = myBtn[3].read;

    btnDTMF1 = myBtn[4].read;
    btnDTMF2 = myBtn[5].read;
    btnDTMF3 = myBtn[6].read;
    btnDTMF4 = myBtn[7].read;
    btnDTMF5 = myBtn[8].read;
    btnDTMF6 = myBtn[9].read;
    btnDTMF7 = myBtn[10].read;
    btnDTMF8 = myBtn[11].read;
    btnDTMF9 = myBtn[12].read;
  }

  //Serial.printf("%d %d %d %d\n", btnA, btnB, btnC, digitalRead(32));

  M5.Speaker.setVolume(beepCurrent);

  // Play sound
  if (
      btnA || btnC)
  {
    M5.Speaker.tone(1000, 50);
  }
  else if (
    btnD)
  {
    M5.Speaker.tone(1000, 50);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  else if (
      btnB ||
      btnDTMF1 || btnDTMF2 || btnDTMF3 ||
      btnDTMF4 || btnDTMF5 || btnDTMF6 ||
      btnDTMF7 || btnDTMF8 || btnDTMF9)
  {
    M5.Speaker.tone(2000, 50);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}