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

// Scroll
void scroll(uint8_t pause)
{
  int16_t h = 18;
  int16_t w;
  int16_t limit =  M5.Lcd.width();

  //delay(pause);
  vTaskDelay(pdMS_TO_TICKS(pause));

  img.setTextSize(1);          // Font size scaling is x1
  img.setTextFont(2);          // Font 2 selected

  w = img.textWidth(message) + 40;
  if (w < limit)
  {
    w = limit;
  }
  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    img.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics  
  img.setTextColor(TFT_WHITE); // White text, no background colour
  img.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  img.drawString(message, pos, 2);
  img.drawString(message, pos - w, 2);

  img.pushSprite(0, 78);

  pos -= 1;
  if (pos == 0)
  {
    pos = w;
  }
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

// Get local time
void printLocalTime()
{
  char timeStringBuff[10]; //10 chars should be enough

  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  if(timeinfo.tm_isdst == 1)
  {
    shift = utc + 1;
    shift = shift - 2;
  }
  else{
    shift = utc - 2;
  }

  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
  //Serial.println(timeStringBuff);
  //Serial.println(shift);

  dateString = String(timeStringBuff);
}