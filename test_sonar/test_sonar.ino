#include <NewPing.h>

NewPing sonar(36, 38, 40);

unsigned long INTERVAL = 100;
unsigned long PREVIOUS_MILLIS=0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println(getSensorDistance());
}

int getSensorDistance(){
    int GET_SENSOR_DISTANCE;
    unsigned long CURRENT_MILLIS = millis();

  //get the sensor distance for every 200 millis
  if (CURRENT_MILLIS - PREVIOUS_MILLIS >= INTERVAL) {
    PREVIOUS_MILLIS = CURRENT_MILLIS;
    
    return GET_SENSOR_DISTANCE=sensorDistance();
    }
  }


int sensorDistance(){
  int distance;
  int uS = sonar.ping();
  distance = uS / US_ROUNDTRIP_CM;
  return distance;
  }
