// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Wifi callback On
void callbackWifiOn(WiFiEvent_t event, WiFiEventInfo_t info)
{
  wifiConnected = true;
  Serial.println("Wifi Client Connected");
}

// Wifi callback Got IP
void callbackWifiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println(WiFi.localIP());
}

// Wifi callback Off
void callbackWifiOff(WiFiEvent_t event, WiFiEventInfo_t info)
{
  wifiConnected = false;
  Serial.println("Wifi Client Disconnected");

  WiFi.begin(config[(configCurrent * 6)], config[(configCurrent * 6) + 1]);
}

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
  switch (colorCurrent)
  {
  case 0:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_ROUGE.r, TFT_HEADER_ROUGE.g, TFT_HEADER_ROUGE.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_ROUGE.r, TFT_INFO_ROUGE.g, TFT_INFO_ROUGE.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_ROUGE.r, TFT_FRONT_ROUGE.g, TFT_FRONT_ROUGE.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_ROUGE.r, TFT_BACK_ROUGE.g, TFT_BACK_ROUGE.b);
    BMP_HEADER = TFT_HEADER_ROUGE;
    BMP_INFO = TFT_INFO_ROUGE;
    BMP_FRONT = TFT_FRONT_ROUGE;
    BMP_BACK = TFT_BACK_ROUGE;
    break;

  case 1:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_ROSE.r, TFT_HEADER_ROSE.g, TFT_HEADER_ROSE.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_ROSE.r, TFT_INFO_ROSE.g, TFT_INFO_ROSE.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_ROSE.r, TFT_FRONT_ROSE.g, TFT_FRONT_ROSE.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_ROSE.r, TFT_BACK_ROSE.g, TFT_BACK_ROSE.b);
    BMP_HEADER = TFT_HEADER_ROSE;
    BMP_INFO = TFT_INFO_ROSE;
    BMP_FRONT = TFT_FRONT_ROSE;
    BMP_BACK = TFT_BACK_ROSE;
    break;

  case 2:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_VIOLET.r, TFT_HEADER_VIOLET.g, TFT_HEADER_VIOLET.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_VIOLET.r, TFT_INFO_VIOLET.g, TFT_INFO_VIOLET.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_VIOLET.r, TFT_FRONT_VIOLET.g, TFT_FRONT_VIOLET.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_VIOLET.r, TFT_BACK_VIOLET.g, TFT_BACK_VIOLET.b);
    BMP_HEADER = TFT_HEADER_VIOLET;
    BMP_INFO = TFT_INFO_VIOLET;
    BMP_FRONT = TFT_FRONT_VIOLET;
    BMP_BACK = TFT_BACK_VIOLET;
    break;

  case 3:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_BLEU.r, TFT_HEADER_BLEU.g, TFT_HEADER_BLEU.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_BLEU.r, TFT_INFO_BLEU.g, TFT_INFO_BLEU.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_BLEU.r, TFT_FRONT_BLEU.g, TFT_FRONT_BLEU.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_BLEU.r, TFT_BACK_BLEU.g, TFT_BACK_BLEU.b);
    BMP_HEADER = TFT_HEADER_BLEU;
    BMP_INFO = TFT_INFO_BLEU;
    BMP_FRONT = TFT_FRONT_BLEU;
    BMP_BACK = TFT_BACK_BLEU;
    break;

  case 4:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_TURQUOISE.r, TFT_HEADER_TURQUOISE.g, TFT_HEADER_TURQUOISE.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_TURQUOISE.r, TFT_INFO_TURQUOISE.g, TFT_INFO_TURQUOISE.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_TURQUOISE.r, TFT_FRONT_TURQUOISE.g, TFT_FRONT_TURQUOISE.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_TURQUOISE.r, TFT_BACK_TURQUOISE.g, TFT_BACK_TURQUOISE.b);
    BMP_HEADER = TFT_HEADER_TURQUOISE;
    BMP_INFO = TFT_INFO_TURQUOISE;
    BMP_FRONT = TFT_FRONT_TURQUOISE;
    BMP_BACK = TFT_BACK_TURQUOISE;
    break;

  case 5:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_VERT.r, TFT_HEADER_VERT.g, TFT_HEADER_VERT.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_VERT.r, TFT_INFO_VERT.g, TFT_INFO_VERT.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_VERT.r, TFT_FRONT_VERT.g, TFT_FRONT_VERT.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_VERT.r, TFT_BACK_VERT.g, TFT_BACK_VERT.b);
    BMP_HEADER = TFT_HEADER_VERT;
    BMP_INFO = TFT_INFO_VERT;
    BMP_FRONT = TFT_FRONT_VERT;
    BMP_BACK = TFT_BACK_VERT;
    break;

  case 6:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_ORANGE.r, TFT_HEADER_ORANGE.g, TFT_HEADER_ORANGE.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_ORANGE.r, TFT_INFO_ORANGE.g, TFT_INFO_ORANGE.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_ORANGE.r, TFT_FRONT_ORANGE.g, TFT_FRONT_ORANGE.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_ORANGE.r, TFT_BACK_ORANGE.g, TFT_BACK_ORANGE.b);
    BMP_HEADER = TFT_HEADER_ORANGE;
    BMP_INFO = TFT_INFO_ORANGE;
    BMP_FRONT = TFT_FRONT_ORANGE;
    BMP_BACK = TFT_BACK_ORANGE;
    break;

  case 7:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_MARRON.r, TFT_HEADER_MARRON.g, TFT_HEADER_MARRON.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_MARRON.r, TFT_INFO_MARRON.g, TFT_INFO_MARRON.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_MARRON.r, TFT_FRONT_MARRON.g, TFT_FRONT_MARRON.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_MARRON.r, TFT_BACK_MARRON.g, TFT_BACK_MARRON.b);
    BMP_HEADER = TFT_HEADER_MARRON;
    BMP_INFO = TFT_INFO_MARRON;
    BMP_FRONT = TFT_FRONT_MARRON;
    BMP_BACK = TFT_BACK_MARRON;
    break;

  case 8:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_GRIS.r, TFT_HEADER_GRIS.g, TFT_HEADER_GRIS.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_GRIS.r, TFT_INFO_GRIS.g, TFT_INFO_GRIS.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_GRIS.r, TFT_FRONT_GRIS.g, TFT_FRONT_GRIS.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_GRIS.r, TFT_BACK_GRIS.g, TFT_BACK_GRIS.b);
    BMP_HEADER = TFT_HEADER_GRIS;
    BMP_INFO = TFT_INFO_GRIS;
    BMP_FRONT = TFT_FRONT_GRIS;
    BMP_BACK = TFT_BACK_GRIS;
    break;

  case 9:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_CREPUSCULE.r, TFT_HEADER_CREPUSCULE.g, TFT_HEADER_CREPUSCULE.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_CREPUSCULE.r, TFT_INFO_CREPUSCULE.g, TFT_INFO_CREPUSCULE.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_CREPUSCULE.r, TFT_FRONT_CREPUSCULE.g, TFT_FRONT_CREPUSCULE.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_CREPUSCULE.r, TFT_BACK_CREPUSCULE.g, TFT_BACK_CREPUSCULE.b);
    BMP_HEADER = TFT_HEADER_CREPUSCULE;
    BMP_INFO = TFT_INFO_CREPUSCULE;
    BMP_FRONT = TFT_FRONT_CREPUSCULE;
    BMP_BACK = TFT_BACK_CREPUSCULE;
    break;

  case 10:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_FORET.r, TFT_HEADER_FORET.g, TFT_HEADER_FORET.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_FORET.r, TFT_INFO_FORET.g, TFT_INFO_FORET.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_FORET.r, TFT_FRONT_FORET.g, TFT_FRONT_FORET.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_FORET.r, TFT_BACK_FORET.g, TFT_BACK_FORET.b);
    BMP_HEADER = TFT_HEADER_FORET;
    BMP_INFO = TFT_INFO_FORET;
    BMP_FRONT = TFT_FRONT_FORET;
    BMP_BACK = TFT_BACK_FORET;
    break;

  case 11:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_ARDOISE.r, TFT_HEADER_ARDOISE.g, TFT_HEADER_ARDOISE.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_ARDOISE.r, TFT_INFO_ARDOISE.g, TFT_INFO_ARDOISE.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_ARDOISE.r, TFT_FRONT_ARDOISE.g, TFT_FRONT_ARDOISE.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_ARDOISE.r, TFT_BACK_ARDOISE.g, TFT_BACK_ARDOISE.b);
    BMP_HEADER = TFT_HEADER_ARDOISE;
    BMP_INFO = TFT_INFO_ARDOISE;
    BMP_FRONT = TFT_FRONT_ARDOISE;
    BMP_BACK = TFT_BACK_ARDOISE;
    break;

  case 12:
    TFT_HEADER = M5.Displays(display).color565(TFT_HEADER_FLORAL.r, TFT_HEADER_FLORAL.g, TFT_HEADER_FLORAL.b);
    TFT_INFO = M5.Displays(display).color565(TFT_INFO_FLORAL.r, TFT_INFO_FLORAL.g, TFT_INFO_FLORAL.b);
    TFT_FRONT = M5.Displays(display).color565(TFT_FRONT_FLORAL.r, TFT_FRONT_FLORAL.g, TFT_FRONT_FLORAL.b);
    TFT_BACK = M5.Displays(display).color565(TFT_BACK_FLORAL.r, TFT_BACK_FLORAL.g, TFT_BACK_FLORAL.b);
    BMP_HEADER = TFT_HEADER_FLORAL;
    BMP_INFO = TFT_INFO_FLORAL;
    BMP_FRONT = TFT_FRONT_FLORAL;
    BMP_BACK = TFT_BACK_FLORAL;
    break;
  }
}

