/**
 * Code d'exemple pour la fonction pulseIn()
 */
#include <Servo.h>
/** Broche d'entrée du signal */
//int input_THROTTLE = 1000;
//int x=0;
Servo L_B_prop;
/** Fonction setup() */
void setup() {
  L_B_prop.attach(3);
  // Configure le port série pour l'exemple
  Serial.begin(9600);

  // Met la broche de signal en entrée
 pinMode(10, INPUT);
}

/** Fonction loop() */
void loop() {
// while (Serial.available()){
 // x=Serial.read();
 // if (x=='1')
//input_THROTTLE += 10; 
 // else if(x=='2')
//input_THROTTLE -= 10;  
 // }
  // Mesure la durée de l'impulsion haute (timeout par défaut de 1s)
 noInterrupts();
  unsigned long duration = pulseIn(10, HIGH);
  interrupts();
  L_B_prop.writeMicroseconds(duration);
  // Affiche la durée de l'impulsion (en us) sur le port série
  Serial.println(duration);
}
