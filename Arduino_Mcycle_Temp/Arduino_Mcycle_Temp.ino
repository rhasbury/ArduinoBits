
#include <EEPROM.h>
char sID[7];

#define OLEDINSTALLED true

#if (OLEDINSTALLED == true)
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <max6675.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#endif


#define CIRCLE_SIZE 16

uint8_t circleData[CIRCLE_SIZE] = {0};
uint8_t circlePointer = 0;
uint16_t circleSum = 0;

// for pro mini
int thermoDO = A3;
int thermoCS = A1;
int thermoCLK = A2;

//for pro micro
//int thermoDO = 7;
//int thermoCS = 9;
//int thermoCLK = 8;


MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
//int vccPin = 3;
//int gndPin = 2;
int temp;
bool toggle; 
int avg;
int diff;
int voltRead;


void setup()
{  
  
   for (int i=0; i<6; i++) {
   sID[i] = EEPROM.read(i);
   }
  
  //pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  //pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  delay(500);
  #if (OLEDINSTALLED == true)
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  #endif

}

void loop()
{
  
  temp = thermocouple.readCelsius();
  //temp = 155;
  updateRollingAverage(temp);
  voltRead = analogRead(A0);
  
  
  #if (OLEDINSTALLED == true)
  display.clearDisplay();  
  if(toggle == true){
    display.drawCircle(110, 20, 3, 1);    
    toggle = false;
  }
  else{    
    toggle = true;
  }

  display.setCursor(40,2);  
  display.setTextColor(WHITE);
  display.setTextSize(2); 
  display.print(voltRead*0.0228972);   
  
  diff = abs(temp - rollingAverage());
  if(diff < 10){
    if(temp>99){
      display.setCursor(18,24);  
    }
    else{
      display.setCursor(30,24);
    }
    display.setTextColor(WHITE);
    display.setTextSize(5);    
    if(temp < 300){
      display.print(temp);   
    }
  }
  display.display();  

  #endif
  delay(500);  
  

}

void updateRollingAverage(uint8_t value)
{
    circleSum = circleSum - circleData[circlePointer] + value;
    circleData[circlePointer] = value;
    circlePointer++;
    if (circlePointer == CIRCLE_SIZE)
    {
        circlePointer = 0;
    }
}

uint8_t rollingAverage()
{
    return circleSum / CIRCLE_SIZE;
}