// Clear screen
void clear()
{
  String option = String(settingsMenu[menuCurrent]);

  // Reset
  message = "";
  // display.clear();
  resetColor();

  if (menuMode != 1 || (menuMode == 1 && option == "COULEUR"))
  {
    // Header
    M5.Displays(display).fillRect(0 + offsetX, 0 + offsetY, 320, 44, TFT_HEADER);
    M5.Displays(display).fillRect(0 + offsetX, 45 + offsetY, 320, 55, TFT_INFO);
    M5.Displays(display).drawFastHLine(0 + offsetX, 0 + offsetY, 320, TFT_WHITE);
    M5.Displays(display).drawFastHLine(0 + offsetX, 44 + offsetY, 320, TFT_WHITE);
  }

  // Info zone
  M5.Displays(display).fillRect(0 + offsetX, 98 + offsetY, 320, 2, TFT_INFO);

  // Grey zone
  M5.Displays(display).drawFastHLine(0 + offsetX, 100 + offsetY, 320, TFT_WHITE);
  M5.Displays(display).fillRect(0 + offsetX, 101 + offsetY, 320, 240 - 101, TFT_BACK);

  if (modeCurrent == 0)
  {
    // Elsewhere
    M5.Displays(display).fillRoundRect(1 + offsetX, 154 + offsetY, 155, 85, 4, TFT_WHITE);
    M5.Displays(display).fillRoundRect(2 + offsetX, 155 + offsetY, 26, 83, 4, TFT_FRONT);

    for (uint8_t i = 24; i < 27; i++)
    {
      M5.Displays(display).drawFastVLine(i + offsetX, 155 + offsetY, 83, TFT_FRONT);
    }

    M5.Displays(display).drawFastVLine(28 + offsetX, 155 + offsetY, 83, TFT_BACK);
    M5.Displays(display).drawFastVLine(100 + offsetX, 155 + offsetY, 83, TFT_BACK);

    for (uint8_t i = 0; i < 5; i++)
    {
      M5.Displays(display).drawFastHLine(2 + offsetX, 168 + (i * 14) + offsetY, 152, TFT_BACK);
    }

    // Log
    //display.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
  }
  else if (modeCurrent == 1)
  {
    for (uint8_t i = 0; i <= 1; i++)
    {
      M5.Displays(display).fillRoundRect(1 + (169 * i) + offsetX, 125 + offsetY, 149, 115, 4, TFT_WHITE);
      M5.Displays(display).fillRoundRect(2 + (169 * i) + offsetX, 126 + offsetY, 147, 113, 4, TFT_FRONT);
      M5.Displays(display).fillRect(51 + (169 * i) + offsetX, 126 + offsetY, 98, 113, TFT_WHITE);
      M5.Displays(display).drawFastVLine(50 + (169 * i) + offsetX, 126 + offsetY, 113, TFT_BACK);

      for (uint8_t k = 1; k <= 5; k++)
      {
        M5.Displays(display).drawFastHLine(1 + (169 * i) + offsetX, 125 + (k * 18) + k + offsetY, 148, TFT_BACK);
      }
    }
  }
  else
  {
    for (uint8_t i = 0; i <= 2; i++)
    {
      for (uint8_t j = 0; j <= 2; j++)
      {
        M5.Displays(display).fillRoundRect(0 + (110 * j) + offsetX, 104 + (48 * i) + offsetY, 100, 40, 4, TFT_WHITE);
        M5.Displays(display).fillRoundRect(1 + (110 * j) + offsetX, 105 + (48 * i) + offsetY, 98, 38, 4, TFT_FRONT);
      }
    }
  }
}

