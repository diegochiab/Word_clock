/* 
 * Diego Word Clock 10x10
 * 
 * This clock is built to display spelled out words depending on the time of day.
 * The code depends on both an RTC and Addressable RGB LEDs
 * 
 * RGB LEDs used: WS2812B LEDs on a 5v strip connected to pin D5
 *
 */
 
 //ESP8266
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
 
 //NTP
#include <NTPClient.h>
 
//LEDS
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

/*** DEF & CONST***/
//ESP8266
const char* ssid = "5G_test"; //ssid della rete
const char* password = "Matildaseimitica$"; // password della rete
// const char* ssid ="Fablab_5G";            
// const char* password = "34935135";  

//NTP
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);
int oraAttuale = 0;

#define RGBLEDPIN    D5

#define N_LEDS 100 // 10 x 10 grid
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define BRIGHTNESSDAY 15
#define BRIGHTNESSNIGHT 10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(N_LEDS, RGBLEDPIN, NEO_GRB + NEO_KHZ800);

// a few vars to keep the peace;
int intBrightness = BRIGHTNESSDAY; // the brightness of the clock (0 = off and 255 = 100%)
String strTime = ""; // used to detect if word time has changed

unsigned long previousMillis = 0;
const long CONST1_SEC = 1000; // Intervallo in millisecondi
const long CONST2_SEC = 2000;
const long CONST5_SEC = 5000; 

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
  1,30,-1};
int arrMIN_TRE[] = {
  1,30,51-1};
int arrMIN_QUATTRO[] = {
  1,30,51,80,-1};
int arrMIN_CINQUE[] = {
  85,56,57,58,59,60,-1};

int arrMIN_DIECI[] = {
  82,83,84,85,56,-1};

int arrMIN_QUINDICI[] = {
  79,78,77,76,55,54,53,52,-1};

int arrMIN_VENTI[] = {
  90,89,88,87,86,-1};

int arrMIN_MEZZA[] = {
  65,64,63,62,61,-1};

void setup() {

  //Connessione WiFi
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println();
  Serial.print("Connessione ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connessa");
  
  //inizializza NTP
  timeClient.begin();
  
  //inizializza pannello led
  pixels.begin();
  pixels.setBrightness(BRIGHTNESSDAY);
  pixels.show(); //inizializza tutti i pixel a 'off'

  delay(3000); // wait for console opening
}

void loop () {
  unsigned long currentMillis = millis();

  // recupero l'ora corrente e aggiorno l'orologio solo ogni INTERVAL
  if (currentMillis - previousMillis >= CONST5_SEC) {
    timeClient.update();  //recupero l'ora corrente dal server NTP
    mostra_tempo();
    previousMillis = currentMillis;
  }
}

void mostra_tempo(){
  int ore = timeClient.getHours();
  int minuti = timeClient.getMinutes();
  int secondi= timeClient.getSeconds();

  Serial.print("ora=");
  Serial.print(ore);
  Serial.print(':');
  Serial.print(minuti);
  Serial.print(' ');
  Serial.print(secondi);
  Serial.println();

  print_minuti(minuti);
  print_ore(ore);

  //accendo la strip nei punti programmati
  pixels.show();
}

void print_minuti(int minuti){
  int decine=minuti/10;
  int unita=minuti % 10;

  //azzero le congiunzioni
  paintWord(arrE_CONG,0);
  paintWord(arrMENO,0);
  
  //controllo se sono prima della mezz'ora o dopo
  if (minuti <= 30){
    paintWord(arrE_CONG,-1);
    paintWord(arrMENO,0);
  }
  else{
    paintWord(arrE_CONG,0);
    paintWord(arrMENO,-1);
  }
  
  //accendo i multipli di 5 minuti
  switch (minuti){
    case 0 ... 4:
    case 56 ... 59:
      paintWord(arrMIN_CINQUE,0);
      break;
    case 5 ... 9:
    case 51 ... 55:
      paintWord(arrMIN_DIECI,0);
      paintWord(arrMIN_CINQUE,-1);
      break;
    case 10 ... 14:
    case 46 ... 50:
      paintWord(arrMIN_QUINDICI,0);
      paintWord(arrMIN_CINQUE,0);
      paintWord(arrMIN_DIECI,-1);
      break;
    case 15 ... 19:
    case 41 ... 45:
      paintWord(arrMIN_VENTI,0);
      paintWord(arrMIN_DIECI,0);
      paintWord(arrMIN_QUINDICI,-1);
      break;
    case 20 ... 24:
    case 36 ... 40:
      paintWord(arrMIN_CINQUE,0);
      paintWord(arrMIN_QUINDICI,0);
      paintWord(arrMIN_VENTI,-1);
      break;
    case 25 ... 29:
    case 31 ... 35:
      paintWord(arrMIN_MEZZA,0);
      paintWord(arrMIN_VENTI,-1);
      paintWord(arrMIN_CINQUE,-1);
      break;
    case 30:
      paintWord(arrMIN_VENTI,0);
      paintWord(arrMIN_CINQUE,0);
      paintWord(arrMIN_MEZZA,-1);
      break;
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
    paintWord(arrORE_DODICI,0);
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

void paintWord(int arrWord[], uint32_t intColor){
  for(int i = 0; i < pixels.numPixels() + 1; i++){
    if(arrWord[i] == -1){
      pixels.show();
      break;
    }
    else{
      pixels.setPixelColor(arrWord[i],intColor);
      //      Serial.print(arrWord[i]);
      //      Serial.print(':');
      //      Serial.println(intColor);
    }
  }
}
