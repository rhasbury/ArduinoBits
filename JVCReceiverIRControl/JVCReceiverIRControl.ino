/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 */
#include <IRremote.h>
 
//#define PanasonicAddress      0x4004     // Panasonic address (Pre data) 
//#define PanasonicPower        0x100BCBD  // Panasonic Power button

#define JVCPower              0xC5E8


String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

IRsend irsend;

void setup()
{
    Serial.begin(115200);
    inputString.reserve(200);
}

void loop() {
    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag
      // so the main loop can do something about it:
      if (inChar == '\n' or inChar == ';') {
        stringComplete = true;
      }
    }
    if (stringComplete) {
        if(inputString.indexOf("jvc_power_on") >= 0){
          irsend.sendJVC(JVCPower, 16,0); // hex value, 16 bits, no repeat        
          Serial.println("turned on");
        }
  
        if(inputString.indexOf("jvc_power_off") >= 0){
          irsend.sendJVC(JVCPower, 16,0); // hex value, 16 bits, no repeat        
          Serial.println("turned off");
        }
    inputString = "";
    stringComplete = false;
    }
  
  
  //irsend.sendPanasonic(PanasonicAddress,PanasonicPower); // This should turn your TV on and off
  
  //irsend.sendJVC(JVCPower, 16,0); // hex value, 16 bits, no repeat
  //delayMicroseconds(50); // see http://www.sbprojects.com/knowledge/ir/jvc.php for information
  //irsend.sendJVC(JVCPower, 16,1); // hex value, 16 bits, repeat

  
  delayMicroseconds(50);
  
}
