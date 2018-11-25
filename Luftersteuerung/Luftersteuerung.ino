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
#include "cactus_io_BME280_I2C.h"

// Für den Bildschirm
#define OLED_RESET 4
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


bool Sensorauslesen();
void Bildschirm();
bool Schalten(int Luefterstatus);
bool Pruefen();


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
  if(DEBUG){debug=analogRead(DEBUGPIN);}
    
  Sensorauslesen();
  Luefterstatus=Pruefen();
  Luefterstatus=Schalten(Luefterstatus);
  Bildschirm();
  delay(100);
}


bool Sensorauslesen ()
{
  Aussensensor.readSensor();  
  DruckAussen=  Aussensensor.getPressure_MB();
  FeuchtigkeitAussen= Aussensensor.getHumidity();
  TemperaturAussen=Aussensensor.getTemperature_C();
  Innensensor.readSensor();  
  DruckInnen=  Innensensor.getPressure_MB();
  FeuchtigkeitInnen= Innensensor.getHumidity();
  TemperaturInnen=Innensensor.getTemperature_C();

  return true;
}

void Bildschirm()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(4,4);
  display.print("L:");
  display.println(Luefterstatus);
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
