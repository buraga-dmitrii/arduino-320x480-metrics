// User_Setup.h

// #define ILI9486_DRIVER
// #define ESP32_PARALLEL

// #define TFT_CS   33  // Chip select control pin (library pulls permanently low
// #define TFT_DC   15  // Data Command control pin - must use a pin in the range 0-31
// #define TFT_RST  32  // Reset pin, toggles on startup

// #define TFT_WR    4  // Write strobe control pin - must use a pin in the range 0-31
// #define TFT_RD    2  // Read strobe control pin

// #define TFT_D0   12  // Must use pins in the range 0-31 for the data bus
// #define TFT_D1   13  // so a single register write sets/clears all bits.
// #define TFT_D2   26  // Pins can be randomly assigned, this does not affect
// #define TFT_D3   25  // TFT screen update performance.
// #define TFT_D4   18
// #define TFT_D5   19
// #define TFT_D6   27
// #define TFT_D7   14
// #define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
// #define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
// #define SMOOTH_FONT

#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(8, 23, NEO_GRB + NEO_KHZ800);
TFT_eSPI tft = TFT_eSPI();

const char* ssid     = "usr";
const char* password = "pass";

void setup()
{
  pixels.begin();
  pixels.setBrightness(30);
  pixels.clear();
  
  tft.init();
  tft.setRotation(1);
  tft.setTextColor(0x0000); 

  tft.drawLine(0, 106, 480, 106, 0x0000);   tft.drawLine(0, 107, 480, 107, 0x0000);
  tft.drawLine(0, 212, 480, 212, 0x0000);   tft.drawLine(0, 213, 480, 213, 0x0000);
  tft.drawLine(240, 0, 240, 212, 0x0000);   tft.drawLine(241, 0, 241, 212, 0x0000);
  tft.drawLine(160, 212, 160, 320, 0x0000); tft.drawLine(161, 212, 161, 320, 0x0000);
  tft.drawLine(320, 212, 320, 320, 0x0000); tft.drawLine(321, 212, 321, 320, 0x0000);

  tft.setTextFont(4); tft.setCursor(80, 15);   tft.println("AVG DB");
  tft.setTextFont(4); tft.setCursor(320, 15);  tft.println("AVG DU");
  tft.setTextFont(4); tft.setCursor(80, 125);  tft.println("MAX DB");
  tft.setTextFont(4) ;tft.setCursor(320, 125); tft.println("MAX DU");
  tft.setTextFont(4); tft.setCursor(60, 220);  tft.println("2xx");
  tft.setTextFont(4); tft.setCursor(220, 220); tft.println("4xx");
  tft.setTextFont(4); tft.setCursor(380, 220); tft.println("5xx");

  Serial.begin(115200);
  delay(10);
    
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
}

void loop()
{ 
    pixels.setBrightness(30);
    HTTPClient http;

    String urls[4] = { 
      "https://api.myjson.com/bins/12iv9r", 
      "https://api.myjson.com/bins/1f0yz3", 
      "https://api.myjson.com/bins//sehzj", 
      "https://api.myjson.com/bins/g56n3" 
    };
    
    http.begin(urls[random(4)]);
    int httpCode = http.GET(); 

    if (httpCode > 0) {
      
        String response = http.getString();
        Serial.println(httpCode);
        Serial.println(response);

        const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + 400;
        DynamicJsonDocument json(capacity);
      
        DeserializationError error = deserializeJson(json, response);
        
        if (!error) {
          String avg_db = json["metrics"]["avg_db"];
          String avg_du = json["metrics"]["avg_duration"];
          String max_db = json["metrics"]["max_db"];
          String max_du = json["metrics"]["max_duration"];
          
          String two_xx  = json["metrics"]["statuses"]["grouped"]["2xx"];
          String four_xx = json["metrics"]["statuses"]["grouped"]["4xx"];
          String five_xx = json["metrics"]["statuses"]["grouped"]["5xx"];

          tft.setTextFont(6); tft.setCursor(50, 50);   tft.fillRect(44, 44, 150, 50, 0xffff);   tft.println(avg_db.toInt());
          tft.setTextFont(6); tft.setCursor(290, 50);  tft.fillRect(284, 44, 150, 50, 0xffff);  tft.println(avg_du.toInt());
          tft.setTextFont(6); tft.setCursor(50, 160);  tft.fillRect(44, 154, 150, 50, 0xffff);  tft.println(max_db.toInt());
          tft.setTextFont(6); tft.setCursor(290, 160); tft.fillRect(284, 154, 150, 50, 0xffff); tft.println(max_du.toInt());
          
          tft.setTextFont(6); tft.setCursor(10, 260);  tft.fillRect(4, 254, 152, 50, 0xffff);   tft.println(two_xx.toInt());
          tft.setTextFont(6); tft.setCursor(170, 260); tft.fillRect(165, 254, 151, 50, 0xffff); tft.println(four_xx.toInt());
          tft.setTextFont(6); tft.setCursor(330, 260); tft.fillRect(325, 254, 151, 50, 0xffff); tft.println(five_xx.toInt());    

          pixels.clear();

          if (two_xx.toInt() >= 1000 ) { addNextPixel(0, 0, 150, 0); } // 1
          if (two_xx.toInt() >= 2000 ) { addNextPixel(1, 0, 150, 0); } // 2
          if (two_xx.toInt() >= 3000 ) { addNextPixel(2, 0, 150, 0); } // 3
          if (two_xx.toInt() >= 4000 ) { addNextPixel(3, 0, 150, 0); } // 4
          if (two_xx.toInt() >= 5000 ) { addNextPixel(4, 0, 150, 0); } // 5
          if (two_xx.toInt() >= 6000 ) { addNextPixel(5, 0, 150, 0); } // 6

          if (five_xx.toInt() > 0 || max_db.toInt() >= 500)  {
            pixels.setBrightness(100); 
            for(int i=0; i<10; i++) {
              if ((i % 2) == 0) { 
                if (five_xx.toInt() > 0)   { pixels.setPixelColor(7, pixels.Color(150, 0, 0)); }
                if (max_db.toInt() >= 500) { pixels.setPixelColor(6, pixels.Color(0, 0, 0));   }
              } else {
                if (five_xx.toInt() > 0)   { pixels.setPixelColor(7, pixels.Color(0, 0, 0)); }
                if (max_db.toInt() >= 500) { pixels.setPixelColor(6, pixels.Color(250, 0, 255)); }
              }
              pixels.show();
              delay(300);
            }

            if (five_xx.toInt() > 0)   { pixels.setPixelColor(7, pixels.Color(150, 0, 0)); }
            if (max_db.toInt() >= 500) { pixels.setPixelColor(6, pixels.Color(250, 0, 255)); }
            
            pixels.setBrightness(30);
            pixels.show();
          }
        }  
    } 

   http.end();
   
   delay(1000);
}

void addNextPixel(int pos, int r, int g, int b){
  for(int i=0; i<=pos; i++)  { pixels.setPixelColor(i, pixels.Color(r, g, b)); pixels.show(); }           
  for(int i=pos+1; i<8; i++) { pixels.setPixelColor(i, pixels.Color(0,0,0)); pixels.show();   }            
}
