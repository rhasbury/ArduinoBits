//
//*****************************************************************
// ADF4351 PLL-Synthesizer 33Mhz - 4,4Ghz
// Integer mode Demo OE6OCG 1/2015
// Hardware Arduino Uno mit Adafruit Keyb-LCD Shield 16x2 ~ 10USD
// ADF4351 PLL-VCO Board assembled from Ebay or China ~ 40 USD
// 
// Buttoms:
// up/down = Freq +- with stepsize
// left = frequency step's from 6.25khz to 1Mhz
// right = scan with stepsize
// select = predefined startfrequencies from 34Mhz to 4.4 Ghz
//
// IDE: Arduino 1.5.4 or up without spec. Library
// License: Free or a gift to Paypal oe6ocg@aon.at
//*****************************************************************
/*                                               Hardware connection Uno to PLL-Board (3.3V logic)
                                  +-----+
     +----[PWR]-------------------| USB |--+
     |                            +-----+  |                        output: 33Mhz - 4.4Ghz ~ 3 dbm
     |         GND/RST2  [ ][ ]            |             PLL-Board + Arduino with Display Bk.light draw 200mA on 5V USB 
     |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] |                                       #
     |          5V/MISO2 [ ][ ]  A4/SDA[ ] |    |---|_3.3k_|-----\                 |
     |                             AREF[ ] |    !---|_3.3k_|---\ |            _____|______
     |                              GND[ ] |----!---|_3.3k_|-| | |           |            |
     | [ ]N/C                    SCK/13[ ] |------|_1.5k_|---!-|-|----Clock--|ADF4351     |
     | [ ]v.ref                 MISO/12[ ] |                   | |           |PLL Board   |
     | [ ]RST                   MOSI/11[ ]~|------|_1.5k_|-----!-|----Data---|3.3V logic  |
     | [ ]3V3    +---+               10[ ]~|                     |           |            |
     | [ ]5v     | A |                9[ ]~|   LCD               !----LE-----|            |
     | [ ]GND   -| R |-               8[ ] |   LCD               |           |____________|
     | [ ]GND   -| D |-                    |                     |
     | [ ]Vin   -| U |-               7[ ] |   LCD               |
     |          -| I |-               6[ ]~|   LCD               |
Keyb | [ ]A0    -| N |-               5[ ]~|   LCD               |
RSSI | [ ]A1    -| O |-               4[ ] |   LCD               |
     | [ ]A2     +---+           INT1/3[ ]~|------|_1.5k_|---LE--/
     | [ ]A3                     INT0/2[ ] |
     | [ ]A4/SDA  RST SCK MISO     TX>1[ ] |
     | [ ]A5/SCL  [ ] [ ] [ ]      RX<0[ ] |
     |            [ ] [ ] [ ]              |
     |  UNO       GND MOSI 5V  ____________/
      \_______________________/

*/

#include <Wire.h>
//#include <LiquidCrystal.h>
#include <SPI.h>

// LiquidCrystal lcd(8, 9, 4, 5, 6, 7);         //DFR LCD-keyb Shield

const int LEPin = 11;  //LE pin. Enables loaded data when pulled high  ADF4350
const int CEPin = 8;  //LE pin. Enables loaded data when pulled high  ADF4350

long Freq = 44220000;  //Startfrequenz generel 100Hz aulösung
long refin = 2500000; // Refrenquarz = 25Mhz
long ChanStep = 625; //Kanalraster = 6,25Khz kleinstes raster
long channel[13]; // Kanal-speicher
long Step[5]; // Kanalraster-speicher
int CHnum = 0; // eingestellter Kanal
int StepNum = 0; // eingestelltes Kanalraster
unsigned long Reg[6]; //ADF4351 Reg's

byte tenHz, hundredHz, ones, tens, hundreds, thousands, tenthousands, hundredthousands, millions; 

int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

void setup() {
//  lcd.begin(16, 2);
//  lcd.setCursor(0, 0);
//  lcd.print(" ADF4350  OE6OCG");

  Serial.begin(115200);// USB to PC for Debug only
  pinMode (LEPin, OUTPUT);
  digitalWrite(LEPin, LOW);
  pinMode (CEPin, OUTPUT);
  digitalWrite(CEPin, HIGH);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  //SPI.begin();
  //SPI.begin(CLK, MISO, MOSI, SS)
  //SPI.begin(6, 8, 7, 11); //something about using gpio 6 and 7 makes the esp32 fault out. 
  SPI.begin(36, 8, 25, 11);
  delay(500);

  Step[0] = 625; // 6,25 khz, 5khz geht nicht im Int-N Mode(MOD > 4095) bei 25Mhz Ref.
  Step[1] = 1000; // 10 khz
  Step[2] = 1250; // 12.5 khz
  Step[3] = 2500; // 25 khz
  Step[4] = 100000; // 1 Mhz Step

  channel[0] = 3400000; // Band select FIXFREQUENZEN
  channel[1] = 5150000; //51.5 Mhz
  channel[2] = 14455000;
  channel[3] = 14500000;
  channel[4] = 14560000;
  channel[5] = 43000000;
  channel[6] = 43127500;
  channel[7] = 43887500;
  channel[8] = 124500000;
  channel[9] = 129820000;
  channel[10] = 234500000;
  channel[11] = 300000000;
  channel[12] = 440000000; // 4.4Ghz

//  lcd.setCursor(0, 0);
//  lcd.print("         ");
//  lcd.setCursor(0, 0);
//  lcd.print("S=");
//  lcd.print(ChanStep);
//  lcd.print("0");
//
//  lcd.setCursor(12, 1);
//  lcd.print(" Mhz");// print Mhz

  Serial.println(" Mhz");// print Mhz
  SetFreq(Freq);
}