// Scroll
void scroll(uint8_t pause)
{  
  uint16_t h = 21;
  uint16_t w;
  uint16_t limit = M5.Displays(display).width();
  static uint32_t timer = 0, wait = 0;

  timer = millis();

  w = Sprite.textWidth(message) + 40;
  if (w < limit)
  {
    w = limit;
  }
  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    Sprite.drawFastHLine(0, h, w, TFT_INFO);

  // Now print text on top of the graphics
  Sprite.setFont(&tahoma8pt7b);
  Sprite.setTextColor(TFT_WHITE); // White text, no background colour
  Sprite.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  Sprite.drawString(message, pos, 2);
  Sprite.drawString(message, pos - w, 2);

  Sprite.pushSprite(0 + offsetX, 78 + offsetY);

  pos -= 1;
  if (pos == 0)
  {
    pos = w;
  }

  wait = millis() - timer;
  if (wait < pause)
  {
    vTaskDelay(pdMS_TO_TICKS(pause - wait));
    // Serial.println(pause - wait);
  }
}

// Detect rotation
void checkAcceleration()
{
  float accX = 0.0F;
  float accY = 0.0F;
  float accZ = 0.0F;

  if (M5.Imu.isEnabled())
  {
    M5.Imu.getAccel(&accX, &accY, &accZ);

    if (int(accY * 1000) < -500 && M5.Displays(display).getRotation() != 3)
    {
     M5.Displays(display).clear();
      reset = 0;
      refresh = 0;
      menuRefresh = 0;
     M5.Displays(display).setRotation(3);
    }
    else if (int(accY * 1000) > 500 && M5.Displays(display).getRotation() != 1)
    {
     M5.Displays(display).clear();
      reset = 0;
      refresh = 0;
      menuRefresh = 0;
     M5.Displays(display).setRotation(1);
    }
  }
}

