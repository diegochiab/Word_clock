/* 
 * Diego Word Clock
 * 
 * This clock is built to display spelled out words depending on the time of day.
 * The code depends on both an RTC and Addressable RGB LEDs
 * 
 * RTC Chip used: DS1307 and connect via I2C interface (pins SCL & SDA)
 * RGB LEDs used: WS2812B LEDs on a 5v strip connected to pin 6
 *
 * To set the RTC for the first time you have to send a string consisting of
 * the letter T followed by ten digit time (as seconds since Jan 1 1970) Also known as EPOCH time.
 *
 * You can send the text "T1357041600" on the next line using Serial Monitor to set the clock to noon Jan 1 2013  
 * Or you can use the following command via linux terminal to set the clock to the current time (UTC time zone)
 * date +T%s > /dev/ttyACM0
 * Inside the processSyncMessage() function I'm offsetting the UTC time with Central time.
 * If you want the clock to be accurate for your time zone, you may need to update the value.
 */
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#ifdef __AVR__
#include <avr/power.h>
#endif

RTC_DS1307 rtc;

#define RGBLEDPIN    6

#define N_LEDS 256 // 16 x 16 grid
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define BRIGHTNESSDAY 100 //200 // full on
#define BRIGHTNESSNIGHT 50 // half on

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, RGBLEDPIN, NEO_GRB + NEO_KHZ800);

const int buttonSET_Pin = 50;
const int buttonUP_Pin = 51;
const int buttonDOWN_Pin = 52;

int buttonSET_State = 0;
int buttonUP_State = 0;
int buttonDOWN_State = 0;

int menu=0;
int ButtonSetState, ButtonUpState,ButtonDownState;
int lastButtonSetState,  lastButtonUpState, lastButtonDownState= 0;

// a few vars to keep the peace;
int intBrightness = BRIGHTNESSDAY; // the brightness of the clock (0 = off and 255 = 100%)
int intTestMode; // set when both buttons are held down
String strTime = ""; // used to detect if word time has changed
int intTimeUpdated = 0; // used to tell if we need to turn on the brightness again when words change

// the words
int arrSONO_LE[] = {
  0,1,2,3,5,6,-1};
int arrE_ACC[] = {
  4,-1};
int arrL_APO[] = {
  7,8,-1};

//ORE
//int ore;
int arrORE_UNA[] = {
  9,10,11,-1};
int arrORE_DUE[] = {
  16,17,18,-1};
int arrORE_TRE[] = {
  19,20,21,-1};
int arrORE_QUATTRO[] = {
  65,66,67,68,69,70,71,-1};
int arrORE_CINQUE[] = {
  53,54,55,56,57,58,-1};
int arrORE_SEI[] = {
  33,34,35,-1};
int arrORE_SETTE[] = {
  59,60,61,62,63,-1};
int arrORE_OTTO[] = {
  48,49,50,51,-1};
int arrORE_NOVE[] = {
  12,13,14,15,-1};
int arrORE_DIECI[] = {
  91,92,93,94,95,-1};
int arrORE_UNDICI[] = {
  74,75,76,77,78,79,-1};
int arrORE_MEZZANOTTE[] = {
  22,23,24,25,26,27,28,29,30,31,-1};
int arrORE_MEZZOGIORNO[] = {
  37,38,39,40,41,42,43,44,45,46,47,-1};

//congiunzioni
int arrMENO[] = {
  86,87,88,89,-1};
int arrE_CONG[] = {
  88,-1};

//minuti
//int minuti;
int arrMIN_UNO[] = {
  192,193,194,-1};
int arrMIN_DUE[] = {
  203,204,205,-1};
int arrMIN_TRE[] = {
  161,162,163,-1};
int arrMIN_QUATTRO[] = {
  225,226,227,228,229,230,231,-1};
int arrMIN_CINQUE[] = {
  195,196,197,198,199,200,-1};
int arrMIN_SEI[] = {
  220,221,222,-1};
int arrMIN_SETTE[] = {
  170,171,172,173,174,-1};
int arrMIN_OTTO[] = {
  152,153,154,155,-1};
int arrMIN_NOVE[] = {
  182,183,184,185,-1};
int arrMIN_DIECI[] = {
  139,140,141,142,143,-1};
