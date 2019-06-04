//include libraries

//I2C library
#include <Wire.h>


//sensor libraries
#include <BH1750.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
BH1750 lux(0x23);


//hash library
#include <MD5.h>

MD5  hashMD5;


//GPS libraries
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;


//display libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//time libraries
#include <TimeLib.h>
#include <SimpleTimer.h>
#include "RTClib.h"

RTC_DS1307 rtc;
SimpleTimer timer;


//memory library
#include <EEPROM.h>

#define EEPROM_SIZE 100


//wifi libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <HTTPClient.h>

WiFiServer server(80);

//define variables

const char *access_point_name = "ControlPost";

String buf[100];
int pos = 0;

int lastDisplaySensor = 0;

String host_name = "http://digcity.000webhostapp.com";
//String host_name = "http://localhost:8000";


//ssd1306 icons
static const unsigned char PROGMEM signal_none[] = {
  0x88, 0x00, 0x3c, 
  0x50, 0x00, 0x24, 
  0x20, 0x07, 0xa4, 
  0x50, 0x04, 0xa4, 
  0x88, 0xf4, 0xa4, 
  0x00, 0x94, 0xa4, 
  0x1e, 0x94, 0xa4, 
  0x12, 0x94, 0xa4, 
  0xd2, 0x94, 0xa4, 
  0x52, 0x94, 0xa4
};

static const unsigned char PROGMEM signal_1[] = {
  0x00, 0x00, 0x3c, 
  0x00, 0x00, 0x24, 
  0x00, 0x07, 0xa4, 
  0x00, 0x04, 0xa4, 
  0x00, 0xf4, 0xa4, 
  0x00, 0x94, 0xa4, 
  0x1e, 0x94, 0xa4, 
  0x12, 0x94, 0xa4, 
  0xd2, 0x94, 0xa4, 
  0xd2, 0x94, 0xa4
};

static const unsigned char PROGMEM signal_2[] = {
  0x00, 0x00, 0x3c, 
  0x00, 0x00, 0x24, 
  0x00, 0x07, 0xa4, 
  0x00, 0x04, 0xa4, 
  0x00, 0xf4, 0xa4, 
  0x00, 0x94, 0xa4, 
  0x1e, 0x94, 0xa4, 
  0x1e, 0x94, 0xa4, 
  0xde, 0x94, 0xa4, 
  0xde, 0x94, 0xa4
};

static const unsigned char PROGMEM signal_3[] = {
  0x00, 0x00, 0x3c, 
  0x00, 0x00, 0x24, 
  0x00, 0x07, 0xa4, 
  0x00, 0x04, 0xa4, 
  0x00, 0xf4, 0xa4, 
  0x00, 0xf4, 0xa4, 
  0x1e, 0xf4, 0xa4, 
  0x1e, 0xf4, 0xa4, 
  0xde, 0xf4, 0xa4, 
  0xde, 0xf4, 0xa4
};

static const unsigned char PROGMEM signal_4[] = {
  0x00, 0x00, 0x3c, 
  0x00, 0x00, 0x24, 
  0x00, 0x07, 0xa4, 
  0x00, 0x07, 0xa4, 
  0x00, 0xf7, 0xa4, 
  0x00, 0xf7, 0xa4, 
  0x1e, 0xf7, 0xa4, 
  0x1e, 0xf7, 0xa4, 
  0xde, 0xf7, 0xa4, 
  0xde, 0xf7, 0xa4
};

static const unsigned char PROGMEM signal_5[] = {
  0x00, 0x00, 0x3c, 
  0x00, 0x00, 0x3c, 
  0x00, 0x07, 0xbc, 
  0x00, 0x07, 0xbc, 
  0x00, 0xf7, 0xbc, 
  0x00, 0xf7, 0xbc, 
  0x1e, 0xf7, 0xbc, 
  0x1e, 0xf7, 0xbc, 
  0xde, 0xf7, 0xbc, 
  0xde, 0xf7, 0xbc
};


