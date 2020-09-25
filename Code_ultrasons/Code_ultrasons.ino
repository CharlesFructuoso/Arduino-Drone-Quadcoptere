#include <Metro.h>

#define TRIGG 36 // Broche TRIGGER
#define ECHO_AV 38    // Broche ECHO
#define ECHO_AR 40    // Broche ECHO
#define ECHO_DR 48    // Broche ECHO
#define ECHO_GA 46    // Broche ECHO
                                               // definition du Timeout
Metro sensor_AV = Metro(125);
Metro sensor_AR= Metro(150);
Metro sensor_DR= Metro(175);
Metro sensor_GA= Metro(200);
const long TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s

float son= 340.0 / 1000; //vitesse du son dans l'air (mm/µs)
int mesure;
int mesure1;
int mesure2;

void setup(){
  pinMode(TRIGG, OUTPUT);  //Configuration des broches
  digitalWrite(TRIGG, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_AV, INPUT);
  pinMode(ECHO_AR, INPUT);
  pinMode(ECHO_DR, INPUT);
  pinMode(ECHO_GA, INPUT);
}

void loop(){
  
  digitalWrite(TRIGG, HIGH); // Lance une mesure de distance en envoyant 
  delayMicroseconds(10);  //une impulsion HIGH de 10µs sur la broche TRIGGER
  digitalWrite(TRIGG, LOW);
    if(sensor_AV.check()){
  mesure_AV = pulseIn(ECHO_AV, HIGH, TIMEOUT); // Mesure le temps entre 
                                          // l'envoi de l'ultrason et sa réception
    }
    if(sensor_AR.check()){
  mesure_AR = pulseIn(ECHO_AR, HIGH, TIMEOUT); // Mesure le temps entre 
                                          // l'envoi de l'ultrason et sa réception
    }
    if(sensor_DR.check()){
  mesure_DR = pulseIn(ECHO_DR, HIGH, TIMEOUT); // Mesure le temps entre 
                                          // l'envoi de l'ultrason et sa réception
    }
    if(sensor_GA.check()){
  mesure_GA = pulseIn(ECHO_GA, HIGH, TIMEOUT); // Mesure le temps entre 
                                          // l'envoi de l'ultrason et sa réception
    }
  float distance_AV = mesure_AV / 2.0 * son; //calcul de la distance grâce au temps
                                      //on divise par 2 car le son fait un aller-retour
  float distance_AR = mesure_AR / 2.0 * son; //calcul de la distance grâce au temps
                                      //on divise par 2 car le son fait un aller-retour
  float distance_DR = mesure_DR / 2.0 * son; //calcul de la distance grâce au temps
                                      //on divise par 2 car le son fait un aller-retour
  float distance_GA = mesure_GA / 2.0 * son; //calcul de la distance grâce au temps
                                      //on divise par 2 car le son fait un aller-retour
                                      
  //The PID set point in degrees per second is determined by the roll receiver input.
  //In the case of deviding by 3 the max roll rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  pid_roll_setpoint = 0;
    
  //We need a little dead band of 16us for better results.
  if(distance_AV > 200 && distance_AV < 600)pid_roll_setpoint=200; 
  else if(distance_AR < 600 && distance_AR > 200)pid_roll_setpoint = -200;
  else if(receiver_input_channel_1 > 1508)pid_roll_setpoint = receiver_input_channel_1 - 1508;
  else if(receiver_input_channel_1 < 1492)pid_roll_setpoint = receiver_input_channel_1 - 1492;
  
  pid_roll_setpoint -= roll_level_adjust;                                   //Subtract the angle correction from the standardized receiver roll input value.
  pid_roll_setpoint /= 3.0;                                                 //Divide the setpoint for the PID roll controller by 3 to get angles in degrees.

  pid_pitch_setpoint = 0;

  //The PID set point in degrees per second is determined by the pitch receiver input.
  //In the case of deviding by 3 the max pitch rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  if(distance_DR > 200 && distance_DR < 600)pid_pitch_setpoint=200;
  else if(distance_GA < 600 && distance_GA > 200)pid_pitch_setpoint = -200;
  else if(receiver_input_channel_2 > 1508)pid_pitch_setpoint = receiver_input_channel_2 - 1508;
  else if(receiver_input_channel_2 < 1492)pid_pitch_setpoint = receiver_input_channel_2 - 1492;
  
  pid_pitch_setpoint -= pitch_level_adjust;                                  //Subtract the angle correction from the standardized receiver pitch input value.
  pid_pitch_setpoint /= 3.0;                                                 //Divide the setpoint for the PID pitch controller by 3 to get angles in degrees.
  
}