int arrMIN_UNDICI[] = {
  106,107,108,109,110,111,-1};
int arrMIN_DODICI[] = {
  133,134,135,136,137,138,-1};
int arrMIN_TREDICI[] = {
  161,162,163,164,165,166,167,-1};
int arrMIN_QUATTORDICI[] = {
  208,209,210,211,212,213,214,215,216,217,218,-1};
int arrMIN_QUINDICI[] = {
  120,121,122,123,124,125,126,127,-1};
int arrMIN_SEDICI[] = {
  176,177,178,179,180,181,-1};
int arrMIN_DICIASSETTE[] = {
  164,165,166,167,168,169,170,171,172,173,174,-1};
int arrMIN_DICIOTTO[] = {
  152,153,154,155,156,157,158,159,-1};
int arrMIN_DICIANNOVE[] = {
  182,183,184,185,186,187,188,189,190,191,-1};
int arrMIN_VENTI[] = {
  128,129,130,131,132,-1};
int arrMIN_VENT[] = {
  128,129,130,131,-1};
int arrMIN_TRENTA[] = {
  80,81,82,83,84,85,-1};
int arrMIN_TRENT[] = {
  81,82,83,84,85,-1};
int arrMIN_QUARANTA[] = {
  112,113,114,115,116,117,118,119,-1};
int arrMIN_QUARANT[] = {
  113,114,115,116,117,118,119,-1};
int arrMIN_CINQUANTA[] = {
  96,97,98,99,100,101,102,103,104,-1};
int arrMIN_CINQUANT[] = {
  96,97,98,99,100,101,102,103,-1};
int arrMIN_QUARTO[] = {
  145,146,147,148,149,150,-1};
int arrMIN_MEZZA[] = {
  233,234,235,236,237,-1};

//secondi
//int secondi;
int arrSEC_ZERO[] = {
  249,-1};
int arrSEC_UNO[] = {
  248,-1};
int arrSEC_DUE[] = {
  247,-1};
int arrSEC_TRE[] = {
  246,-1};
int arrSEC_QUATTRO[] = {
  245,-1};
int arrSEC_CINQUE[] = {
  244,-1};
int arrSEC_SEI[] = {
  243,-1};
int arrSEC_SETTE[] = {
  242,-1};
int arrSEC_OTTO[] = {
  241,-1};
int arrSEC_NOVE[] = {
  240,-1};
int arrSEC_DIECI[] = {
  254,-1};
int arrSEC_VENTI[] = {
  253,-1};
int arrSEC_TRENTA[] = {
  252,-1};
int arrSEC_QUARANTA[] = {
  251,-1};
int arrSEC_CINQUANTA[] = {
  250,-1};
int arrSEC_SESSANTA[] = {
  255,-1};
int arrSEC_unita[10]={
  249,248,247,246,245,244,243,242,241,240};
int arrSEC_decine[6]={
  255,254,253,252,251,250};

//int arrSEC_unita[10]={15,14,13,12,11,10,9,8,7,6};
//int arrSEC_decine[6]={5,4,3,2,1,0};

DateTime now; //inizializzo la variabile tempo


