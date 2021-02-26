/*
 RC PulseIn Joystick Servo Control
 By: Nick Poole
 SparkFun Electronics
 Date: 5
 License: CC-BY SA 3.0 - Creative commons share-alike 3.0
 use this code however you'd like, just keep this license and
 attribute. Let me know if you make hugely, awesome, great changes.
 */


unsigned long ch1; // Here's where we'll keep our channel values
unsigned long ch2; // Here's where we'll keep our channel values

int ch1_in = 7;
int ch2_in = 8;
int move; // Forward/Back speed
int move2; // Forward/Back speed

int pwm_a = 5;  //PWM control for motor outputs
int pwm_b = 6;  //PWM control for motor outputs

int pwm_c = 10;  //PWM control for motor outputs
int pwm_d = 9;  //PWM control for motor outputs

void setup() {
  Serial.begin(115200);
  pinMode(ch1_in, INPUT); // Set our input pins as such  
  pinMode(ch2_in, INPUT); // Set our input pins as such
  
  
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
  ch1 = pulseIn(ch1_in, HIGH); //, 25000); // each channel
  ch2 = pulseIn(ch2_in, HIGH); //, 25000); // each channel
  if(ch1 != 0 and ch2 != 0){
    Serial.println("Go");  
    
  //  Serial.print("Ch1: ");
  //  Serial.print(ch1);
  //  Serial.print(" Ch2: ");  
  //  Serial.println(ch2);
  //  
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
  //  analogWrite(pwm_c, 0);
  //  analogWrite(pwm_d, 80);
  //  delay(500);
  //  analogWrite(pwm_c, 80);
  //  analogWrite(pwm_d, 0);
  //  delay(500);
    
  //    Serial.print("Move: ");
  //    Serial.print(move);
  //    Serial.print(" Move2:");
  //    Serial.println(move2);
   
  //  Serial.print("move:"); //Serial debugging stuff
  //  Serial.println(move);
    
  //  Serial.println(); //Serial debugging stuff
  //  Serial.println();
  //  Serial.println();
  }
  else{
    Serial.println("Stop");
    analogWrite(pwm_a, 0);  
    analogWrite(pwm_b, 0);
    analogWrite(pwm_c, 0);  
    analogWrite(pwm_d, 0);
  }
  
  delay(500);
}
