// WRITES ARDUINO SERIAL TO EEPROM
//
// do this only once on an Arduino, 
// write the Serial of the Arduino in the 
// first 6 bytes of the EEPROM

#include <EEPROM.h>
  struct { 
    char sID[7] = "NC0001";
    char ssid[50] = "";
    char wifi_password[50] = "";
  } data;

int addr = 0;

void setup()
{
 Serial.begin(9600);
 EEPROM.begin(512);
 EEPROM.put(addr,data);
 /*
 for (int i=0; i<6; i++) {
   
   EEPROM.write(i,sID[i]);
   
 }

 EEPROM.put(100,ssid);
 EEPROM.put(200,wifi_password);
 */
 
 //EEPROM.commit();
}

void loop() {
 Serial.println(data.sID);
 delay(1000);
}
