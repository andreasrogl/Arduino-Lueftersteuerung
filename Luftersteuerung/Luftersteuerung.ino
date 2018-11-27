#include <cactus_io_BME280_I2C.h>

/*********************************************************************
// Lüfersteuerung basierend auf Temperatur, Druck, Feuchtigkeit
/*
 * V0_02  Bildschirm gibt Werte aus und Relais schaltet basierend auf Schalter
 *        Temperatursensoren werden ausgelesen und ausgegeben
 * 
 * 
 */
/**************************************************************************/
//Eine Teständerung
// Für den Bildschirm
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Für den Bildschirm
#define OLED_RESET 4
#define RTC_I2C_ADDRESS 0x68 // I2C Adresse des RTC  DS323
Adafruit_SSD1306 display(OLED_RESET);  //Bildschirm
BME280_I2C Aussensensor(0x76);         //Temperatursensor
BME280_I2C Innensensor(0x77);  

//PINBELEGUNG 
// Relais(Lüftung an Pin1)
//
//

#define RELAIS 8

//Debugvariable
#define DEBUG 1
#define DEBUGPIN A0
int debug = 1;



// Für den Bildschirm
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

/*
* Variablendefinition
*/
int Luefterstatus=0;   //0 Für Lüftung aus, 1 für Lüftung an.
double DruckAussen =0;
double TemperaturAussen =0;
double FeuchtigkeitAussen =0;           
double DruckInnen =0;
double TemperaturInnen =0;
double FeuchtigkeitInnen =0;   
int jahr,monat,tag,stunde,minute,sekunde, wochentag;

bool Sensorauslesen();
void Bildschirm();
bool Schalten(int Luefterstatus);
bool Pruefen();
void rtcReadTime();
byte bcdToDec(byte val);
void Testfunktion();

void setup()   {                
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Bildschirm initialize with the I2C addr 0x3C (for the 128x32)
  if (!Aussensensor.begin()) {
    Serial.println("Es konnte kein BME280 Aussen Sensor gefunden werden!");
    Serial.println("Bitte überprüfen Sie die Verkabelung!");
    while (1);
  }
  if (!Innensensor.begin()) {
    Serial.println("Es konnte kein BME280 InnenSensor gefunden werden!");
    Serial.println("Bitte überprüfen Sie die Verkabelung!");
    while (1);
  }
 
  Aussensensor.setTempCal(-1);
  Innensensor.setTempCal(0);
 //Pinbelegung 
  pinMode(RELAIS,OUTPUT);

}


void loop() {
  // draw a white circle, 10 pixel radius
  if(DEBUG){
    debug=analogRead(DEBUGPIN);
    Testfunktion();
    }
    
  Sensorauslesen();
  Luefterstatus=Pruefen();
  Luefterstatus=Schalten(Luefterstatus);
  Bildschirm();
  delay(100);
}


bool Sensorauslesen ()
{
  // Aussensensor  
  Aussensensor.readSensor();  
  DruckAussen=  Aussensensor.getPressure_MB();
  FeuchtigkeitAussen= Aussensensor.getHumidity();
  TemperaturAussen=Aussensensor.getTemperature_C();

  //Innensensor
  Innensensor.readSensor();  
  DruckInnen=  Innensensor.getPressure_MB();
  FeuchtigkeitInnen= Innensensor.getHumidity();
  TemperaturInnen=Innensensor.getTemperature_C();

  // Uhrzeit
  rtcReadTime();

  return true;
}

void Bildschirm()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //Erste Zeile
  display.setCursor(4,4);
  display.print("L:");
  display.print(Luefterstatus);
  display.print(" T:");
  display.print(tag);
  display.print(".");
  display.print(monat);
  //display.print(".");
  //display.print(jahr);
  display.print(" ");
  display.print(stunde);
  display.print(":");
  display.print(minute);
  display.print(":");
  display.println(sekunde);
  //Zweite Zeile
  display.setCursor(4,14);
  display.print("A:");
  display.print (TemperaturAussen);
  display.print("C ");
  display.print (int(FeuchtigkeitAussen));
  display.print("% ");
  display.print (DruckAussen);
  display.println("m");  
  display.setCursor(4,24);
  display.print("I:");
  display.print (TemperaturInnen);
  display.print("C ");
  display.print (int(FeuchtigkeitInnen));
  display.print("% ");
  display.print (DruckInnen);
  display.println("m");  
  display.display();
  
  Serial.print(Luefterstatus);
  Serial.println(debug);
}

bool Schalten(int Luefterstatus)
{
    if (Luefterstatus)
    {
      digitalWrite(RELAIS,HIGH);
      return true;
    }
    else
    {
      digitalWrite(RELAIS,LOW);
      return false;
    }
    
}


bool Pruefen()
{
    if(DEBUG){
      debug=analogRead(DEBUGPIN);
      if (debug>500)
      {
        return true;
       }
      else
      {
        return false;
      }
    }
}


//Liest die Zeit von der Digitalen Uhr
void rtcReadTime()
{
   
  Wire.beginTransmission(RTC_I2C_ADDRESS); //Aufbau der Verbindung zur Adresse 0x68
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(RTC_I2C_ADDRESS, 7);
    sekunde    = bcdToDec(Wire.read() & 0x7f);
  minute     = bcdToDec(Wire.read()); 
  stunde     = bcdToDec(Wire.read() & 0x3f); 
  //Der Wochentag wird hier nicht ausgelesen da dieses mit 
  //dem Modul RTC DS3231 nicht über die Wire.h zuverlässig funktioniert.
  /* wochentag  =*/ bcdToDec(Wire.read());
  tag        = bcdToDec(Wire.read());
  monat      = bcdToDec(Wire.read());
  jahr       = bcdToDec(Wire.read())+2000;   
   
}
//Convertiert binäre Zeichen in Dezimal Zeichen.
byte bcdToDec(byte val){
  return ( (val/16*10) + (val%16) );
}

void Testfunktion()
{
  Serial.print(stunde);
 Serial.print(" ");
 Serial.print(minute);
 Serial.print(" ");
 Serial.print(sekunde);
 Serial.print("   ");
 Serial.print(tag);
 Serial.print(" ");
 Serial.print(monat);
 Serial.print(" ");
 Serial.print(jahr);
 Serial.println(" ");

}
