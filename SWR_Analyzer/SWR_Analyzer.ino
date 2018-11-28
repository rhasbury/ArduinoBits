

#define OLEDINSTALLED true

#if (OLEDINSTALLED == true)
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#endif


const int ainput = A0; //GAS sensor output pin to Arduino analog A0 pin
const int ainput1 = A1; //GAS sensor output pin to Arduino analog A0 pin
const int ainput2 = A2; //GAS sensor output pin to Arduino analog A0 pin


int a0, a1, a2;
float fa0, fa1, fa2;


String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{
  
  Serial.begin(9600); //Initialize serial port - 9600 bps
   // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  #if (OLEDINSTALLED == true)
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  #endif
}

void loop()
{
  

  if (stringComplete) {
    //Serial.println(inputString);
    // clear the string:
    
    if(inputString.indexOf("get_a0") >= 0){
      Serial.println(analogRead(ainput));    
    }
    if(inputString.indexOf("get_a1") >= 0){
      Serial.println(analogRead(ainput1));    
    }
    if(inputString.indexOf("get_a2") >= 0){
      Serial.println(analogRead(ainput2));    
    }
    
    


    
    //Serial.println(inputString);    
    inputString = "";
    stringComplete = false;
  }

    #if (OLEDINSTALLED == true)
    a0 = analogRead(ainput);
    a1 = analogRead(ainput1);
    a2 = analogRead(ainput2);

    fa0 = (float)a0/1023;
    fa1 = (float)a1/1023;
    fa2 = (float)a2/1023;
       
    display.clearDisplay();  
    display.setCursor(40+(int)(fa0*80), 1);  
    display.print("|");   
    
    display.setCursor(40+(int)(fa1*80), 10);  
    display.print("|");   

    display.setCursor(40+(int)(fa2*80), 20);  
    display.print("|");   

    
    
    
    display.setCursor(10,1);  
    display.setTextColor(WHITE);
    display.setTextSize(1); 
    display.print(a0);   
    display.setCursor(10,10);        
    display.print(a1);   
    display.setCursor(10,20);        
    display.print(a2);          
    display.display();  
  
    #endif
 
  delay(100); // Print value every 1 sec.
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