// Compute distance
int computeDistance(float latitudeLink, float longitudeLink)
{
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
void updateLocalTime()
{
  char timeStringBuff[10]; // 10 chars should be enough
  char utcStringBuff[10];  // 10 chars should be enough

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
  strftime(utcStringBuff, sizeof(utcStringBuff), "%z", &timeinfo);

  sscanf(utcStringBuff, "%d", &utc);
  utc = utc / 100;

  // Serial.println(utc);

  dateString = String(timeStringBuff);
}

// M5Screen4bmp() (dump 4 bits screen to a WiFi client)
bool M5Screen4bmp()
{
  uint16_t image_height = M5.Displays(display).height();
  uint16_t image_width = M5.Displays(display).width();
  uint32_t filesize = 54 + 28 + ((image_width * image_height) / 2);
  uint8_t l = 0;
  uint8_t r = 0;

  unsigned char line_data_raw[image_width * 3];
  unsigned char line_data_bmp[(image_width / 2) * 10];

  unsigned char header[54] = {
      'B', 'M',         // BMP signature (Windows 3.1x, 95, NT, …)
      0, 0, 0, 0,       // Image file size in bytes
      0, 0, 0, 0,       // Reserved
      54 + 28, 0, 0, 0, // Start of pixel array
      40, 0, 0, 0,      // Info header size
      0, 0, 0, 0,       // Image width
      0, 0, 0, 0,       // Image height
      1, 0,             // Number of color planes
      4, 0,             // Bits per pixel
      0, 0, 0, 0,       // Compression
      0, 0, 0, 0,       // Image size (can be 0 for uncompressed images)
      0, 0, 0, 0,       // Horizontal resolution (dpm)
      0, 0, 0, 0,       // Vertical resolution (dpm)
      7, 0, 0, 0,       // Colors in color table (0 = none)
      0, 0, 0, 0};      // Important color count (0 = all colors are important)

  unsigned char palette[28] = {
      0, 0, 0, 255,
      255, 255, 255, 255,
      192, 192, 192, 255,
      BMP_HEADER.b, BMP_HEADER.g, BMP_HEADER.r, 255,
      BMP_INFO.b, BMP_INFO.g, BMP_INFO.r, 255,
      BMP_FRONT.b, BMP_FRONT.g, BMP_FRONT.r, 255,
      BMP_BACK.b, BMP_BACK.g, BMP_BACK.r, 255};

  // Fill filesize, width and heigth in the header array
  for (uint8_t i = 0; i < 4; i++)
  {
    header[2 + i] = (char)((filesize >> (8 * i)) & 255);
    header[18 + i] = (char)((image_width >> (8 * i)) & 255);
    header[22 + i] = (char)((image_height >> (8 * i)) & 255);
  }
  // Write header
  httpClient.write(header, 54);

  // Write palette
  httpClient.write(palette, 28);

  // The coordinate origin of a BMP image is at the bottom left.
  // Therefore, the image must be read from bottom to top.
  for (uint16_t y = image_height; y > 0; y -= 10)
  {
    uint16_t k = 0;

    for (uint8_t z = 0; z < 10; z++)
    {
      // Get one line of the screen content
      M5.Displays(display).readRectRGB(0, y - 1 - z, image_width, 1, line_data_raw);
      // BMP color order is: Blue, Green, Red
      // Return values from readRectRGB is: Red, Green, Blue
      // Therefore: R und B need to be swapped
      //
      // Note the strange euristic algo with abs substract to match palette color
      // Because, for example, TFT_WHITE is #FCFCFC and not #FFFFFF, when you read screen
      for (uint16_t x = 0; x < image_width; x += 2)
      {
        // Left
        if (
            line_data_raw[x * 3] == 0 &&
            line_data_raw[x * 3 + 1] == 0 &&
            line_data_raw[x * 3 + 2] == 0)
        {
          l = 0b0000;
        }
        else if (
            abs(line_data_raw[x * 3] - 255) < 16 &&
            abs(line_data_raw[x * 3 + 1] - 255) < 16 &&
            abs(line_data_raw[x * 3 + 2] - 255) < 16)
        {
          l = 0b0001;
        }
        else if (
            abs(line_data_raw[x * 3] - 192) < 16 &&
            abs(line_data_raw[x * 3 + 1] - 192) < 16 &&
            abs(line_data_raw[x * 3 + 2] - 192) < 16)
        {
          l = 0b0010;
        }
        else if (
            abs(line_data_raw[x * 3] - BMP_HEADER.r) < 16 &&
            abs(line_data_raw[x * 3 + 1] - BMP_HEADER.g) < 16 &&
            abs(line_data_raw[x * 3 + 2] - BMP_HEADER.b) < 16)
        {
          l = 0b0011;
        }
        else if (
            abs(line_data_raw[x * 3] - BMP_INFO.r) < 16 &&
            abs(line_data_raw[x * 3 + 1] - BMP_INFO.g) < 16 &&
            abs(line_data_raw[x * 3 + 2] - BMP_INFO.b) < 16)
        {
          l = 0b0100;
        }
        else if (
            abs(line_data_raw[x * 3] - BMP_FRONT.r) < 16 &&
            abs(line_data_raw[x * 3 + 1] - BMP_FRONT.g) < 16 &&
            abs(line_data_raw[x * 3 + 2] - BMP_FRONT.b) < 16)
        {
          l = 0b0101;
        }
        else
        {
          l = 0b0110;
        }

        // Right
        if (
            line_data_raw[(x + 1) * 3] == 0 &&
            line_data_raw[(x + 1) * 3 + 1] == 0 &&
            line_data_raw[(x + 1) * 3 + 2] == 0)
        {
          r = 0b0000;
        }
        else if (
            abs(line_data_raw[(x + 1) * 3] - 255) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 1] - 255) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 2] - 255) < 16)
        {
          r = 0b0001;
        }
        else if (
            abs(line_data_raw[(x + 1) * 3] - 192) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 1] - 192) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 2] - 192) < 16)
        {
          r = 0b0010;
        }
        else if (
            abs(line_data_raw[(x + 1) * 3] - BMP_HEADER.r) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 1] - BMP_HEADER.g) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 2] - BMP_HEADER.b) < 16)
        {
          r = 0b0011;
        }
        else if (
            abs(line_data_raw[(x + 1) * 3] - BMP_INFO.r) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 1] - BMP_INFO.g) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 2] - BMP_INFO.b) < 16)
        {
          r = 0b0100;
        }
        else if (
            abs(line_data_raw[(x + 1) * 3] - BMP_FRONT.r) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 1] - BMP_FRONT.g) < 16 &&
            abs(line_data_raw[(x + 1) * 3 + 2] - BMP_FRONT.b) < 16)
        {
          r = 0b0101;
        }
        else
        {
          r = 0b0110;
        }

        line_data_bmp[k] = (l << 4) | r;
        k++;
      }
    }
    // Write the line to the file
    httpClient.write(line_data_bmp, ((image_width) / 2) * 10);
  }
  return true;
}