//battery level
static const unsigned char PROGMEM battery_none[] = {
  0xff, 0xff, 0xc0, 
  0x80, 0x00, 0x40, 
  0x80, 0x00, 0x40, 
  0x80, 0x00, 0x70, 
  0x80, 0x00, 0x70, 
  0x80, 0x00, 0x70, 
  0x80, 0x00, 0x70, 
  0x80, 0x00, 0x40, 
  0x80, 0x00, 0x40, 
  0xff, 0xff, 0xc0
};

static const unsigned char PROGMEM battery_1[] = {
  0xff, 0xff, 0xc0, 
  0xf8, 0x00, 0x40, 
  0xf8, 0x00, 0x40, 
  0xf8, 0x00, 0x70, 
  0xf8, 0x00, 0x70, 
  0xf8, 0x00, 0x70, 
  0xf8, 0x00, 0x70, 
  0xf8, 0x00, 0x40, 
  0xf8, 0x00, 0x40, 
  0xff, 0xff, 0xc0
};

static const unsigned char PROGMEM battery_2[] = {
  0xff, 0xff, 0xc0, 
  0xff, 0x80, 0x40, 
  0xff, 0x80, 0x40, 
  0xff, 0x80, 0x70, 
  0xff, 0x80, 0x70, 
  0xff, 0x80, 0x70, 
  0xff, 0x80, 0x70, 
  0xff, 0x80, 0x40, 
  0xff, 0x80, 0x40, 
  0xff, 0xff, 0xc0
};

static const unsigned char PROGMEM battery_3[] = {
  0xff, 0xff, 0xc0, 
  0xff, 0xf8, 0x40, 
  0xff, 0xf8, 0x40, 
  0xff, 0xf8, 0x70, 
  0xff, 0xf8, 0x70, 
  0xff, 0xf8, 0x70, 
  0xff, 0xf8, 0x70, 
  0xff, 0xf8, 0x40, 
  0xff, 0xf8, 0x40, 
  0xff, 0xff, 0xc0
};

static const unsigned char PROGMEM battery_4[] = {
  0xff, 0xff, 0xc0, 
  0xff, 0xff, 0xc0, 
  0xff, 0xff, 0xc0, 
  0xff, 0xff, 0xf0, 
  0xff, 0xff, 0xf0, 
  0xff, 0xff, 0xf0, 
  0xff, 0xff, 0xf0, 
  0xff, 0xff, 0xc0, 
  0xff, 0xff, 0xc0, 
  0xff, 0xff, 0xc0
};

//GPS
static const unsigned char PROGMEM GPS[] = {
  0x0c, 0x00, 
  0x1e, 0x00, 
  0x3f, 0x00, 
  0x33, 0x00, 
  0x33, 0x00, 
  0x1e, 0x00, 
  0x1e, 0x00, 
  0x0c, 0x00, 
  0x0c, 0x00, 
  0x00, 0x00
};

//loading
static const unsigned char PROGMEM loading_0[] = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};

static const unsigned char PROGMEM loading_1[] = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
  0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 
  0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 
  0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};

static const unsigned char PROGMEM loading_2[] = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0x3f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
  0x7f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x7f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 
  0x3f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};

static const unsigned char PROGMEM loading_3[] = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x08, 
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x04, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x02, 
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x04, 
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x08, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};

static const unsigned char PROGMEM loading_4[] = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x08, 
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x04, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x02, 
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x04, 
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x08, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};

static const unsigned char PROGMEM loading_5[] = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};







//functions

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) { n = source[i]; i++; if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30; break; } case 0xD1: { n = source[i]; i++; if (n == 0x91) { n = 0xB8; break; } if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  
  return target;
}

