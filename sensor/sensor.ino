#include <NewPing.h>

NewPing sonar_AV(36, 38, 50);
NewPing sonar_GA(36, 40, 50);
NewPing sonar_DR(36, 48, 50);

float Sensor[3];
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Sensor[0] = sonar_AV.ping_cm();
  Sensor[1] = sonar_GA.ping_cm();
  Sensor[2] = sonar_DR.ping_cm();
  Serial.println(Sensor[0]);
  Serial.println(Sensor[1]);
  Serial.println(Sensor[2]);
  delay(1000);
  
}