/*
//M5Screen8bmp() (dump 8 bits screen to a WiFi client)
bool M5Screen8bmp(){
  uint16_t image_height =display.height();
  uint16_t image_width =display.width();
  uint32_t filesize = 54 + 24 + (image_width * image_height);

  unsigned char line_data_raw[image_width * 3];
  unsigned char line_data_bmp[image_width];

  unsigned char header[54] = {
    'B', 'M',         // BMP signature (Windows 3.1x, 95, NT, …)
    0, 0, 0, 0,       // Image file size in bytes
    0, 0, 0, 0,       // Reserved
    54 + 24, 0, 0, 0, // Start of pixel array
    40, 0, 0, 0,      // Info header size
    0, 0, 0, 0,       // Image width
    0, 0, 0, 0,       // Image height
    1 ,0,             // Number of color planes
    8, 0,             // Bits per pixel
    0, 0, 0, 0,       // Compression
    0, 0, 0, 0,       // Image size (can be 0 for uncompressed images)
    0, 0, 0, 0,       // Horizontal resolution (dpm)
    0, 0, 0, 0,       // Vertical resolution (dpm)
    6, 0, 0, 0,       // Colors in color table (0 = none)
    0, 0, 0, 0 };     // Important color count (0 = all colors are important)

  unsigned char palette[24] = {
    0, 0, 0, 255,
    255, 255, 255, 255,
    192, 192, 192, 255,
    TFT_HEADER.b, TFT_HEADER.g, TFT_HEADER.r, 255,
    TFT_BACK.b, TFT_BACK.g, TFT_BACK.r, 255,
    TFT_FRONT.b, TFT_FRONT.g, TFT_FRONT.r, 255
  };

  // Fill filesize, width and heigth in the header array
  for(uint8_t i = 0; i < 4; i++) {
      header[ 2 + i] = (char)((filesize>>(8 * i)) & 255);
      header[18 + i] = (char)((image_width  >> (8 * i)) & 255);
      header[22 + i] = (char)((image_height >> (8 * i)) & 255);
  }
  // Write header
  httpClient.write(header, 54);

  // Write palette
  httpClient.write(palette, 24);

  // The coordinate origin of a BMP image is at the bottom left.
  // Therefore, the image must be read from bottom to top.
  for(uint16_t y = image_height; y > 0; y--){
    // Get one line of the screen content
   display.readRectRGB(0, y - 1, image_width, 1, line_data_raw);
    // BMP color order is: Blue, Green, Red
    // Return values from readRectRGB is: Red, Green, Blue
    // Therefore: R und B need to be swapped
    //
    // Note the strange euristic algo with abs substract to match palette color
    // Because, for example, TFT_WHITE is #FCFCFC and not #FFFFFF, when you read screen

    for(uint16_t x = 0; x < image_width; x++) {
      if(
        line_data_raw[x * 3] == 0 &&
        line_data_raw[x * 3 + 1] == 0 &&
        line_data_raw[x * 3 + 2] == 0
      ) {
        line_data_bmp[x] = 0;
      }
      else if(
        abs(line_data_raw[x * 3] - 255) < 16 &&
        abs(line_data_raw[x * 3 + 1] - 255) < 16 &&
        abs(line_data_raw[x * 3 + 2] - 255) < 16
      ) {
        line_data_bmp[x] = 1;
      }
      else if(
        abs(line_data_raw[x * 3] - 192) < 16 &&
        abs(line_data_raw[x * 3 + 1] - 192) < 16 &&
        abs(line_data_raw[x * 3 + 2] - 192) < 16
      ) {
        line_data_bmp[x] = 2;
      }
      else if(
        abs(line_data_raw[x * 3] - TFT_HEADER.r) < 16 &&
        abs(line_data_raw[x * 3 + 1] - TFT_HEADER.g) < 16 &&
        abs(line_data_raw[x * 3 + 2] - TFT_HEADER.b) < 16
      ) {
        line_data_bmp[x] = 3;
      }
      else if(
        abs(line_data_raw[x * 3] - TFT_BACK.r) < 16 &&
        abs(line_data_raw[x * 3 + 1] - TFT_BACK.g) < 16 &&
        abs(line_data_raw[x * 3 + 2] - TFT_BACK.b) < 16
      ) {
        line_data_bmp[x] = 4;
      }
      else {
        line_data_bmp[x] = 5;
      }
    }
    // Write the line to the file
    httpClient.write(line_data_bmp, (image_width));
  }
  return true;
}

// M5Screen24bmp (dump 24 bits screen to a WiFi client)
bool M5Screen24bmp(){
  uint16_t image_height =display.height();
  uint16_t image_width =display.width();
  const uint16_t pad = (4 - (3 * image_width) % 4) % 4;
  uint32_t filesize = 54 + (3 * image_width + pad) * image_height;
  unsigned char swap;
  unsigned char line_data[image_width * 3 + pad];
  unsigned char header[54] = {
    'B','M',  // BMP signature (Windows 3.1x, 95, NT, …)
    0,0,0,0,  // Image file size in bytes
    0,0,0,0,  // Reserved
    54,0,0,0, // Start of pixel array
    40,0,0,0, // Info header size
    0,0,0,0,  // Image width
    0,0,0,0,  // Image height
    1,0,      // Number of color planes
    24,0,     // Bits per pixel
    0,0,0,0,  // Compression
    0,0,0,0,  // Image size (can be 0 for uncompressed images)
    0,0,0,0,  // Horizontal resolution (dpm)
    0,0,0,0,  // Vertical resolution (dpm)
    0,0,0,0,  // Colors in color table (0 = none)
    0,0,0,0 };// Important color count (0 = all colors are important)

  // Fill filesize, width and heigth in the header array
  for(uint8_t i = 0; i < 4; i++) {
      header[ 2 + i] = (char)((filesize>>(8 * i)) & 255);
      header[18 + i] = (char)((image_width  >> (8 * i)) & 255);
      header[22 + i] = (char)((image_height >> (8 * i)) & 255);
  }
  // Write the header to the file
  httpClient.write(header, 54);

  // To keep the required memory low, the image is captured line by line
  // initialize padded pixel with 0
  for(uint16_t i = (image_width - 1) * 3; i < (image_width * 3 + pad); i++){
    line_data[i]=0;
  }
  // The coordinate origin of a BMP image is at the bottom left.
  // Therefore, the image must be read from bottom to top.
  for(uint16_t y = image_height; y > 0; y--){
    // Get one line of the screen content
   display.readRectRGB(0, y - 1, image_width, 1, line_data);
    // BMP color order is: Blue, Green, Red
    // Return values from readRectRGB is: Red, Green, Blue
    // Therefore: R und B need to be swapped
    for(uint16_t x = 0; x < image_width; x++){
      swap = line_data[x * 3];
      line_data[x * 3] = line_data[x * 3 + 2];
      line_data[x * 3 + 2] = swap;
    }
    // Write the line to the file
    httpClient.write(line_data, (image_width * 3) + pad);
  }
  return true;
}
*/

