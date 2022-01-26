// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

uint8_t btnA, btnB, btnC, btnD = 0;
uint8_t btnDTMF1, btnDTMF2, btnDTMF3 = 0;
uint8_t btnDTMF4, btnDTMF5, btnDTMF6 = 0;
uint8_t btnDTMF7, btnDTMF8, btnDTMF9 = 0;

// Power init
void power()
{
    M5.Power.begin();
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
void getButton(uint8_t modeCurrent = 1)
{
    M5.update();
    btnA = M5.BtnA.read();
    btnB = M5.BtnB.read();
    btnC = M5.BtnC.read();
}

// Speaker
void speaker()
{
    M5.Speaker.setVolume(1);
    M5.Speaker.update();
}

// Manage Tone
void totTone()
{
    M5.Speaker.tone(2000);
}

// Manage Mute
void totMute()
{
    M5.Speaker.mute();
}

// Manage brightness
void setBrightness(uint8_t value)
{
    M5.Lcd.setBrightness(value);
}