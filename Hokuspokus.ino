#include <SoftwareSerial.h>
#include <LowPower.h>
#define DEBUG

#include "PowerControledThingspeakConnection.h"
// Controll pins
#define GNDCTRLPIN 12
#define VCC2TRLPIN 9
#define MODEM_RESET_PIN 4

#define NUM_TRANSMISSION_RETRIES 5
#define DELAY_AFTER_TRANSMISSION_FAILURE 60*60*1 //in seconds
#define REGULAR_UPDATE_INTERVAL 60*60*6 //in seconds

//http://api.thingspeak.com/update?api_key=YDHVCBHPKX9TOPXF&field1=BATTERY&field2=SENSOR1&field3=SENSOR2
SoftwareSerial modemSerial(8, 3); // RX, TX

PowerControledThingspeakConnection modem(modemSerial, GNDCTRLPIN, VCC2TRLPIN, MODEM_RESET_PIN);

void setup()
{
  Serial.begin(9600);
  modemSerial.begin(9600);
//  while (!Serial) {} //Needs to be commented out when running in "headless" mode (without a serial monitor open)
  enableSensors();
}

unsigned long elapsedSeconds = 0;
unsigned long nextOutgoingUpdate = 0;

void loop() {
//  modem.manualModemControlLoop();
    if(elapsedSeconds >= nextOutgoingUpdate || sensorValueChanged()) {
        Serial.println("TIME TO SEND NEW DADA!!!___________________________________________");
        if(modem.pushToThingSpeak(NUM_TRANSMISSION_RETRIES,
                                  batteryVoltage(),\
                                  sensorValue(0),
                                  sensorValue(1),
                                  elapsedSeconds,
                                  nextOutgoingUpdate,
                                  elapsedSeconds + REGULAR_UPDATE_INTERVAL)) {
            nextOutgoingUpdate = elapsedSeconds + REGULAR_UPDATE_INTERVAL;
        } else {
            nextOutgoingUpdate = elapsedSeconds + DELAY_AFTER_TRANSMISSION_FAILURE;
        }
    }
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
//    delay(8000);
    elapsedSeconds += 8;
}