// Get screenshot
void getScreenshot()
{
  unsigned long timeout_millis = millis() + 3000;
  String currentLine = "";

  httpClient = httpServer.available();

  if (WiFi.status() == WL_CONNECTED)
  {
    // httpClient.setNoDelay(1);
    if (httpClient)
    {
      // Force a disconnect after 3 seconds
      // Serial.println("New Client.");
      // Loop while the client's connected
      while (httpClient.connected())
      {
        // If the client is still connected after 3 seconds,
        // Something is wrong. So kill the connection
        if (millis() > timeout_millis)
        {
          // Serial.println("Force Client stop!");
          httpClient.stop();
        }
        // If there's bytes to read from the client,
        if (httpClient.available())
        {
          char c = httpClient.read();
          Serial.write(c);
          // If the byte is a newline character
          if (c == '\n')
          {
            // Two newline characters in a row (empty line) are indicating
            // The end of the client HTTP request, so send a response:
            if (currentLine.length() == 0)
            {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line,
              // followed by the content:
              switch (htmlGetRequest)
              {
              case GET_index_page:
              {
                httpClient.println("HTTP/1.1 200 OK");
                httpClient.println("Content-type:text/html");
                httpClient.println();
                if (M5.getBoard() == m5::board_t::board_M5Stack)
                {
                  httpClient.write_P(index_m5stack_html, sizeof(index_m5stack_html));
                }
                else if (M5.getBoard() == m5::board_t::board_M5StackCore2)
                {
                  httpClient.write_P(index_core2_html, sizeof(index_core2_html));
                }
                break;
              }
              case GET_screenshot:
              {
                httpClient.println("HTTP/1.1 200 OK");
                httpClient.println("Content-type:image/bmp");
                httpClient.println();
                if (screensaverMode == 1)
                {
                  M5.Displays(display).wakeup();
                }
                M5Screen4bmp();
                screensaver = millis(); // Screensaver update !!!
                break;
              }
              default:
                httpClient.println("HTTP/1.1 404 Not Found");
                httpClient.println("Content-type:text/html");
                httpClient.println();
                httpClient.print("404 Page not found.<br>");
                break;
              }
              // The HTTP response ends with another blank line:
              // httpClient.println();
              // Break out of the while loop:
              break;
            }
            else
            { // if a newline is found
              // Analyze the currentLine:
              // detect the specific GET requests:
              if (currentLine.startsWith("GET /"))
              {
                htmlGetRequest = GET_unknown;
                // If no specific target is requested
                if (currentLine.startsWith("GET / "))
                {
                  htmlGetRefresh = 3;
                  htmlGetRequest = GET_index_page;
                }
                // If the screenshot image is requested
                if (currentLine.startsWith("GET /screenshot.bmp"))
                {
                  htmlGetRefresh = 3;
                  htmlGetRequest = GET_screenshot;
                }
                // If the button left was pressed on the HTML page
                if (currentLine.startsWith("GET /buttonLeft"))
                {
                  buttonLeftPressed = true;
                  htmlGetRefresh = 1;
                  htmlGetRequest = GET_index_page;
                }
                // If the button center was pressed on the HTML page
                if (currentLine.startsWith("GET /buttonCenter"))
                {
                  buttonCenterPressed = true;
                  htmlGetRefresh = 1;
                  htmlGetRequest = GET_index_page;
                }
                // If the button right was pressed on the HTML page
                if (currentLine.startsWith("GET /buttonRight"))
                {
                  buttonRightPressed = true;
                  htmlGetRefresh = 1;
                  htmlGetRequest = GET_index_page;
                }
              }
              currentLine = "";
            }
          }
          else if (c != '\r')
          {
            // Add anything else than a carriage return
            // character to the currentLine
            currentLine += c;
          }
        }
      }
      // Close the connection
      httpClient.stop();
      // Serial.println("Client Disconnected.");
    }
  }
}

