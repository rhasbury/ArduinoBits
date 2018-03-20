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


int TVUSB_state = 0;
int JVCPower_state = 0;


void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo  
  pinMode(TVUSB, INPUT_PULLUP);
  pinMode(JVCPower, INPUT_PULLUP);
  //pinMode(11, INPUT);
  MyReceiver.enableIRIn();//start receiving
}

void loop() {
  /*
  if (Serial.read() != -1) {
    //send a code every time a character is received from the 
    // serial port. You could modify this sketch to send when you
    // push a button connected to an digital input pin.
    //Substitute values and protocols in the following statement
    // for device you have available.
    mySender.send(JVC, 0xC5E8, 16);//Sony DVD power A8BCA, 20 bits    
    Serial.println(F("Sent signal."));
  }
  */

  JVCPower_state = digitalRead(JVCPower);
  TVUSB_state = digitalRead(TVUSB);

  if(digitalRead(TVUSB) == HIGH){
    if(digitalRead(JVCPower) == LOW){
      mySender.send(JVC, 0xC5E8, 16);  
    }
  }
  
  if(digitalRead(TVUSB) == LOW){
    if(digitalRead(JVCPower) == HIGH){
      mySender.send(JVC, 0xC5E8, 16);  
    }
  }
  Serial.print("Power: ");
  Serial.print(JVCPower_state);
  Serial.print("  TVUSB: ");
  Serial.print(TVUSB_state);
  Serial.println("  ");

  if (MyReceiver.getResults()) {//wait till it returns true
    MyDecoder.decode();
    MyDecoder.dumpResults();
    MyReceiver.enableIRIn(); //restart the receiver
    }

  Serial.println(F("Looping."));
  
  delay(5000);
}

