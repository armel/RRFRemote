// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

CORE2SPK _Core2SPK;

uint8_t btnA, btnB, btnC, btnD = 0;
uint8_t btnDTMF1, btnDTMF2, btnDTMF3 = 0;
uint8_t btnDTMF4, btnDTMF5, btnDTMF6 = 0;
uint8_t btnDTMF7, btnDTMF8, btnDTMF9 = 0;

/*
Gesture swipeLeft(  "swipe left",   160, DIR_LEFT,  30, true);
Gesture swipeRight( "swipe right",  160, DIR_RIGHT, 30, true);
Gesture swipeDown(  "swipe down",   120, DIR_DOWN,  30, true);
Gesture swipeUp(    "swipe up",     120, DIR_UP,    30, true);
*/

// Expand virtual fucking button
Button myBtnA(  0, 200, 100,  80);
Button myBtnB(110, 200, 100,  80);
Button myBtnC(220, 200, 100,  80);
Button myBtnD(  0,   0, 320,  80);

// Special DTMF button
/*
Button myBtnDTMF1(10 + (110 * 0), 109 + (48 * 0), 80, 30, false, "", {BLACK, WHITE, WHITE});
Button myBtnDTMF2(10 + (110 * 1), 109 + (48 * 0), 80, 30, false, "", {BLACK, WHITE, WHITE});
Button myBtnDTMF3(10 + (110 * 2), 109 + (48 * 0), 80, 30, false, "", {BLACK, WHITE, WHITE});
Button myBtnDTMF4(10 + (110 * 0), 109 + (48 * 1), 80, 30, false, "", {BLACK, WHITE, WHITE});
Button myBtnDTMF5(10 + (110 * 1), 109 + (48 * 1), 80, 30, false, "", {BLACK, WHITE, WHITE});
Button myBtnDTMF6(10 + (110 * 2), 109 + (48 * 1), 80, 30, false, "", {BLACK, WHITE, WHITE});
Button myBtnDTMF7(10 + (110 * 0), 109 + (48 * 2), 80, 30, false, "", {BLACK, WHITE, WHITE});
Button myBtnDTMF8(10 + (110 * 1), 109 + (48 * 2), 80, 30, false, "", {BLACK, WHITE, WHITE});
Button myBtnDTMF9(10 + (110 * 2), 109 + (48 * 2), 80, 30, false, "", {BLACK, WHITE, WHITE});
*/

Button myBtnDTMF1(10 + (110 * 0), 109 + (48 * 0), 80, 30);
Button myBtnDTMF2(10 + (110 * 1), 109 + (48 * 0), 80, 30);
Button myBtnDTMF3(10 + (110 * 2), 109 + (48 * 0), 80, 30);
Button myBtnDTMF4(10 + (110 * 0), 109 + (48 * 1), 80, 30);
Button myBtnDTMF5(10 + (110 * 1), 109 + (48 * 1), 80, 30);
Button myBtnDTMF6(10 + (110 * 2), 109 + (48 * 1), 80, 30);
Button myBtnDTMF7(10 + (110 * 0), 109 + (48 * 2), 80, 30);
Button myBtnDTMF8(10 + (110 * 1), 109 + (48 * 2), 80, 30);
Button myBtnDTMF9(10 + (110 * 2), 109 + (48 * 2), 80, 30);

/*
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
*/

// Power init
void power()
{
  // By default, M5.begin() will initialize AXP192 chip on Core2
  M5.Axp.SetLed(0);
  /*
  swipeLeft.addHandler(detectSwipeLeft);
  swipeRight.addHandler(detectSwipeRight);
  swipeDown.addHandler(detectSwipeDown);
  swipeUp.addHandler(detectSwipeUp);
  */
}

// Get Battery level
uint8_t getBatteryLevel(bool type)
{
  float baterryVoltage = M5.Axp.GetBatVoltage();
  uint16_t batteryPercentage = (baterryVoltage < 3.2) ? 0 : (baterryVoltage - 3.2) * 100;

  if(batteryPercentage > 100) {
    batteryPercentage = 100;
  }

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
void getButton(uint8_t modeCurrent = 1)
{
  static uint8_t modeChange = 10;

  if(modeChange != modeCurrent) {
    modeChange = modeCurrent;

    if(modeChange != 2) {
      myBtnDTMF1.hide();
      myBtnDTMF2.hide();
      myBtnDTMF3.hide();
      myBtnDTMF4.hide();
      myBtnDTMF5.hide();
      myBtnDTMF6.hide();
      myBtnDTMF7.hide();
      myBtnDTMF8.hide();
      myBtnDTMF9.hide();

      myBtnA.draw();
      myBtnB.draw();
      myBtnC.draw();
      //myBtnD.draw();

      //Serial.println("DTMF OFF");
    }
    else {
      myBtnDTMF1.draw();
      myBtnDTMF2.draw();
      myBtnDTMF3.draw();
      myBtnDTMF4.draw();
      myBtnDTMF5.draw();
      myBtnDTMF6.draw();
      myBtnDTMF7.draw();
      myBtnDTMF8.draw();
      myBtnDTMF9.draw();

      myBtnA.hide();
      myBtnB.hide();
      myBtnC.hide();
      //myBtnD.hide();

      //Serial.println("DTMF ON");
    }
  }

  M5.update();

  btnA = myBtnA.read();
  btnB = myBtnB.read();
  btnC = myBtnC.read();
  btnD = myBtnD.isPressed();

  btnDTMF1 = myBtnDTMF1.read();
  btnDTMF2 = myBtnDTMF2.read();
  btnDTMF3 = myBtnDTMF3.read();
  btnDTMF4 = myBtnDTMF4.read();
  btnDTMF5 = myBtnDTMF5.read();
  btnDTMF6 = myBtnDTMF6.read();
  btnDTMF7 = myBtnDTMF7.read();
  btnDTMF8 = myBtnDTMF8.read();
  btnDTMF9 = myBtnDTMF9.read();

  if(
      btnA || btnB || btnC || btnD || 
      btnDTMF1 || btnDTMF2 || btnDTMF3 ||
      btnDTMF4 || btnDTMF5 || btnDTMF6 ||
      btnDTMF7 || btnDTMF8 || btnDTMF9   
    ) 
  {
    _Core2SPK.playBeep(5000, 100, 2000, false);
  }

  /*
  Serial.print(btnA);
  Serial.print(" - ");
  Serial.print(btnB);
  Serial.print(" - ");
  Serial.println(btnC);

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
  */
}

// Manage Vibrator
void vibration(int t) {
  M5.Axp.SetLDOEnable(3, true);
  delay(t);
  M5.Axp.SetLDOEnable(3, false);
}

// Speaker
void speaker()
{
  M5.Axp.SetSpkEnable(true);
  _Core2SPK.begin();
  _Core2SPK.playBeep(5000, 100, 2000, false);
  vibration(200);
}

// Manage Tone
void totTone()
{
  _Core2SPK.playBeep(5000, 100, 2000, false);
  vTaskDelay(pdMS_TO_TICKS(100));
  //M5.Axp.SetLed(1);
}

// Manage Mute
void totMute()
{
  //M5.Axp.SetLed(0);
}

// Manage brightness
void setBrightness(uint8_t value)
{
    uint16_t tmp = map(value, 0, 128, 2500, 3300);
    M5.Axp.SetLcdVoltage(tmp); // from 2500 to 3300
}