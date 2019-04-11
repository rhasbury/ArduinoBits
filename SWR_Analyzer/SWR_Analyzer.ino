
//#define STORED

#define OLEDINSTALLED true

#if (OLEDINSTALLED == true)
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#include <MD_AD9833.h>
#include <SPI.h>

// Pins for SPI comm with the AD9833 IC
#define DATA  4  ///< SPI Data pin number
#define CLK   2  ///< SPI Clock pin number
#define FSYNC 3  ///< SPI Load pin number (FSYNC in AD9833 usage)


MD_AD9833  AD(DATA, CLK, FSYNC); // Arbitrary SPI pins

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
int steps = STEPS;
unsigned long stepsize = 12500000 / STEPS;


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#endif


const int ainput = A0; // phase
const int ainput1 = A1; // magnitude
const int ainput2 = A2; // vref
int vref = 0;

int a0, a1, a2;
float fa0, fa1, fa2;



String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{
  
  Serial.begin(9600); //Initialize serial port - 9600 bps
  AD.begin();
   // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  #if (OLEDINSTALLED == true)
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  #endif

  AD.setFrequency(MD_AD9833::CHAN_0, 5000);
  AD.setPhase(MD_AD9833::CHAN_0, 0);
  AD.setMode(MD_AD9833::MODE_SINE);
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
      Serial.println("swr");    
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

  AD.setFrequency(MD_AD9833::CHAN_0, stepcount * stepsize);
  AD.setPhase(MD_AD9833::CHAN_0, 0);
  AD.setMode(MD_AD9833::MODE_SINE);    
  delay(10);
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