void loop() {
  Freq = 44220000;  //
  SetFreq(Freq);
  Serial.println(Freq);
  delay(2000);

  for(int i = 0; i< 100;i++){
      Freq += ChanStep;  
      SetFreq(Freq);
      Serial.println(Freq);
      int a1 = analogRead(14); // vref
      int a2 = analogRead(12); // mag
      int a3 = analogRead(13); // phase
      Serial.println(a1);
      Serial.println(a2);
      Serial.println(a3);
      delay(2000);
      
    
  }


  
}


void SetFreq(long Freq)
{

  ConvertFreq(Freq, Reg);
  WriteADF2(5);
  delayMicroseconds(2500);
  WriteADF2(4);
  delayMicroseconds(2500);
  WriteADF2(3);
  delayMicroseconds(2500);
  WriteADF2(2);
  delayMicroseconds(2500);
  WriteADF2(1);
  delayMicroseconds(2500);
  WriteADF2(0);
  delayMicroseconds(2500);
}

void WriteADF2(int idx)
{ // make 4 byte from integer for SPI-Transfer
  byte buf[4];
  for (int i = 0; i < 4; i++)
    buf[i] = (byte)(Reg[idx] >> (i * 8));
  WriteADF(buf[3], buf[2], buf[1], buf[0]);
}
int WriteADF(byte a1, byte a2, byte a3, byte a4) {
  // write over SPI to ADF4350
//  digitalWrite(slaveSelectPin, LOW);
  delayMicroseconds(10);
  SPI.transfer(a1);
  SPI.transfer(a2);
  SPI.transfer(a3);
  SPI.transfer(a4);

}

