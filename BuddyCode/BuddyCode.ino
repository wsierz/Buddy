/*
    Code by Will Sierzputowski
    Jan. 30, 2021

 Soruces of code:
  Some code (setup code) from
    Heart beat plotting!
    By: Nathan Seidle @ SparkFun Electronics
    Date: October 20th, 2016

  Ardunio Setup
  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  LED Blue+ leg = D2
  LED Red+ leg = d#

*/

#include <Wire.h>
#include "MAX30105.h"

MAX30105 particleSensor;

long max1 = 0;
long max2 = 0;
long tmp = 0;
unsigned long tmpTime = 0;

unsigned long max1Time = 0;
unsigned long max2Time = 0;

int i = 0;
unsigned long store[4] = {0, 0, 0, 0};
unsigned long avgTime = 0;

void setup()
{
  Serial.begin(115200);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    while (1);
  }

  //Setup to sense
  byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
  byte sampleAverage = 8; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  //initialize max1 to give a starting value
  max1 = 120000;
  max1Time = millis();
}

void loop()
{
  //Save info at top of cycle for use later to keep timing consitent
  tmp = particleSensor.getIR();
  tmpTime = millis();

  //Checks to see if a peak was found (i.e. ebnsure it was a wave peak and not just the peak of a spike. If peak is found, 
  //store delta time between peaks and calcuate the delta, adding to a running total of the past 4 deltas. Then 
  //prepare to accept the next peak data. 
  if (tmp <= max2 - 200) {
    
    //calculate delta time
    unsigned long beatTime = max2Time - max1Time;

    //store delta time in buffer
    store[i%4] = beatTime;
    i++;

    //Averages delta times stored in buffer
    avgTime = 0;
    for (int j = 0; j< 4; j++) {
      avgTime += store[j];
    }
    avgTime = avgTime / 4;

    //setup max 1 to store the last found peak, so that the following peak has a place to go. 
    max1 = max2;
    max1Time = max2Time;
    max2 = 0;
    max2Time = 0;
  }
  
  //If the peak before was a false peak, update with the new one then wait until the peak is confimed. 
  else if (tmp > max2){
    max2 = tmp;
    max2Time = tmpTime;
  }

//Run light calculations (decide on the light to show)
  if (avgTime < 600) {
    //red light
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
  }
  else {
    //green light
    digitalWrite(3, HIGH);
    digitalWrite(2, LOW);
  }
}
