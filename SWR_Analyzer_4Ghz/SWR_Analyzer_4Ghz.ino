
//#define STORED

#define OLEDINSTALLED true



#include <SPI.h>
#include <Wire.h>
//#include <stdint.h>
#include <Arduino.h>
#include "adf4351.h"
#if (OLEDINSTALLED == true)
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#endif


#define SWVERSION "1.0"

#define PIN_SS 2  ///< SPI slave select pin, default value

ADF4351  vfo(PIN_SS, SPI_MODE0, 1000000UL , MSBFIRST) ;
uint32_t steps[] = { 1000, 5000, 10000, 50000, 100000 , 500000, 1000000 };

#ifdef STORED
  #define STEPS 35

  struct swrtable 
  {
    unsigned long frequency;
    float phase;    // a0
    float magnitude; // a1 
  };
  typedef struct swrtable SWRTable;
  SWRTable VSWRTable[STEPS];
#else
  #define STEPS 300
#endif

int stepcount = 1;
//int divisions = STEPS;
unsigned long stepsize = 100000000 / STEPS;






const int ainput = 14; // phase
const int ainput1 = 15; // magnitude
const int ainput2 = 16; // vref
int vref = 0;

int a0, a1, a2;
float fa0, fa1, fa2;



String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{
  
  Serial.begin(9600); //Initialize serial port - 9600 bps
  Wire.begin();
   // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  #if (OLEDINSTALLED == true)
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  #endif

  /*!
     setup the chip (for a 10 mhz ref freq)
     most of these are defaults
  */
  vfo.pwrlevel = 0 ; ///< sets to -4 dBm output
  vfo.RD2refdouble = 0 ; ///< ref doubler off
  vfo.RD1Rdiv2 = 0 ;   ///< ref divider off
  vfo.ClkDiv = 150 ;
  vfo.BandSelClock = 80 ;
  vfo.RCounter = 1 ;  ///< R counter to 1 (no division)
  vfo.ChanStep = steps[2] ;  ///< set to 10 kHz steps

  /*!
     sets the reference frequency to 10 Mhz
  */
  if ( vfo.setrf(10000000UL) ==  0 )
    Serial.println("ref freq set to 10 Mhz") ;
    else
      Serial.println("ref freq set error") ;
      /*!
         initialize the chip
      */
      vfo.init() ;

  /*!
     enable frequency output
  */
  vfo.enable() ;


}

void loop()
{
  
//*********** serial responding section
  if (stringComplete) {
    //Serial.println(inputString);   
    
    if(inputString.indexOf("get_a0") >= 0){
      Serial.println(analogRead(ainput));    
    }
    if(inputString.indexOf("get_a1") >= 0){
      Serial.println(analogRead(ainput1));    
    }
    if(inputString.indexOf("get_a2") >= 0){
      Serial.println(analogRead(ainput2));    
    }  
    if(inputString.indexOf("whatis") >= 0){
      Serial.println("swr4g");    
    }
    inputString = "";
    stringComplete = false;
  }

//*********** Oled management section

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


//*********** Frequency stepping and measuring section

  if(stepcount > STEPS){

    #ifdef STORED
    for(int i=1; i < STEPS; i++){
        Serial.print(VSWRTable[i].frequency);   
        Serial.print(',');   
        Serial.print(VSWRTable[i].magnitude);   
        Serial.print(',');   
        Serial.println(VSWRTable[i].phase);   
        
    }
    #endif
    Serial.println("break");   
    Serial.println("break");   
    delay(5000);
    stepcount = 1;
  }
//
//  AD.setFrequency(MD_AD9833::CHAN_0, (stepcount * stepsize) + 1070000000UL);
//  AD.setPhase(MD_AD9833::CHAN_0, 0);
//  AD.setMode(MD_AD9833::MODE_SINE);    
  vfo.setf((stepcount * stepsize) + 1070000000UL);
  delay(100);
  #ifdef STORED
  VSWRTable[stepcount].frequency = stepcount * stepsize;
  VSWRTable[stepcount].phase = analogRead(ainput);
  VSWRTable[stepcount].magnitude  = analogRead(ainput1); 
  #else
  
  vref = analogRead(ainput2);
  Serial.print(stepcount * stepsize);   
  Serial.print(',');   
  Serial.print(analogRead(ainput) - (vref/2));   
  Serial.print(',');   
  Serial.println(analogRead(ainput1) - (vref/2));   
  #endif  
  stepcount++;
  delay(20); // Print value every 1 sec.
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