int Toggle() {
  digitalWrite(LEPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(LEPin, LOW);
}


void ConvertFreq(long freq, unsigned long R[])
{
  // PLL-Reg-R0         =  32bit
  // Registerselect        3bit
  // int F_Frac = 4;       // 12bit
  // int N_Int = 92;       // 16bit
  // reserved           // 1bit

  // PLL-Reg-R1         =  32bit
  // Registerselect        3bit
  //int M_Mod = 5;        // 12bit
  int P_Phase = 1;     // 12bit bei 2x12bit hintereinander pow()-bug !!
  int Prescal = 0;     // 1bit geht nicht ???
  int PhaseAdj = 0;    // 1bit geht auch nicht ???
  // reserved           // 3bit

  // PLL-Reg-R2         =  32bit
  // Registerselect        3bit
  int U1_CountRes = 0; // 1bit
  int U2_Cp3state = 0; // 1bit
  int U3_PwrDown = 0;  // 1bit
  int U4_PDpola = 1;    // 1bit
  int U5_LPD = 0;       // 1bit
  int U6_LPF = 1;       // 1bit 1=Integer, 0=Frac not spported yet
  int CP_ChgPump = 7;     // 4bit
  int D1_DoublBuf = 0; // 1bit
  //  int R_Counter = 1;   // 10bit
  //  int RD1_Rdiv2 = 0;    // 1bit
  //  int RD2refdoubl = 0; // 1bit
  int M_Muxout = 0;     // 3bit
  int LoNoisSpur = 0;      // 2bit
  // reserved           // 1bit

  // PLL-Reg-R3         =  32bit
  // Registerselect        3bit
  int D_Clk_div = 150; // 12bit
  int C_Clk_mode = 0;   // 2bit
  //  reserved          // 1bit
  int F1_Csr = 0;       // 1bit
  //  reserved          // 2bit
  int F2_ChgChan = 0;   // 1bit
  int F3_ADB = 0;       // 1bit
  int F4_BandSel = 0;  // 1bit
  //  reserved          // 8bit

  // PLL-Reg-R4         =  32bit
  // Registerselect        3bit
  int D_out_PWR = 0 ;    // 2bit
  int D_RF_ena = 1;     // 1bit
  int D_auxOutPwr = 0;  // 2bit
  int D_auxOutEna = 0;  // 1bit
  int D_auxOutSel = 0;  // 1bit
  int D_MTLD = 0;       // 1bit
  int D_VcoPwrDown = 0; // 1bit 1=VCO off

  //  int B_BandSelClk = 200; // 8bit

  int D_RfDivSel = 3;    // 3bit 3=70cm 4=2m
  int D_FeedBck = 1;     // 1bit
  // reserved           // 8bit

  // PLL-Reg-R5         =  32bit
  // Registerselect     // 3bit
  // reserved           // 16bit
  // reserved     11    // 2bit
  // reserved           // 1bit
  int D_LdPinMod = 1;    // 2bit muss 1 sein
  // reserved           // 8bit

  // Referenz Freg Calc
  //  long refin = 250000; // Refrenquarz = 25000000hz
  int R_Counter = 1;   // 10bit
  int RD1_Rdiv2 = 0;    // 1bit
  int RD2refdoubl = 0; // 1bit
  int B_BandSelClk = 200; // 8bit
  //  int F4_BandSel = 0;  // 1bit

  // int F4_BandSel = 10.0 * B_BandSelClk / PFDFreq;

  long RFout = Freq;   // VCO-Frequenz
  // calc bandselect und RF-div
  int outdiv = 1;

  if (RFout >= 220000000) {
    outdiv = 1;
    D_RfDivSel = 0;
  }
  if (RFout < 220000000) {
    outdiv = 2;
    D_RfDivSel = 1;
  }
  if (RFout < 110000000) {
    outdiv = 4;
    D_RfDivSel = 2;
  }
  if (RFout < 55000000) {
    outdiv = 8;
    D_RfDivSel = 3;
  }
  if (RFout < 27500000) {
    outdiv = 16;
    D_RfDivSel = 4;
  }
  if (RFout < 13800000) {
    outdiv = 32;
    D_RfDivSel = 5;
  }
  if (RFout < 6900000) {
    outdiv = 64;
    D_RfDivSel = 6;
  }

  float PFDFreq = refin * ((1.0 + RD2refdoubl) / (R_Counter * (1.0 + RD1_Rdiv2))); //Referenzfrequenz
  float N = ((RFout) * outdiv) / PFDFreq;
  int N_Int = N;
  long M_Mod = PFDFreq * (100000 / ChanStep) / 100000;
  int F_Frac = round((N - N_Int) * M_Mod);

  R[0] = (unsigned long)(0 + F_Frac * pow(2, 3) + N_Int * pow(2, 15));
  R[1] = (unsigned long)(1 + M_Mod * pow(2, 3) + P_Phase * pow(2, 15) + Prescal * pow(2, 27) + PhaseAdj * pow(2, 28));
  //  R[1] = (R[1])+1; // Registerselect adjust ?? because unpossible 2x12bit in pow() funktion
  R[2] = (unsigned long)(2 + U1_CountRes * pow(2, 3) + U2_Cp3state * pow(2, 4) + U3_PwrDown * pow(2, 5) + U4_PDpola * pow(2, 6) + U5_LPD * pow(2, 7) + U6_LPF * pow(2, 8) + CP_ChgPump * pow(2, 9) + D1_DoublBuf * pow(2, 13) + R_Counter * pow(2, 14) + RD1_Rdiv2 * pow(2, 24) + RD2refdoubl * pow(2, 25) + M_Muxout * pow(2, 26) + LoNoisSpur * pow(2, 29));
  R[3] = (unsigned long)(3 + D_Clk_div * pow(2, 3) + C_Clk_mode * pow(2, 15) + 0 * pow(2, 17) + F1_Csr * pow(2, 18) + 0 * pow(2, 19) + F2_ChgChan * pow(2, 21) +  F3_ADB * pow(2, 22) + F4_BandSel * pow(2, 23) + 0 * pow(2, 24));
  R[4] = (unsigned long)(4 + D_out_PWR * pow(2, 3) + D_RF_ena * pow(2, 5) + D_auxOutPwr * pow(2, 6) + D_auxOutEna * pow(2, 8) + D_auxOutSel * pow(2, 9) + D_MTLD * pow(2, 10) + D_VcoPwrDown * pow(2, 11) + B_BandSelClk * pow(2, 12) + D_RfDivSel * pow(2, 20) + D_FeedBck * pow(2, 23));
  R[5] = (unsigned long)(5 + 0 * pow(2, 3) + 3 * pow(2, 19) + 0 * pow(2, 21) + D_LdPinMod * pow(2, 22));
}
//to do instead of writing 0x08000000 you can use other two possibilities: (1ul << 27) or (uint32_t) (1 << 27).

// as PLL-Register Referenz
// R[0] = (0x002E0020); // 145.0 Mhz, 12.5khz raster
// R[1] = (0x08008029);
// R[2] = (0x00004E42);
// R[3] = (0x000004B3);
// R[4] = (0x00BC8024);
// R[5] = (0x00580005);
