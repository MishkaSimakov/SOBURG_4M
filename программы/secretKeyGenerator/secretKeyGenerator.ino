#include <EEPROM.h>
#define EEPROM_SIZE 10

void setup() {
  Serial.begin(115200);
  
  EEPROM.begin(EEPROM_SIZE);

  int secret_key;

  Serial.println("Generating secret key... ");
  secret_key = random(1, 99);

  EEPROM.write(2, secret_key);
  EEPROM.commit();

  Serial.print("Secret key: ");
  Serial.println(secret_key);
}

void loop() {
  // put your main code here, to run repeatedly:

}