void displayLoading(String message, int stage) {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.cp437(true);

  switch (stage) {
    case 0:
      display.drawBitmap(20, 0, loading_0, 88, 10, 2);
      
      break;
    case 1:
      display.drawBitmap(20, 0, loading_1, 88, 10, 2);
      
      break;
    case 2:
      display.drawBitmap(20, 0, loading_2, 88, 10, 2);
      
      break;
    case 3:
      display.drawBitmap(20, 0, loading_3, 88, 10, 2);
      
      break;
    case 4:
      display.drawBitmap(20, 0, loading_4, 88, 10, 2);
      
      break;
    case 5:
      display.drawBitmap(20, 0, loading_5, 88, 10, 2);
      
      break;
  }

  display.setCursor(15, 15);
  display.print(utf8rus(message));

  display.setTextSize(2);
  display.setCursor(12, 40);

  display.print(utf8rus("СОБУРГ 4М"));
  
  display.display();
}

void writeStringEEPROM(char add, String data) {
   int _size = data.length();
   int i;

   for (i = 0; i < _size; i++) {
    EEPROM.write(add + i, data[i]);
   }

   EEPROM.write(add + _size, '\0');
   EEPROM.commit();
}

String readStringEEPROM(char add) {
  int i;
  char data[100];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);


  while(k != '\0' && len < 500) {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }

  data[len] = '\0';
  return String(data);
}

String secret_key() {
  String secret_key = String(EEPROM.read(2));

  return secret_key;
}

String getMacAddress() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}

String hash(String text)
{
  char charBuff[text.length() + 1];

  text.toCharArray(charBuff, text.length() + 1);
  
  String md5str = hashMD5.md5(charBuff);

  return md5str;
}

String makeSignature(String message)
{
  String secretKey = secret_key();
  
  String sign = hash(message + secretKey);
  
  return sign;
}

bool checkSign(String message, String sign)
{
  String expectedSign = makeSignature(message);
  
  Serial.println("checking signature...");
  Serial.println("message: " + message + "  given signature: " + sign + "  expected signature: " + expectedSign);
  
  if (expectedSign == sign) {
    Serial.println("sign correct!");
    
    return true;
  }

  Serial.println("sign incorrect!");
  
  return false;
}

String getTime()
{
  HTTPClient http;

  String chip_id = getMacAddress();
  String message = chip_id + "";

  http.begin("http://digcity.000webhostapp.com/date?message=" + message + "&signature=" + makeSignature(message)); //Specify the URL
  int httpCode = http.GET();                                        //Make the request

  String dateTime = http.getString();

  http.end();
  
  return dateTime;
}

void sendData(String sensor_name, float value, float longitude, float latitude) {
  int httpResponseCode = 1;

  String chip_id = getMacAddress();
  String message = chip_id + "|" + sensor_name + "|" + date() + "|" + value + "|" + getLongitude() + "|" + getLatitude();

  Serial.println("message: " + message);
  Serial.println("signature: " + makeSignature(message));
  
  String requestString = "message=" + message + "&signature=" + makeSignature(message);


  if (WiFi.status() == WL_CONNECTED) {
    for (int i = 0; i <= 5; i++) {
      HTTPClient http;

      http.begin("http://digcity.000webhostapp.com/upload");  //Specify destination for HTTP request
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      httpResponseCode = http.POST(requestString);

      Serial.println("sending " + sensor_name + "(" + httpResponseCode + ") with value " + value);

      if (httpResponseCode == 200) {
        for (int i = 0; i < 99; i++) {
          if (buf[i] != "") {
            HTTPClient http;

            http.begin("http://digcity.000webhostapp.com/upload");  //Specify destination for HTTP request
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");

            httpResponseCode = http.POST(buf[i]);

            if (httpResponseCode == 200) {
              buf[i] = "";
            }
          }
        }

        break;
      }

      delay(1000);
    }
  } else {
  //if (httpResponseCode != 200) {
    saveValue(requestString);
    Serial.println("Request string [" + requestString + "] saved");
  //}
  }
}

float getLatitude() {
  if (gps.location.isValid()) {
    return gps.location.lat();
  } else {
    return 200;
  }
}

float getLongitude() {
  if (gps.location.isValid()) {
    return gps.location.lng();
  } else {
    return 200;
  }
}

String scanI2C(byte address) {
  byte error;

  Wire.beginTransmission(address);
  error = Wire.endTransmission();

  if (error == 0) {
    return "true";
  } else {
    return "false";
  }
}

