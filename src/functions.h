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
  String option = String(menu[menuCurrent]);

  // Reset
  message = "";
  //M5.lcd.clear();
  resetColor();

  if(menuMode != 1 || (menuMode == 1 && option == "COULEUR")) {
    // Header
    M5.Lcd.fillRect(0, 0, 320, 44, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
    M5.Lcd.drawFastHLine(  0,   0, 320, TFT_WHITE);
    M5.Lcd.drawFastHLine(  0,  44, 320, TFT_WHITE);
  }

  // Grey zone
  M5.Lcd.drawFastHLine(  0, 100, 320, TFT_WHITE);
  M5.Lcd.fillRect(0, 101, 320, 239, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

  if(modeCurrent == 0) {
    // Elsewhere
    M5.Lcd.fillRoundRect(0, 154, 155, 85, 4, TFT_WHITE);
    M5.Lcd.fillRoundRect(1, 155, 26, 83, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));

    for (uint8_t i = 24; i < 27; i++) {
      M5.Lcd.drawFastVLine(i, 155, 83, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
    }

    M5.Lcd.drawFastVLine(27, 155, 83, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.drawFastVLine(100, 155, 83, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

    for (uint8_t i = 0; i < 5; i++) {
      M5.Lcd.drawFastHLine(2, 168 + (i * 14), 152, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    }

    // Log
    M5.Lcd.fillRoundRect(160, 117, 160, 122, 4, TFT_WHITE);
  }
  else if(modeCurrent == 1) {
    for (uint8_t i = 0; i <= 1; i++) {
      M5.Lcd.fillRoundRect(0 + (170 * i), 125, 150, 115, 4, TFT_WHITE);
      M5.Lcd.fillRoundRect(1 + (170 * i), 126, 148, 113, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      M5.Lcd.fillRect(51 + (170 * i), 126, 98, 113, TFT_WHITE);
      M5.Lcd.drawFastVLine(50 + (170 * i), 126, 113, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

      for(uint8_t k = 1; k <= 5; k++) {
        M5.Lcd.drawFastHLine(1 + (170 * i), 125 + (k * 18) + k, 148, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      }
    }
  }
  else {
    for (uint8_t i = 0; i <= 2; i++) {
      for (uint8_t j = 0; j <= 2; j++) {
        M5.Lcd.fillRoundRect(0 + (110 * j), 104 + (48 * i), 100, 40, 4, TFT_WHITE);
        M5.Lcd.fillRoundRect(1 + (110 * j), 105 + (48 * i), 98, 38, 4, M5.Lcd.color565(TFT_FRONT.r, TFT_FRONT.g, TFT_FRONT.b));
      }
    }
  }
}

// Scroll
void scroll(uint8_t pause)
{
  uint16_t h = 18;
  uint16_t w;
  uint16_t limit =  M5.Lcd.width();
  static uint32_t timer = 0, wait = 0; 
  
  timer = millis();

  w = Sprite.textWidth(message) + 40;
  if (w < limit)
  {
    w = limit;
  }
  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    Sprite.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics  
  Sprite.setTextColor(TFT_WHITE); // White text, no background colour
  Sprite.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  Sprite.drawString(message, pos, 2);
  Sprite.drawString(message, pos - w, 2);

  Sprite.pushSprite(0, 78);

  pos -= 1;
  if (pos == 0)
  {
    pos = w;
  }

  wait = millis() - timer;
  if (wait < pause)
  {
    vTaskDelay(pdMS_TO_TICKS(pause - wait));
    //Serial.println(pause - wait);
  }
}

// Detect rotation
void checkAcceleration()
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
void updateLocalTime()
{
  char timeStringBuff[10];  //10 chars should be enough
  char utcStringBuff[10];   //10 chars should be enough

  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
  strftime(utcStringBuff, sizeof(utcStringBuff), "%z", &timeinfo);

  sscanf(utcStringBuff, "%d", &utc);
  utc = utc / 100;

  //Serial.println(utc);

  dateString = String(timeStringBuff);
}

//M5Screen4bmp() (dump 4 bits screen to a WiFi client)
bool M5Screen4bmp(){
  uint16_t image_height = M5.Lcd.height();
  uint16_t image_width = M5.Lcd.width();
  uint32_t filesize = 54 + 24 + ((image_width * image_height) / 2); 
  uint8_t l = 0;
  uint8_t r = 0;

  unsigned char line_data_raw[image_width * 3];
  unsigned char line_data_bmp[(image_width / 2) * 10];

  unsigned char header[54] = { 
    'B', 'M',         // BMP signature (Windows 3.1x, 95, NT, …)
    0, 0, 0, 0,       // Image file size in bytes
    0, 0, 0, 0,       // Reserved
    54 + 24, 0, 0, 0, // Start of pixel array
    40, 0, 0, 0,      // Info header size
    0, 0, 0, 0,       // Image width
    0, 0, 0, 0,       // Image height
    1 ,0,             // Number of color planes
    4, 0,             // Bits per pixel
    0, 0, 0, 0,       // Compression
    0, 0, 0, 0,       // Image size (can be 0 for uncompressed images)
    0, 0, 0, 0,       // Horizontal resolution (dpm)
    0, 0, 0, 0,       // Vertical resolution (dpm)
    6, 0, 0, 0,       // Colors in color table (0 = none)
    0, 0, 0, 0 };     // Important color count (0 = all colors are important)

  unsigned char palette[24] = {
    0, 0, 0, 255,
    252, 252, 252, 255,
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
  for(uint16_t y = image_height; y > 0; y-= 10){
    uint16_t k = 0;

    for(uint8_t z = 0; z < 10; z++) {
      // Get one line of the screen content
      M5.Lcd.readRectRGB(0, y - 1 - z, image_width, 1, line_data_raw);
      // BMP color order is: Blue, Green, Red
      // Return values from readRectRGB is: Red, Green, Blue
      // Therefore: R und B need to be swapped
      //
      // Note the strange euristic algo with abs substract to match palette color
      // Because, for example, TFT_WHITE is #FCFCFC and not #FFFFFF, when you read screen
      for(uint16_t x = 0; x < image_width; x += 2) {
        // Left
        if( 
          line_data_raw[x * 3] == 0 && 
          line_data_raw[x * 3 + 1] == 0 && 
          line_data_raw[x * 3 + 2] == 0
        ) {
          l = 0b0000;
        }
        else if(
          abs(line_data_raw[x * 3] - 255) < 16 && 
          abs(line_data_raw[x * 3 + 1] - 255) < 16 && 
          abs(line_data_raw[x * 3 + 2] - 255) < 16
        ) {
          l = 0b0001;
        }
        else if(
          abs(line_data_raw[x * 3] - 192) < 16 && 
          abs(line_data_raw[x * 3 + 1] - 192) < 16 && 
          abs(line_data_raw[x * 3 + 2] - 192) < 16
        ) {
          l = 0b0010;
        }
        else if(
          abs(line_data_raw[x * 3] - TFT_HEADER.r) < 16 && 
          abs(line_data_raw[x * 3 + 1] - TFT_HEADER.g) < 16 && 
          abs(line_data_raw[x * 3 + 2] - TFT_HEADER.b) < 16
        ) {
          l = 0b0011;
        }
        else if(
          abs(line_data_raw[x * 3] - TFT_BACK.r) < 16 && 
          abs(line_data_raw[x * 3 + 1] - TFT_BACK.g) < 16 && 
          abs(line_data_raw[x * 3 + 2] - TFT_BACK.b) < 16
        ) {
          l = 0b0100;
        }
        else {
          l = 0b0101;
        }

        // Right
        if(
          line_data_raw[(x + 1) * 3] == 0 && 
          line_data_raw[(x + 1) * 3 + 1] == 0 && 
          line_data_raw[(x + 1) * 3 + 2] == 0
        ) {
          r = 0b0000;
        }
        else if(
          abs(line_data_raw[(x + 1) * 3] - 255) < 16 && 
          abs(line_data_raw[(x + 1) * 3 + 1] - 255) < 16 && 
          abs(line_data_raw[(x + 1) * 3 + 2] - 255) < 16
        ) {
          r = 0b0001;
        }
        else if(
          abs(line_data_raw[(x + 1) * 3] - 192) < 16 && 
          abs(line_data_raw[(x + 1) * 3 + 1] - 192) < 16 && 
          abs(line_data_raw[(x + 1) * 3 + 2] - 192) < 16
        ) {
          r = 0b0010;
        }
        else if(
          abs(line_data_raw[(x + 1) * 3] - TFT_HEADER.r) < 16 && 
          abs(line_data_raw[(x + 1) * 3 + 1] - TFT_HEADER.g) < 16 && 
          abs(line_data_raw[(x + 1) * 3 + 2] - TFT_HEADER.b) < 16
        ) {
          r = 0b0011;
        }
        else if(
          abs(line_data_raw[(x + 1) * 3] - TFT_BACK.r) < 16 &&
          abs(line_data_raw[(x + 1) * 3 + 1] - TFT_BACK.g) < 16 &&
          abs(line_data_raw[(x + 1) * 3 + 2] - TFT_BACK.b) < 16
        ) {
          r = 0b0100;
        }
        else {
          r = 0b0101;
        }

        line_data_bmp[k] = (l<<4) | r;
        k++;
      }
    }
    // Write the line to the file
    httpClient.write(line_data_bmp, ((image_width) / 2) * 10);
  }
  return true;
}

//M5Screen8bmp() (dump 8 bits screen to a WiFi client)
bool M5Screen8bmp(){
  uint16_t image_height = M5.Lcd.height();
  uint16_t image_width = M5.Lcd.width();
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
    252, 252, 252, 255,
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
    M5.Lcd.readRectRGB(0, y - 1, image_width, 1, line_data_raw);
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
  uint16_t image_height = M5.Lcd.height();
  uint16_t image_width = M5.Lcd.width();
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
    M5.Lcd.readRectRGB(0, y - 1, image_width, 1, line_data);
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

// Get screenshot
void getScreenshot()
{
  unsigned long timeout_millis = millis() + 3000;
  String currentLine = "";                

  httpClient = httpServer.available(); 

  if(WiFi.status() == WL_CONNECTED){
    //httpClient.setNoDelay(1);
    if (httpClient) {  
      // Force a disconnect after 3 seconds
      Serial.println("New Client.");  
      // Loop while the client's connected
      while (httpClient.connected()) { 
        // If the client is still connected after 3 seconds,
        // Something is wrong. So kill the connection
        if(millis() > timeout_millis){
          Serial.println("Force Client stop!");  
          httpClient.stop();
        } 
        // If there's bytes to read from the client,
        if (httpClient.available()) {             
          char c = httpClient.read();
          Serial.write(c);
          // If the byte is a newline character             
          if (c == '\n') {    
            // Two newline characters in a row (empty line) are indicating
            // The end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line,
              // followed by the content:
              switch (htmlGetRequest)
              {
                case GET_index_page: {
                  httpClient.println("HTTP/1.1 200 OK");
                  httpClient.println("Content-type:text/html");
                  httpClient.println();
                  httpClient.write_P(index_html, sizeof(index_html));
                  break;
                }
                case GET_screenshot: {
                  httpClient.println("HTTP/1.1 200 OK");
                  httpClient.println("Content-type:image/bmp");
                  httpClient.println();
                  if(screensaverMode == 1) {
                    M5.Lcd.wakeup();
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
            } else {    // if a newline is found
              // Analyze the currentLine:
              // detect the specific GET requests:
              if(currentLine.startsWith("GET /")){
                htmlGetRequest = GET_unknown;
                // If no specific target is requested
                if(currentLine.startsWith("GET / ")){
                  htmlGetRefresh = 3;
                  htmlGetRequest = GET_index_page;
                }
                // If the screenshot image is requested
                if(currentLine.startsWith("GET /screenshot.bmp")){
                  htmlGetRefresh = 3;
                  htmlGetRequest = GET_screenshot;
                }
                // If the button left was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonLeft")){
                  buttonLeftPressed = true;
                  htmlGetRefresh = 1;
                  htmlGetRequest = GET_index_page;
                }
                // If the button center was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonCenter")){
                  buttonCenterPressed = true;
                  htmlGetRefresh = 1;
                  htmlGetRequest = GET_index_page;
                }
                // If the button right was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonRight")){
                  buttonRightPressed = true;
                  htmlGetRefresh = 1;
                  htmlGetRequest = GET_index_page;
                }
              }
              currentLine = "";
            }
          } else if (c != '\r') {  
            // Add anything else than a carriage return
            // character to the currentLine 
            currentLine += c;      
          }
        }
      }
      // Close the connection
      httpClient.stop();
      Serial.println("Client Disconnected.");
    }
  }
}

// Manage Wifi and try to reconnect

void checkWifi() {
  if (screensaverMode == 1) {
    if(WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      WiFi.reconnect();
      delay(30 * 1000);
    }
  }
}

// Manage black and white list

void ledAlert(bool type) {
  boolean qrzDetected = false;
  uint8_t i, j;

  if(type) {
    for(i = 0; i < sizeof(qrz) / sizeof(qrz[0]); i += 2)
    {
      if(strstr(indicatifString.c_str(), qrz[i]) != NULL) {
        Serial.println(qrz[i]);
        Serial.println(i);
    
        qrzDetected = true;
        break;
      }
    }

    if(qrzDetected) {
      for(j = 0; j <= 4 ; j++){
        leds[j] = (int)strtol(qrz[i + 1], NULL, 0);
        leds[9 - j] = (int)strtol(qrz[i + 1], NULL, 0);
      }
      FastLED.setBrightness(16);
      FastLED.show();
    }
    else {
      for(j = 0; j <= 4 ; j++){
        leds[j] = CRGB::Black;
        leds[9 - j] = CRGB::Black;
      }
      FastLED.setBrightness(16);
      FastLED.show();
    }
  }
  else {
    for(j = 0; j <= 4 ; j++){
      leds[j] = CRGB::Black;
      leds[9 - j] = CRGB::Black;
    }
    FastLED.setBrightness(16);
    FastLED.show();
  }
}