// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

uint8_t btnA, btnB, btnC, btnD, btnE, btnF = 0;

Gesture swipeLeft("swipe left", 160, DIR_LEFT, 30, true);
Gesture swipeRight("swipe right", 160, DIR_RIGHT, 30, true);
Gesture swipeDown("swipe down", 120, DIR_DOWN, 30, true);
Gesture swipeUp("swipe up", 120, DIR_UP, 30, true);

// Expand virtual fucking button
Button myBtnA(0, 200, 80, 80);
Button myBtnB(120, 200, 80, 80);
Button myBtnC(240, 200, 80, 80);
//Button myBtnD(0, 0, 320, 80);

// Swipe left manage
void detectSwipeLeft(Event &e)
{
  btnE = 1;
}

// Swipe right manage
void detectSwipeRight(Event &e)
{
  btnE = 2;
}

// Swipe bottom manage
void detectSwipeDown(Event &e)
{
  btnF = 1;
}

// Swipe top manage
void detectSwipeUp(Event &e)
{
  btnF = 2;
}

// Power init
void power()
{
  // By default, M5.begin() will initialize AXP192 chip on Core2
  M5.Axp.SetLed(0);
  swipeLeft.addHandler(detectSwipeLeft);
  swipeRight.addHandler(detectSwipeRight);
  swipeDown.addHandler(detectSwipeDown);
  swipeUp.addHandler(detectSwipeUp);
}

// Get Battery level
uint8_t getBatteryLevel(bool type)
{
  float baterryVoltage = M5.Axp.GetBatVoltage();
  uint8_t batteryPercentage = (baterryVoltage < 3.2) ? 0 : (baterryVoltage - 3.2) * 100;

  if (type == 0)
  {
    if (batteryPercentage > 80)
    {
      return 100;
    }
    else if (batteryPercentage > 60)
    {
      return 75;
    }
    else if (batteryPercentage > 40)
    {
      return 50;
    }
    else if (batteryPercentage > 20)
    {
      return 25;
    }
    return 0;
  }
  else
  {
    return batteryPercentage;
  }
}

// Control if charging
bool isCharging()
{
  return M5.Axp.isCharging();
}

// Get button
void getButton()
{
  M5.update();
  btnA = myBtnA.read();
  btnB = myBtnB.read();
  btnC = myBtnC.read();
  //btnD = myBtnD.read();

  //Serial.println(btnA);
  //Serial.println(btnB);
  //Serial.println(btnC);
  //Serial.println(btnD);
  //Serial.println("----------");

  if (btnE == 1)
  {
    btnA = 1;
    btnE = 0;
  }
  else if (btnE == 2)
  {
    btnC = 1;
    btnE = 0;
  }

  if (btnF == 1 || btnF == 2)
  {
    btnB = 1;
    btnF = 0;
  }
}

// Speaker
void speaker()
{
  // Core2 is not ready
}

// Manage Tone
void totTone()
{
  M5.Axp.SetLed(1);
}

// Manage Mute
void totMute()
{
  M5.Axp.SetLed(0);
}