float read_BH1750() {
  if (scanI2C(35) == "true") {
    Serial.println("BH1750 connected!");

    float lux1 = lux.readLightLevel();

    sendData("BH1750", lux1, 10.5, 11.5);

    if (WiFi.status() == WL_CONNECTED) {
      timer.setTimeout(getPeriodicity("BH1750") * 1000, read_BH1750);
    }
  }
}

void saveValue(String requestString) {
  buf[pos++] = requestString;
  delay(100);
  if (pos == 99) pos = 0;

  //for (int i = 0; i < 99; i++) {
  //  Serial.println("saved request [" + buf[i] + "]");
  //}
}

int getPeriodicity(String sensor_name) {
  String chip_id = getMacAddress();
  String message = chip_id + "|" + sensor_name;

  HTTPClient http;

  http.begin("http://digcity.000webhostapp.com/channels/periodicity?message=" + message + "&signature=" + makeSignature(message)); //Specify the URL
  int httpCode = http.GET();

  String payload = http.getString();

  if (checkSign(payload.substring(0, payload.length() - 32), payload.substring(payload.length() - 32, payload.length()))) {
    int period = payload.substring(0, payload.length() - 32).toInt();

    Serial.println(sensor_name + " request time is: " + period + " sec");

    http.end(); //Free the resources

    return period;
  }

  Serial.println(sensor_name + " request time is: 3600 sec");

  return 3600;
}

void read_BMP180() {
  if (scanI2C(119) == "true") {
    Serial.println("BMP180 connected!");

    float temp = read_BMP180_temperature();
    float pres = read_BMP180_pressure();
  }
}

float read_BMP180_temperature() {
  float temp = bmp.readTemperature();

  sendData("BMP180(temp)", temp, 10.5, 11.5);
  if (WiFi.status() == WL_CONNECTED) {
    timer.setTimeout(getPeriodicity("BMP180(temp)") * 1000, read_BMP180_temperature);
  }

  return temp;
}

float read_BMP180_pressure() {
  float pres = bmp.readPressure() / 101325.0;

  sendData("BMP180(pres)", pres, 10.5, 11.5);

  if (WiFi.status() == WL_CONNECTED) {
    timer.setTimeout(getPeriodicity("BMP180(pres)") * 1000, read_BMP180_pressure);
  }

  return pres;
}

void SetTime() 
{
  String dateTime = getTime();

  Serial.println("Time set on: " + dateTime);

  if (checkSign(dateTime.substring(0, 19), dateTime.substring(19, dateTime.length())) == true) {
    if (scanI2C(104) == "true") {
      rtc.adjust(DateTime(dateTime.substring(0, 4).toInt(), dateTime.substring(5, 7).toInt(), dateTime.substring(8, 10).toInt(), dateTime.substring(11, 13).toInt(), dateTime.substring(14, 16).toInt(), dateTime.substring(17, 19).toInt()));
    }
  } else {
    
  }
}

String date() {
  if (scanI2C(104) == "true") {
    DateTime now = rtc.now();

    String date = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());

    if (String(now.hour()).length() == 1) {
      date = date + " 0" + String(now.hour());
    } else {
      date = date + " " + String(now.hour());
    }

    if (String(now.minute()).length() == 1) {
      date = date + ":0" + String(now.minute());
    } else {
      date = date + ":" + String(now.minute());
    }

    if (String(now.second()).length() == 1) {
      date = date + ":0" + String(now.second());
    } else {
      date = date + ":" + String(now.second());
    }
    
    return date;
  } else {
    String dateTime = getTime();

    String date = String(String(dateTime.substring(0, 4).toInt()) + "-" + String(dateTime.substring(5, 7).toInt()) + "-" + String(dateTime.substring(8, 10).toInt()));

    if (String(dateTime.substring(11, 13).toInt()).length() == 1) {
      date = date + " 0" + String(dateTime.substring(11, 13).toInt());
    } else {
      date = date + " " + String(dateTime.substring(11, 13).toInt());
    }

    if (String(dateTime.substring(14, 16).toInt()).length() == 1) {
      date = date + ":0" + String(dateTime.substring(14, 16).toInt());
    } else {
      date = date + ":" + String(dateTime.substring(14, 16).toInt());
    }

    if (String(dateTime.substring(17, 19).toInt()).length() == 1) {
      date = date + ":0" + String(dateTime.substring(17, 19).toInt());
    } else {
      date = date + ":" + String(dateTime.substring(17, 19).toInt());
    }
  
    return date;
  }
}