void setup() {
  //inizializza led strip
  strip.begin();
  strip.setBrightness(10);
  strip.show(); //initializa all pixel to 'off'

  pinMode(buttonSET_Pin, INPUT);
  pinMode(buttonUP_Pin, INPUT);
  pinMode(buttonDOWN_Pin, INPUT);

  //inizializza RTC
#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  Serial.begin(9600);
  Wire.begin();
  rtc.begin();

   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  //Clear the buffer.
  display.clearDisplay();
  text(); 


  delay(3000); // wait for console opening

  //  if (! rtc.begin()) {
  //    Serial.println("Couldn't find RTC");
  //    while (1);
  //  }

  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void text(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop () {

  now = rtc.now();

  ButtonSetState = digitalRead(buttonSET_Pin);

  if (ButtonSetState == LOW) { 
    Serial.println("low");
  }
  else if (ButtonSetState == HIGH) { 
    Serial.println("high");
  }

  if (ButtonSetState != lastButtonSetState) {
    if (ButtonSetState == HIGH) {
      menu=menu+1;
      if (menu>3){
        menu=0;  
      }
    }
  }

//  Serial.print("menu ");
//  Serial.print(menu);
//  Serial.println();
  display.clearDisplay();
  display.setCursor(0,24);
  display.print("menu: ");
  
  switch (menu) {
  case 0:
    display.print("mostra ore");
    mostra_tempo();
    break;
  case 1:
  display.print("cambio ore");
    cambia_ore();
    mostra_tempo();
    break;
  case 2:
    display.print("cambio minuti");
    cambia_minuti();
    mostra_tempo();
    break;
  case 3:
    display.print("reset orario");
    reset_tempo();
    mostra_tempo();
//    menu=0;
    break;
  }
  display.display();
  lastButtonSetState=ButtonSetState;
  //delay(100);
}

void reset_tempo(){
  ButtonUpState = digitalRead(buttonUP_Pin);

  if (ButtonUpState != lastButtonUpState) {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    menu=0;
  }
  lastButtonUpState = ButtonUpState;
}

void mostra_tempo(){
  int ore = now.hour();
  int minuti = now.minute();
  int secondi= now.second();

  display.setCursor(0,12);
  display.print(ore, DEC);
  display.print(":");
  display.print(minuti, DEC);
  display.print(" ");
  display.print(secondi, DEC);

//  Serial.print("ora=");
//  Serial.print(ore);
//  Serial.print(':');
//  Serial.print(minuti);
//  Serial.print(' ');
//  Serial.print(secondi);
//  Serial.println();

  print_secondi(secondi);
  print_minuti(minuti);
  print_ore(ore);

  //accendo la strip nei punti programmati
  strip.show();
}

void print_secondi(int secondi){
  int decine=secondi/10;
  int unita=secondi % 10;

  if (unita==0){
    strip.setPixelColor(arrSEC_unita[9],0);
    }

  strip.setPixelColor(arrSEC_unita[unita-1],0);
  strip.setPixelColor(arrSEC_decine[decine-1],0);

  if (secondi==0){
    strip.setPixelColor(arrSEC_unita[9],0);
    strip.setPixelColor(arrSEC_decine[5],0);
  }


  strip.setPixelColor(arrSEC_unita[unita],-1);
  strip.setPixelColor(arrSEC_decine[decine],-1);
}

void print_minuti(int minuti){
  int decine=minuti/10;
  int unita=minuti % 10;

  if (minuti ==0){
    paintWord(arrE_CONG,0);
    }
   else{
    paintWord(arrE_CONG,-1);
    }
//  arrE_CONG[];
  
  if(decine==1){ //DA DIECI A DICIANNOVE
    switch (unita) {
    case 0:
      paintWord(arrMIN_NOVE,0);
      paintWord(arrMIN_UNDICI,0);
      paintWord(arrMIN_DIECI,-1);
      break;
    case 1:
      paintWord(arrMIN_DIECI,0);
      paintWord(arrMIN_DODICI,0);
      paintWord(arrMIN_UNDICI,-1);
      break;
    case 2:
      paintWord(arrMIN_UNDICI,0);
      paintWord(arrMIN_TREDICI,0);
      paintWord(arrMIN_DODICI,-1);
      break;
    case 3:
      paintWord(arrMIN_DODICI,0);
      paintWord(arrMIN_QUATTORDICI,0);
      paintWord(arrMIN_TREDICI,-1);
      break;
    case 4:
      paintWord(arrMIN_TREDICI,0);
      paintWord(arrMIN_QUINDICI,0);
      paintWord(arrMIN_QUATTORDICI,-1);
      break;
    case 5:
      paintWord(arrMIN_QUATTORDICI,0);
      paintWord(arrMIN_SEDICI,0);
      paintWord(arrMIN_QUINDICI,-1);
      break;
    case 6:
      paintWord(arrMIN_QUINDICI,0);
      paintWord(arrMIN_DICIASSETTE,0);
      paintWord(arrMIN_SEDICI,-1);
      break;
    case 7:
      paintWord(arrMIN_SEDICI,0);
      paintWord(arrMIN_DICIOTTO,0);
      paintWord(arrMIN_DICIASSETTE,-1);
      break;
    case 8:
      paintWord(arrMIN_DICIASSETTE,0);
      paintWord(arrMIN_DICIANNOVE,0);
      paintWord(arrMIN_DICIOTTO,-1);
      break;
    case 9:
      paintWord(arrMIN_DICIOTTO,0);
      paintWord(arrMIN_VENTI,0);
      paintWord(arrMIN_DICIANNOVE,-1);
      break;
    }
  }
  else{ 
    if(unita==1){ // SE UNITÀ È 1
      switch (decine) {
      case 0:
        paintWord(arrMIN_UNO,-1);
        break;
      case 1:
        paintWord(arrMIN_DIECI,0);
        paintWord(arrMIN_UNDICI,-1);
        break;
      case 2:
        paintWord(arrMIN_VENTI,0);
        paintWord(arrMIN_VENT,-1);
        paintWord(arrMIN_UNO,-1);
        break;
      case 3:
        paintWord(arrMIN_TRENTA,0);
        paintWord(arrMIN_TRENT,-1);
        paintWord(arrMIN_UNO,-1);
        break;
      case 4:
        paintWord(arrMIN_QUARANTA,0);
        paintWord(arrMIN_QUARANT,-1);
        paintWord(arrMIN_UNO,-1);
        break;
      case 5:
        paintWord(arrMIN_CINQUANTA,0);
        paintWord(arrMIN_CINQUANT,-1);
        paintWord(arrMIN_UNO,-1);
        break;
      default:
        // statements
        break;
      }
    }
    else{
      switch (decine) {
      case 0:
        paintWord(arrMIN_CINQUANTA,0);
        break;
      case 1:
        paintWord(arrMIN_NOVE,0);
        paintWord(arrMIN_DIECI,-1);
        break;
      case 2:
        paintWord(arrMIN_DICIANNOVE,0);
        paintWord(arrMIN_VENTI,-1);
        break;
      case 3:
        paintWord(arrMIN_VENTI,0);
        paintWord(arrMIN_TRENTA,-1);
        break;
      case 4:
        paintWord(arrMIN_TRENTA,0);
        paintWord(arrMIN_QUARANTA,-1);
        break;
      case 5:
        paintWord(arrMIN_QUARANTA,0);
        paintWord(arrMIN_CINQUANTA,-1);
        break;
      }
      switch (unita) {
      case 0:
        paintWord(arrMIN_NOVE,0);
        paintWord(arrMIN_UNO,0);
        break;
      case 1:
      paintWord(arrMIN_DUE,0);
        paintWord(arrMIN_UNO,-1);
        break;
      case 2:
        paintWord(arrMIN_UNO,0);
        paintWord(arrMIN_TRE,0);
        paintWord(arrMIN_DUE,-1);
        break;
      case 3:
        paintWord(arrMIN_DUE,0);
        paintWord(arrMIN_QUATTRO,0);
        paintWord(arrMIN_TRE,-1);
        break;
      case 4:
        paintWord(arrMIN_TRE,0);
        paintWord(arrMIN_CINQUE,0);
        paintWord(arrMIN_QUATTRO,-1);
        break;
      case 5:
        paintWord(arrMIN_QUATTRO,0);
        paintWord(arrMIN_SEI,0);
        paintWord(arrMIN_CINQUE,-1);
        break;
      case 6:
        paintWord(arrMIN_CINQUE,0);
        paintWord(arrMIN_SETTE,0);
        paintWord(arrMIN_SEI,-1);
        break;
      case 7:
        paintWord(arrMIN_SEI,0);
        paintWord(arrMIN_OTTO,0);
        paintWord(arrMIN_SETTE,-1);
        break;
      case 8:
        paintWord(arrMIN_SETTE,0);
        paintWord(arrMIN_NOVE,0);
        paintWord(arrMIN_OTTO,-1);
        break;
      case 9:
        paintWord(arrMIN_OTTO,0);
        paintWord(arrMIN_NOVE,-1);
        break;
      }
    }
  }
}

void print_ore(int ore){
  if(ore>12){
    ore=ore-12;
  }

  switch (ore) {
  case 0:
    paintWord(arrSONO_LE,0);
    paintWord(arrORE_UNDICI,0);
    paintWord(arrL_APO,0);
    paintWord(arrORE_UNA,0);
    paintWord(arrE_ACC,-1);
    paintWord(arrORE_MEZZANOTTE,-1);
    break;
  case 1:
//  paintWord(arrE_ACC,0);
    paintWord(arrORE_MEZZANOTTE,0);
    paintWord(arrORE_MEZZOGIORNO,0);
    paintWord(arrSONO_LE,0);
    paintWord(arrORE_DUE,0);
    paintWord(arrE_ACC,-1);
    paintWord(arrL_APO,-1);
    paintWord(arrORE_UNA,-1);
    break;
  case 2:
    paintWord(arrE_ACC,0);
    paintWord(arrL_APO,0);
    paintWord(arrORE_UNA,0);
    paintWord(arrORE_TRE,0);
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_DUE,-1);
    break;
  case 3:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_DUE,0);
    paintWord(arrORE_QUATTRO,0);
    paintWord(arrORE_TRE,-1);
    break;
  case 4:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_TRE,0);
    paintWord(arrORE_CINQUE,0);
    paintWord(arrORE_QUATTRO,-1);
    break;
  case 5:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_QUATTRO,0);
    paintWord(arrORE_SEI,0);
    paintWord(arrORE_CINQUE,-1);
    break;
  case 6:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_CINQUE,0);
    paintWord(arrORE_SETTE,0);    
    paintWord(arrORE_SEI,-1);
    break;
  case 7:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_SEI,0);
    paintWord(arrORE_OTTO,0);
    paintWord(arrORE_SETTE,-1);
    break;
  case 8:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_SETTE,0);
    paintWord(arrORE_NOVE,0);
    paintWord(arrORE_OTTO,-1);
    break;
  case 9:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_OTTO,0);
    paintWord(arrORE_DIECI,0);
    paintWord(arrORE_NOVE,-1);
    break;
  case 10:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_NOVE,0);
    paintWord(arrORE_UNDICI,0);
    paintWord(arrORE_DIECI,-1); 
    break;
  case 11:
    paintWord(arrSONO_LE,-1);
    paintWord(arrORE_DIECI,0);
    paintWord(arrE_ACC,0);
    paintWord(arrORE_MEZZOGIORNO,0);
    paintWord(arrORE_MEZZANOTTE,0);
    paintWord(arrORE_UNDICI,-1);
    break;
  case 12:
    paintWord(arrSONO_LE,0);
    paintWord(arrORE_UNDICI,0);
    paintWord(arrL_APO,0);
    paintWord(arrORE_UNA,0);
    paintWord(arrE_ACC,-1);
    paintWord(arrORE_MEZZOGIORNO,-1);
    break;
  }
}  

