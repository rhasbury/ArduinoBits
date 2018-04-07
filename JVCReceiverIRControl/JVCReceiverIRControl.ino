/* send.ino Example sketch for IRLib2
 *  Illustrates how to send a code.
 */
#include <IRLibSendBase.h>    // First include the send base
//Now include only the protocols you wish to actually use.
//The lowest numbered protocol should be first but remainder 
//can be any order.
#include <IRLibRecv.h>
#include <IRLibDecodeBase.h>
#include <IRLib_P06_JVC.h>    
#include <IRLibCombo.h>     // After all protocols, include this
// All of the above automatically creates a universal sending
// class called "IRsend" containing only the protocols you want.
// Now declare an instance of that sender.

#define TVUSB 7 //D7
#define JVCPower 8 //D8

IRsend mySender;
IRrecv MyReceiver(11);
IRdecode MyDecoder;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

int TVUSB_state = 0;
int JVCPower_state = 0;


void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo  
  pinMode(TVUSB, INPUT_PULLUP);
  pinMode(JVCPower, INPUT_PULLUP);
  inputString.reserve(200);
  //pinMode(11, INPUT);
  //MyReceiver.enableIRIn();//start receiving
}

void loop() {
  JVCPower_state = digitalRead(JVCPower);
  TVUSB_state = digitalRead(TVUSB);

  if (stringComplete) {
    //Serial.println(inputString);
    // clear the string:

    if(inputString.indexOf("isjvc") >= 0){
      Serial.println("1");    
    }
    if(inputString.indexOf("turn_on") >= 0){
      if(digitalRead(JVCPower) == HIGH){
        mySender.send(JVC, 0xC5E8, 16);  
        Serial.println("turned_on");  
      }
      Serial.println("or_already_on");  
    }
    if(inputString.indexOf("turn_off") >= 0){
      if(digitalRead(JVCPower) == LOW){
        mySender.send(JVC, 0xC5E8, 16);  
        Serial.println("turned_off");  
      }
      Serial.println("or already off");  
    }
  
   //Serial.println(inputString);    
    inputString = "";
    stringComplete = false;
  }
  //Serial.println(F("Looping."));
  //Serial.print("Power: ");
  //Serial.print(JVCPower_state);
  //Serial.print(" B TVUSB: ");
  //Serial.print(TVUSB_state);
  //Serial.println("  ");


  
  delay(500);
}


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
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
}
