// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

uint8_t btnA, btnB, btnC, btnD = 0;
uint8_t btnDTMF1, btnDTMF2, btnDTMF3 = 0;
uint8_t btnDTMF4, btnDTMF5, btnDTMF6 = 0;
uint8_t btnDTMF7, btnDTMF8, btnDTMF9 = 0;

struct Button
{
  String name; 
  int x;
  int y;
  int w;
  int h;
  boolean active;
  boolean read;
};

Button myBtn[] = {
  {"myBtnA",   0, 200, 100,  80, true, false},
  {"myBtnB", 110, 200, 100,  80, true, false},
  {"myBtnC", 220, 200, 100,  80, true, false},
  {"myBtnD",   0,   0, 320,  80, true, false},

  {"myBtnDTMF1", 10 + (110 * 0), 109 + (48 * 0), 80, 30, false, false},
  {"myBtnDTMF2", 10 + (110 * 1), 109 + (48 * 0), 80, 30, true, false},
  {"myBtnDTMF3", 10 + (110 * 2), 109 + (48 * 0), 80, 30, false, false},
  {"myBtnDTMF4", 10 + (110 * 0), 109 + (48 * 1), 80, 30, false, false},
  {"myBtnDTMF5", 10 + (110 * 1), 109 + (48 * 1), 80, 30, false, false},
  {"myBtnDTMF6", 10 + (110 * 2), 109 + (48 * 1), 80, 30, false, false},
  {"myBtnDTMF7", 10 + (110 * 0), 109 + (48 * 2), 80, 30, false, false},
  {"myBtnDTMF8", 10 + (110 * 1), 109 + (48 * 2), 80, 30, false, false},
  {"myBtnDTMF9", 10 + (110 * 2), 109 + (48 * 2), 80, 30, false, false},
};

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

// Get button
void getButton(uint8_t modeCurrent = 1, uint8_t menuMode = 0)
{
  M5.update();

  if (M5.getBoard() == m5::board_t::board_M5Stack)
  {
    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.isPressed();
    btnC = M5.BtnC.isPressed();
  }
  else if (M5.getBoard() == m5::board_t::board_M5StackCore2)
  {
    auto t = M5.Touch.getDetail();
    uint8_t limit = sizeof myBtn / sizeof myBtn[0];

    for (uint8_t i = 0; i < limit; i++)
    {
      myBtn[i].read = false;

      if(t.state == 1 || t.state == 5 || t.state == 9 || t.state == 13)
      {
        if(
        myBtn[i].active == true
        && (t.x >= myBtn[i].x) 
        && (t.x <= (myBtn[i].x + myBtn[i].w)) 
        && (t.y >= myBtn[i].y)
        && (t.y <= (myBtn[i].y + myBtn[i].h))
        ) 
        {
          myBtn[i].read = true;
        }
      }
    }

    btnA = myBtn[0].read;
    btnB = myBtn[1].read;
    btnC = myBtn[2].read;
    btnD = myBtn[3].read;
  }
}

// Manage brightness
void setBrightness(uint8_t value)
{
  display.setBrightness(value);
}