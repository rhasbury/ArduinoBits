#include <PPMReader.h>

int interruptPin = 3;
int channelAmount = 6;
PPMReader ppm(interruptPin, channelAmount);

unsigned long ch1; // Here's where we'll keep our channel values
unsigned long ch2; // Here's where we'll keep our channel values



int move; // Forward/Back speed
int move2; // Forward/Back speed

int pwm_a = 4;  //PWM control for motor outputs
int pwm_b = 5;  //PWM control for motor outputs

int pwm_c = 9;  //PWM control for motor outputs
int pwm_d = 10;  //PWM control for motor outputs

void setup() {
  Serial.begin(115200);
  
  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);

  pinMode(pwm_c, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_d, OUTPUT);
  
  analogWrite(pwm_a, 0);  
  analogWrite(pwm_b, 0);
  analogWrite(pwm_c, 0);  
  analogWrite(pwm_d, 0);
  
}

void loop() {  
  ch1 = ppm.latestValidChannelValue(1, 0);
  ch2 = ppm.latestValidChannelValue(2, 0);
  Serial.print("Ch1 : ");  
  Serial.print(ch1);  
  Serial.print("  Ch2 : ");  
  Serial.print(ch2);  
  Serial.print("  Ch3 : ");  
  Serial.print(ppm.latestValidChannelValue(3, 0));  
  Serial.print("  Ch4 : ");  
  Serial.print(ppm.latestValidChannelValue(4, 0));  
  Serial.print("  Ch5 : ");  
  Serial.print(ppm.latestValidChannelValue(5, 0));  
  Serial.print("  Ch6 : ");  
  Serial.println(ppm.latestValidChannelValue(6, 0));  

    
  if(ch1 != 0 and ch2 != 0){
    Serial.println("Go");  

    move = map(ch1, 1000,2000, -500, 500); //center over zero
    move = constrain(move, -255, 255); //only pass values whose absolutes are                                   
                                     //valid pwm values
    move2 = map(ch2, 1000,2000, -500, 500); //center over zero
    move2 = constrain(move2, -255, 255); //only pass values whose absolutes are
                                     //valid pwm values
    
    /*What we're doing here is determining whether we want to move
    forward or backward*/
    if(move>0){analogWrite(pwm_a, abs(move));analogWrite(pwm_b, 0);};
    if(move<0){analogWrite(pwm_a, 0);analogWrite(pwm_b, abs(move));};
    //Serial.println(move);
  
    if(move2>0){analogWrite(pwm_c, abs(move2));analogWrite(pwm_d, 0);};
    if(move2<0){analogWrite(pwm_c, 0);analogWrite(pwm_d, abs(move2));};

  }
  else{
    Serial.println("Stop");
    analogWrite(pwm_a, 0);  
    analogWrite(pwm_b, 0);
    analogWrite(pwm_c, 0);  
    analogWrite(pwm_d, 0);
  }
  
  //delay(500);
}