int wifi_quality() {
  int rssi = WiFi.RSSI();

  //Serial.print("RSSI:");
  //Serial.println(2 * (rssi + 100));

  return 2 * (rssi + 100);
}

void displaySensors() {
  if (lastDisplaySensor == 3) {
    lastDisplaySensor = 1;
  } else {
    lastDisplaySensor++;
  }

  if (lastDisplaySensor == 1) {    
    if (scanI2C(35) == "true") {
      display.print(int(lux.readLightLevel()));
      display.print(" lux");
    } else {
      lastDisplaySensor++;
    }
  }

  if (lastDisplaySensor == 2) {    
    if (scanI2C(119) == "true") {
      display.print(bmp.readPressure() / 101325.0);
      display.print(" AT");
    } else {
      lastDisplaySensor++;
    }
  }

  if (lastDisplaySensor == 3) {
    if (scanI2C(119) == "true") {
      display.print(bmp.readTemperature());
      display.print(" C");
    } else {
      if (scanI2C(35) == "true") {
        display.print(int(lux.readLightLevel()));
        display.print(" lux");
        lastDisplaySensor = 1;
      } else {
        lastDisplaySensor = 0;
      }
    }
  }
}

void drawSignalLevel()
{
  Serial.println("wifi quality: " + String(wifi_quality()) + "%");
  

  if (WiFi.status() == WL_CONNECTED) {
    if (wifi_quality() < 20) {
       display.drawBitmap(0, 0, signal_1, 24, 10, 2);
    }
    if (wifi_quality() >= 20 && wifi_quality() < 40) {
       display.drawBitmap(0, 0, signal_2, 24, 10, 2);
    }
    if (wifi_quality() >= 40 && wifi_quality() < 60) {
       display.drawBitmap(0, 0, signal_3, 24, 10, 2);
    }
    if (wifi_quality() >= 60 && wifi_quality() < 80) {
       display.drawBitmap(0, 0, signal_4, 24, 10, 2);
    }
    if (wifi_quality() >= 80 && wifi_quality()) {
       display.drawBitmap(0, 0, signal_5, 24, 10, 2);
    }
  } else {
    display.drawBitmap(0, 0, signal_none, 24, 10, 2);
  }  
}

void view() {
  DateTime now = rtc.now();
  gps.encode(Serial2.read());


  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.cp437(true);

  //signal level
  drawSignalLevel();

  //GPS
  if (gps.satellites.value() != 0) {
    display.drawBitmap(95, 0, GPS, 16, 10, 2);
  }

  //battery
  display.drawBitmap(108, 0, battery_none, 24, 10, 2);

  //time

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(35, 15);
  display.cp437(true);
  
  display.print(String(now.hour()));
  display.print(":");
  
  if (String(now.minute()).length() == 1) {
    display.print("0" + String(now.minute()));
  } else {
    display.print(String(now.minute()));
  }

  //sensors

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 40);
  display.cp437(true);

  displaySensors();

  display.display();
}

void connectWiFi() {
  String ssid = readStringEEPROM(4);
  String password = readStringEEPROM(38);

  const char *wifi_name = ssid.c_str();
  const char *wifi_password = password.c_str();
  

  Serial.println("wifi name: " + readStringEEPROM(4));
  Serial.println("wifi password: " + readStringEEPROM(38));

  
  WiFi.begin(wifi_name, wifi_password);

  for (int i = 0; i <= 15; i++) {
    delay(1000);
    Serial.println("Connecting to WiFi..");

    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Can't connect to the WiFi network");

    timer.setTimeout(1800000, connectWiFi);
  } else {
    Serial.println("Connected to the WiFi network");

    SetTime();
  }

  delay(5000);
}

