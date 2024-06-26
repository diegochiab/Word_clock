/* 
 * Diego Word Clock 10x10
 * 
 * This clock is built to display spelled out words depending on the time of day.
 * The code depends on both an RTC and Addressable RGB LEDs
 * 
 * RGB LEDs used: WS2812B LEDs on a 5v strip connected to pin 6
 *
 */
 
 //ESP8266
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
 
 //NTP
#include <NTPClient.h>
 
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

/*** DEF & CONST***/
//ESP8266
const char ssid[] = "Fablab_5G";            // inserire l'ssid della rete
const char pass[] = "34935135";  // password della rete


// Creazione di un server web in ascolto sulla porta 80
// attende contenuti (pagine html, immagini, css, ecc...)
WiFiServer server(80);

//NTP
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);
int oraAttuale = 0;

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#ifdef __AVR__
#include <avr/power.h>
#endif

#define RGBLEDPIN    6

#define N_LEDS 100 // 10 x 10 grid
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
  2,3,4,5,28,29,-1};
int arrE_ACC[] = {
  26-1};
int arrL_APO[] = {
  97,-1};

//ORE
//int ore;
int arrORE_UNA[] = {
  96,75,74,-1};
int arrORE_DUE[] = {
  100,99,98,-1};
int arrORE_TRE[] = {
  73,72,71,-1};
int arrORE_QUATTRO[] = {
  10,9,8,7,6,35,34-1};
int arrORE_CINQUE[] = {
  16,45,44,43,42,41,-1};
int arrORE_SEI[] = {
  21,22,23,-1};
int arrORE_SETTE[] = {
  91,92,93,94,95,-1};
int arrORE_OTTO[] = {
  34,33,32,31,-1};
int arrORE_NOVE[] = {
  11,12,13,14,-1};
int arrORE_DIECI[] = {
  36,37,38,39,40,-1};
int arrORE_UNDICI[] = {
  25,46,47,48,49,50,-1};
int arrORE_DODICI[] = {
  20,19,18,17,16,45,-1};

//congiunzioni
int arrMENO[] = {
  67,68,69,70,-1};
int arrE_CONG[] = {
  68,-1};

//minuti
//int minuti;
int arrMIN_UNO[] = {
  1,-1};
int arrMIN_DUE[] = {
  30,-1};
int arrMIN_TRE[] = {
  51-1};
int arrMIN_QUATTRO[] = {
  80,-1};
int arrMIN_CINQUE[] = {
  85,56,57,58,59,60,-1};

int arrMIN_DIECI[] = {
  82,83,84,85,56,-1};

int arrMIN_QUINDICI[] = {
  79,78,77,76,55,54,53,52,-1};

int arrMIN_VENTI[] = {
  90,89,88,87,86,-1};

int arrMIN_TRENTA[] = {
  65,64,63,62,61,-1};
int arrMIN_QUARANTA[] = {
  112,113,114,115,116,117,118,119,-1};

int arrMIN_CINQUANTA[] = {
  96,97,98,99,100,101,102,103,104,-1};

int arrMIN_QUARTO[] = {
  79,78,77,76,55,54,53,52,-1};
int arrMIN_MEZZA[] = {
  65,64,63,62,61,-1};


void setup() {
  //inizializza led strip
  strip.begin();
  strip.setBrightness(10);
  strip.show(); //initializa all pixel to 'off'

  pinMode(buttonSET_Pin, INPUT);
  pinMode(buttonUP_Pin, INPUT);
  pinMode(buttonDOWN_Pin, INPUT);

  //inizializza NTP
  timeClient.begin();


  Serial.begin(9600);
  Wire.begin();

     delay(3000); // wait for console opening

}

void text(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop () {
  timeClient.update();  //recupero l'ora corrente dal server NTP
  now = timeClient.getHours();

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

  print_minuti(minuti);
  print_ore(ore);

  //accendo la strip nei punti programmati
  strip.show();
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
      paintWord(arrMIN_QUATTRO,0);
      paintWord(arrMIN_UNO,0);
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
    paintWord(arrORE_DODICI,-1);
    break;
  case 1:
//  paintWord(arrE_ACC,0);
    paintWord(arrORE_DODICI,0);
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
    paintWord(arrORE_DODICI,-1);
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
