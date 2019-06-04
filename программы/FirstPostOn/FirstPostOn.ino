#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

// Set these to your desired credentials.
const char *ssid = "Пост контроля";
const char *password = "";

WiFiServer server(80);

void connectWiFi(String ssid, String password) {

  const char *wifi_name = ssid.c_str();
  const char *wifi_password = password.c_str();
  
  WiFi.begin(wifi_name, wifi_password);

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
  } else {
    Serial.println("Connected to the WiFi network");
  }

  delay(5000);
}

void firstPostOn() {
    WiFiClient client = server.available();   // listen for incoming clients
    
    if (client) {                             // if you get a client,
      Serial.println("New Client.");           // print a message out the serial port
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
              client.print("<head><meta charset='utf-8'></head>");
              client.print("<h1 style='text-align: center'>Пост контроля</h1> <p>Это страница вашего поста контроля. Для его успешной работы вы должны ввести данные вашей wifi сети, чтобы пост мог передавать данные на сервер, и добавить этот пост на <a href='http://digcity.000webhostapp.com/'>сайт системы СОБУРГ4М</a></p> <h2>Данные wifi сети</h2> <form method='GET' action='/'> <div style='margin: 10px'> <label for='wifi_name'>Имя wifi сети:</label> <input style='margin-left: 10px; border-radius: 5px; border: 2px solid gray; padding: 5px;' type='text' class='form-control' id='wifi_name' name='wifi_name' placeholder='ведите имя сети'> </div> <div style='margin: 10px'> <label for='wifi_password'>Пароль от wifi сети</label> <input style='margin-left: 10px; border-radius: 5px; border: 2px solid gray; padding: 5px;' type='password' class='form-control' id='wifi_password' name='wifi_password' placeholder='введите пароль'> </div> <input type='hidden' value='true' name='end'> <input type='submit' style='margin-left: 10px; border-radius: 5px; border: 2px solid gray; padding: 5px; width: 100px;' value='Отправить'> </form> <h2>Добавление поста на сайт СОБУРГ 4М</h2> <p>Для добавления поста контроля на сайт:</p> <ol> <li>Подключитесь к интернету с доступом к сети</li> <li>Перейдите по ссылке <a href='http://digcity.000webhostapp.com/posts/confirm?mac_address=dsada&code=fgdg'>http://digcity.000webhostapp.com/posts/confirm?mac_address=dsada&code=fgdg</a></li> </ol>");
  
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
          
          // Check to see if the client request was "GET /H" or "GET /L":
          if (currentLine.endsWith("&end=true") && currentLine.startsWith("Referer")) {
            int wifi_name_start_index = currentLine.indexOf("wifi_name=");
            int wifi_password_start_index = currentLine.indexOf("wifi_password=");
            
            String wifi_name = currentLine.substring(wifi_name_start_index + 10, wifi_password_start_index - 1);
            String wifi_password = currentLine.substring(wifi_password_start_index + 14, currentLine.length() - 9);
            
            Serial.println("wifi name: " + wifi_name);
            Serial.println("wifi password: " + wifi_password);
  
            connectWiFi(wifi_name, wifi_password);
          }
        }
      }
      // close the connection:
      client.stop();
      Serial.println("Client Disconnected.");
    }
}


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop() {
  firstPostOn();
}