void firstPostOn() {
    bool flag = 0;

    Serial.println("first post on!");

    while (flag == 0) {
      WiFiClient client = server.available();   // listen for incoming clients
      
      if (client) {                             // if you get a client,
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
          if (client.available()) {             // if there's bytes to read from the client,
            char c = client.read();  // read a byte, then
            Serial.write(c);                    // print it out the serial monitor  
            if (c == '\n') {                    // if the byte is a newline character
    
              // if the current line is blank, you got two newline characters in a row.
              // that's the end of the client HTTP request, so send a response:
              if (currentLine.length() == 0) {
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Charset:utf-8");
                client.println();
    
                // the content of the HTTP response follows the header:
                client.print("<head><meta charset='utf-8'><title>СОБУРГ 4М</title></head>");
                client.print("<style> .header { font-family: Nunito, sans-serif; font-weight: 500; line-height: 1.2; } h1 { text-align: center; font-size: 2.25rem; } h2 { margin-left: 10px; font-size: 1.8rem; } input { height: auto; background-clip: padding-box; background-color: rgb(255, 255, 255); border-image-outset: 0px; border-image-repeat: stretch; border-image-slice: 100%; border-image-source: none; border-image-width: 1; border-radius: 4px; border: 1px solid rgb(206, 212, 218); box-sizing: border-box; color: rgb(73, 80, 87); cursor: text; display: inline; font-family: Nunito, sans-serif; font-size: 14.4px; font-stretch: 100%; font-style: normal; font-variant-caps: normal; font-variant-east-asian: normal; font-variant-ligatures: normal; font-variant-numeric: normal; font-weight: 400; letter-spacing: normal; line-height: 23.04px; margin: 0px; overflow-wrap: break-word; overflow-x: visible; overflow-y: visible; padding: 6px 12px; text-align: start; text-indent: 0px; text-rendering: auto; text-shadow: none; text-transform: none; transition-delay: 0s, 0s; transition-duration: 0.15s, 0.15s; transition-property: border-color, box-shadow; transition-timing-function: ease-in-out, ease-in-out; } input[type='submit'] { margin-top: 10px; align-items: flex-start; background-color: rgb(52, 144, 220); border-top-left-radius: 4px; border-top-right-radius: 4px; border: 1px solid rgb(52, 144, 220); color: rgb(255, 255, 255); cursor: pointer; padding-bottom: 6px; padding-left: 12px; padding-right: 12px; text-align: center; transition-property: color, background-color, border-color, box-shadow; user-select: none; vertical-align: middle; white-space: nowrap; word-spacing: 0px; } p { margin-left: 10px; margin-top: 0px; margin-bottom: 0px; font-family: Nunito, sans-serif; font-size: 14.4px; font-weight: 400; line-height: 23.04px; } a { color: rgb(52, 144, 220); cursor: pointer; font-family: Nunito, sans-serif; font-size: 14.4px; font-weight: 400; line-height: 23.04px; } li { margin-left: 10px; box-sizing: border-box; color: rgb(33, 37, 41); display: list-item; font-family: Nunito, sans-serif; font-size: 14.4px; font-weight: 400; line-height: 23.04px; list-style-type: decimal; text-align: left; } form .form-list li { list-style: none; } form .form-list li * { display: inline; margin: 5px; } form .form-list li input { padding: 1vh; margin: 5px; font-size: 2vh; }</style>");
                client.print("<h1 class='header'>Пост контроля</h1><p>Это страница вашего поста контроля. Для его успешной работы вы должны ввести данные вашей wifi сети, чтобы пост мог передавать данные на сервер, и добавить этот пост на <a href='" + host_name + "'>сайт системы СОБУРГ4М</a></p><h2 class='header'>Данные wifi сети</h2><form method='GET' action='/'> <ul class='form-list'> <li> <p>Имя wifi сети <input type='text' id='wifi_name' name='wifi_name' placeholder='ведите имя сети'></p> </li> <li> <p>Пароль wifi сети</p> <input type='password' id='wifi_password' name='wifi_password' placeholder='введите пароль'> </li> <input type='hidden' value='true' name='end'> <li> <input type='submit' value='Отправить'> </li> </ul></form><h2 class='header'>Добавление поста на сайт СОБУРГ 4М</h2><p>Для добавления поста контроля на сайт:</p><ol> <li>Подключитесь к интернету с доступом к сети</li> <li>Перейдите по ссылке <a href=''></a> </li></ol>");
    
                // The HTTP response ends with another blank line:
                client.println();
                // break out of the while loop:
                break;
              } else {    // if you got a newline, then clear currentLine:
                currentLine = "";
              }
            } else if (c != '\r') {  // if you got anything else but a carriage return character,
              currentLine += c;      // add it to the end of the currentLine
            }

            Serial.println(currentLine);
            
            if (currentLine.endsWith("&end=true") && currentLine.startsWith("Referer")) {
              int wifi_name_start_index = currentLine.indexOf("wifi_name=");
              int wifi_password_start_index = currentLine.indexOf("wifi_password=");
              
              String wifi_name = currentLine.substring(wifi_name_start_index + 10, wifi_password_start_index - 1);
              String wifi_password = currentLine.substring(wifi_password_start_index + 14, currentLine.length() - 9);

              writeStringEEPROM(4, wifi_name);
              writeStringEEPROM(38, wifi_password);

              flag = 1;
            }
          }
        }
        // close the connection:
        client.stop();
      }
    }
}

