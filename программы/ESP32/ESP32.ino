//include libraries

#include <TimeLib.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <BH1750.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_BMP085.h>
#include "RTClib.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//define variables
Adafruit_BMP085 bmp;
BH1750 lux(0x23);
RTC_DS1307 rtc;
TinyGPSPlus gps;

SimpleTimer timer;

//const char* ssid = "tata2";
//const char* password =  "QazxsWedcvfRt";

//const char* ssid = "iPhone (Михаил)";
//const char* password = "MishA2005";

const char* ssid = "WiFiNET";
const char* password = "QazxsWEdcvfRTY";

String buf[100];
int pos = 0;

int lastDisplaySensor = 0;










//functions

void sendData(String sensor_name, float value, float longitude, float latitude) {
  int httpResponseCode = 1;

  String chip_id = getMacAddress();
  String requestString = "mac_address=" + chip_id + "&sensor_name=" + sensor_name + "&date=" + date() + "&value=" + value + "&longitude=" + getLongitude() + "&latitude=" + getLatitude();


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

String getMacAddress() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
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

  HTTPClient http;

  http.begin("http://digcity.000webhostapp.com/channels/periodicity?mac_address=" + chip_id + "&sensor_name=" + sensor_name); //Specify the URL
  int httpCode = http.GET();                                        //Make the request

  String payload = http.getString();
  int period = payload.toInt();

  Serial.println(payload);

  http.end(); //Free the resources

  return period;
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

void SetTime() {
  HTTPClient http;

  http.begin("http://digcity.000webhostapp.com/date"); //Specify the URL
  int httpCode = http.GET();                                        //Make the request

  String dateTime = http.getString();

  rtc.adjust(DateTime(dateTime.substring(0, 4).toInt(), dateTime.substring(5, 7).toInt(), dateTime.substring(8, 10).toInt(), dateTime.substring(11, 13).toInt(), dateTime.substring(14, 16).toInt(), dateTime.substring(17, 19).toInt()));

  http.end(); //Free the resources
}

String date() {
  DateTime now = rtc.now();

  String date = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  return date;
}

int wifi_quality() {
  int rssi = WiFi.RSSI();

  //Serial.print("RSSI:");
  //Serial.println(2 * (rssi + 100));

  return 2 * (rssi + 100);
}

void view() {
  DateTime now = rtc.now();
  gps.encode(Serial2.read());



  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.print("wifi:");

  if (WiFi.status() == WL_CONNECTED) {
    display.print(wifi_quality());
    display.print("%");
  } else {
    display.print("x");
  }

  display.setCursor(55, 0);

  display.print("gps sats:");
  display.print(String(gps.satellites.value()));

  //time

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(35, 15);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.print(String(now.hour()));
  display.print(":");
  display.print(String(now.minute()));

  //sensors

  display.setTextSize(3);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 40);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  displaySensors();


  display.display();
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
      lastDisplaySensor = 1;
    }
  }
}


void connectWiFi() {
  WiFi.begin(ssid, password);

  for (int i = 0; i <= 15; i++) {
    //while (WiFi.status() != WL_CONNECTED) {
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
    Serial.println(date());
  }

  delay(5000);
}










//setup

void setup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.setRotation(2);

  display.display();
  delay(2000);

  display.clearDisplay();
  
  
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  delay(4000);

  rtc.begin();
  bmp.begin();
  Wire.begin();
  lux.begin();

  connectWiFi();

  read_BMP180();
  read_BH1750();
}







//program

void loop() {
  timer.run();

  view();

  delay(2500);
}