void cambia_ore(void){

  ButtonUpState = digitalRead(buttonUP_Pin);
  ButtonDownState = digitalRead(buttonDOWN_Pin);

  int ore = now.hour();
  int minuti = now.minute();
  int secondi=now.second();

  if (ButtonUpState != lastButtonUpState) {
    if (ButtonUpState == HIGH) {
      rtc.adjust(DateTime(now.unixtime() + 3600));
    }
  }
  else if (ButtonDownState != lastButtonDownState) {
    if (ButtonDownState == HIGH) {
      rtc.adjust(DateTime(now.unixtime() - 3600));
    }
  }

  lastButtonUpState = ButtonUpState;
  lastButtonDownState = ButtonDownState;

} 

void cambia_minuti(void){

  ButtonUpState = digitalRead(buttonUP_Pin);
  ButtonDownState = digitalRead(buttonDOWN_Pin);

  int ore = now.hour();
  int minuti = now.minute();
  int secondi=now.second();

  if (ButtonUpState != lastButtonUpState) {
    if (ButtonUpState == HIGH) {
      rtc.adjust(DateTime(now.unixtime() + 60));
    }
  }
  else if (ButtonDownState != lastButtonDownState) {
    if (ButtonDownState == HIGH) {
      rtc.adjust(DateTime(now.unixtime() - 60));
    }
  }

  lastButtonUpState = ButtonUpState;
  lastButtonDownState = ButtonDownState;

}

void paintWord(int arrWord[], uint32_t intColor){
  for(int i = 0; i < strip.numPixels() + 1; i++){
    if(arrWord[i] == -1){
      strip.show();
      break;
    }
    else{
      strip.setPixelColor(arrWord[i],intColor);
      //      Serial.print(arrWord[i]);
      //      Serial.print(':');
      //      Serial.println(intColor);
    }
  }
}