void checkBootButton() {
  if (digitalRead(0) == 0) {
    Serial.println("restarting...");
    

    display.clearDisplay();
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 22);
    display.cp437(true);
    
    display.print(utf8rus("Перезагрузка..."));

    display.display();    
    
    EEPROM.write(3, 0);

    for (int i = 4; i < 55; i++) {
      EEPROM.write(0, i);
    }

   EEPROM.commit();

    delay(2500);

    ESP.restart();
  }
}







//setup

void setup() {  
  Serial.begin(115200);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.setRotation(2);

  display.display();

  display.clearDisplay();

  displayLoading("", 0);

  delay(2000);


  //loading 1 - GPS begin
  displayLoading("включение GPS...", 1);
  
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  
  //loading 2 - memory begin
  displayLoading("запуск памяти...", 2);
  
  EEPROM.begin(EEPROM_SIZE);

  Serial.print("Secret key: ");
  Serial.println(secret_key());
  
  Serial.print("Mac address: ");
  Serial.println(getMacAddress());

  delay(4000);

  rtc.begin();

  displayLoading("инициализация датчиков...", 3);
  bmp.begin();
  Wire.begin();
  lux.begin();

  //loading 4 - wifi begin
  displayLoading("запуск WiFi...", 4);
  
  if (EEPROM.read(3) != 1) {
    Serial.println();
    Serial.println("Configuring access point...");

    String password = String(random(10000000, 99999999));
    const char *access_point_password = password.c_str();

    Serial.println("access point password: " + String(access_point_password));
    
    WiFi.softAP(access_point_name, access_point_password);


    //display password on ssd1306
    display.clearDisplay();
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 0);
    display.cp437(true);
    
    display.print(utf8rus("Пароль для подключения к точке доступа:"));

    display.setTextSize(2);
    display.setCursor(15, 25);
    display.print(String(access_point_password));

    display.display();
    
    
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();
  
    Serial.println("Server started");
    
    firstPostOn();
    
    EEPROM.write(3, 1);
    EEPROM.commit();

    ESP.restart();
  } else {
    connectWiFi();
  }

  //loading 5 - starting
  displayLoading("опрос датчиков...", 5);
  
  read_BMP180();
  read_BH1750();
}







//program

void loop() {
  checkBootButton();
  
  timer.run();

  view();

  delay(2500);
}
