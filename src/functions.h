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

// M5Screen2bmp (Dump the screen to a WiFi client)

bool M5Screen2bmp(WiFiClient &client)
{
  int image_height = M5.Lcd.height();
  int image_width = M5.Lcd.width();
  const uint pad = (4-(3*image_width)%4)%4;
  int start = (image_width-1) * 3;
  int stop = (image_width * 3 + pad);
  uint filesize = 54+(3*image_width+pad)*image_height; 
  unsigned char header[54] = { 
    'B','M',  // BMP signature (Windows 3.1x, 95, NT, …)
    0,0,0,0,  // image file size in bytes
    0,0,0,0,  // reserved
    54,0,0,0, // start of pixel array
    40,0,0,0, // info header size
    0,0,0,0,  // image width
    0,0,0,0,  // image height
    1,0,      // number of color planes
    24,0,     // bits per pixel
    0,0,0,0,  // compression
    0,0,0,0,  // image size (can be 0 for uncompressed images)
    0,0,0,0,  // horizontal resolution (dpm)
    0,0,0,0,  // vertical resolution (dpm)
    0,0,0,0,  // colors in color table (0 = none)
    0,0,0,0 };// important color count (0 = all colors are important)

  // Fill filesize, width and heigth in the header array
  for(uint i = 0; i < 4; i++) {
      header[ 2+i] = (char)((filesize>>(8*i))&255);
      header[18+i] = (char)((image_width   >>(8*i))&255);
      header[22+i] = (char)((image_height  >>(8*i))&255);
  }
  // Write the header to the file
  client.write(header, 54);
  
  // To keep the required memory low, the image is captured line by line
  unsigned char line_data[image_width*3+pad];
  // Initialize padded pixel with 0 
  for(int i = start; i < stop; i++){
    line_data[i]=0;
  }
  // The coordinate origin of a BMP image is at the bottom left.
  // Therefore, the image must be read from bottom to top.
  for(int y = image_height; y > 0; y--){
    // Get one line of the screen content
    M5.Lcd.readRectRGB(0, y-1, image_width, 1, line_data);
    // BMP color order is: Blue, Green, Red
    // Return values from readRectRGB is: Red, Green, Blue
    // Therefore: R und B need to be swapped
    for(int x = 0; x < image_width; x++){
      unsigned char r_buff = line_data[x * 3];
      line_data[x * 3] = line_data[x * 3 + 2];
      line_data[x * 3 + 2] = r_buff;
    }
    // Write the line to the file
    client.write(line_data, stop);
  }
  return true;
}

// Get screenshot

void getScreenshot()
{
  // Check if WIFI is still connected
  // If the WIFI is not connected (anymore)
  // A reconnect is triggert
  wl_status_t wifi_Status = WiFi.status();
  // Check if WIFI is connected
  // Needed because of the above mentioned reconnection attempt
  wifi_Status = WiFi.status();
  if(wifi_Status == WL_CONNECTED){
    // Check for incoming clients
    WiFiClient client = server.available(); 
    if (client) {  
      // Force a disconnect after 2 seconds
      unsigned long timeout_millis = millis()+2000;
      Serial.println("New Client.");  
      // A String to hold incoming data from the client line by line        
      String currentLine = "";                
      // Loop while the client's connected
      while (client.connected()) { 
        // If the client is still connected after 2 seconds,
        // Something is wrong. So kill the connection
        if(millis() > timeout_millis){
          Serial.println("Force Client stop!");  
          client.stop();
        } 
        // If there's bytes to read from the client,
        if (client.available()) {             
          char c = client.read();            
          Serial.write(c);    
          // If the byte is a newline character             
          if (c == '\n') {    
            // Uwo newline characters in a row (empty line) are indicating
            // The end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line,
              // followed by the content:
              switch (htmlGetRequest)
              {
                case GET_index_page: {
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                  client.println();
                  client.write_P(index_html, sizeof(index_html));
                  break;
                }
                case GET_screenshot: {              
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:image/bmp");
                  client.println();
                  M5Screen2bmp(client);
                  break;
                }
                default:
                  client.println("HTTP/1.1 404 Not Found");
                  client.println("Content-type:text/html");
                  client.println();
                  client.print("404 Page not found.<br>");
                  break;
              }
              // The HTTP response ends with another blank line:
              client.println();
              // Break out of the while loop:
              break;
            } else {    // if a newline is found
              // Analyze the currentLine:
              // detect the specific GET requests:
              if(currentLine.startsWith("GET /")){
                htmlGetRequest = GET_unknown;
                // If no specific target is requested
                if(currentLine.startsWith("GET / ")){
                  htmlGetRequest = GET_index_page;
                }
                // If the screenshot image is requested
                if(currentLine.startsWith("GET /screenshot.bmp")){
                  htmlGetRequest = GET_screenshot;
                }
                // If the button left was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonLeft")){
                  buttonLeftPressed = true;
                  htmlGetRequest = GET_index_page;
                }
                // If the button center was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonCenter")){
                  buttonCenterPressed = true;
                  htmlGetRequest = GET_index_page;
                }
                // If the button right was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonRight")){
                  buttonRightPressed = true;
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
      // Close the connection:
      client.stop();
      Serial.println("Client Disconnected.");
    }
  }
}