// Manage Wifi and try to reconnect

void checkWifi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    M5.Displays(display).fillCircle(314 + offsetX, 6 + offsetY, 3, TFT_HEADER);
    ping = 2;
  }
  else
  {
    if (ping == 1)
    {
      M5.Displays(display).fillCircle(314 + offsetX, 6 + offsetY, 3, TFT_HEADER);
      M5.Displays(display).drawCircle(314 + offsetX, 6 + offsetY, 3, TFT_WHITE);
    }
    else
    {
      M5.Displays(display).fillCircle(314 + offsetX, 6 + offsetY, 3, TFT_WHITE);
    }
  }
}

// Manage black and white list

void ledAlert(bool type)
{
  boolean qrzDetected = false;
  uint8_t i, j;

  if (type)
  {
    for (i = 0; i < sizeof(qrz) / sizeof(qrz[0]); i += 2)
    {
      if (strstr(indicatifString.c_str(), qrz[i]) != NULL)
      {
        // Serial.println(qrz[i]);
        // Serial.println(i);

        qrzDetected = true;
        break;
      }
    }

    if (qrzDetected)
    {
      for (j = 0; j <= 4; j++)
      {
        leds[j] = (int)strtol(qrz[i + 1], NULL, 0);
        leds[9 - j] = (int)strtol(qrz[i + 1], NULL, 0);
      }
      FastLED.setBrightness(16);
      FastLED.show();
    }
    else
    {
      for (j = 0; j <= 4; j++)
      {
        leds[j] = CRGB::Black;
        leds[9 - j] = CRGB::Black;
      }
      FastLED.setBrightness(16);
      FastLED.show();
    }
  }
  else
  {
    for (j = 0; j <= 4; j++)
    {
      leds[j] = CRGB::Black;
      leds[9 - j] = CRGB::Black;
    }
    FastLED.setBrightness(16);
    FastLED.show();
  }
}