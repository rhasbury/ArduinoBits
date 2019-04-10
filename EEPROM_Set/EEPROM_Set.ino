// WRITES ARDUINO SERIAL TO EEPROM
//
// do this only once on an Arduino, 
// write the Serial of the Arduino in the 
// first 6 bytes of the EEPROM

#include <EEPROM.h>
char sID[7] = "NC0002";
const char* ssid = "";
const char* wifi_password = "";


void setup()
{
 Serial.begin(9600);
 EEPROM.begin();
 for (int i=0; i<6; i++) {
   
   EEPROM.write(i,sID[i]);
   
 }

 EEPROM.put(100,ssid);
 EEPROM.put(200,wifi_password);
 
 //EEPROM.commit();
}

void loop() {
 Serial.println(sID);
 delay(1000);
}
