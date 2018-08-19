// WRITES ARDUINO SERIAL TO EEPROM
//
// do this only once on an Arduino, 
// write the Serial of the Arduino in the 
// first 6 bytes of the EEPROM

#include <EEPROM.h>
char sID[7] = "NC0002";


void setup()
{
 Serial.begin(9600);
 EEPROM.begin(6);
 for (int i=0; i<6; i++) {
   
   EEPROM.write(i,sID[i]);
   
 }
 EEPROM.commit();
}

void loop() {
 Serial.println(sID);
 delay(1000);
}
