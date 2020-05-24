#include <Servo.h>

#define pinServo1 10
#define pinServo2 9

Servo ServoA;
Servo ServoB;

char comando;
int posicion;

void setup (){
  ServoA.attach(pinServo1);
  ServoB.attach(pinServo2);
  ServoA.write(0);
  ServoB.write(0);
  
  Serial.begin(9600);
}

void loop(){
  leer_Dato();
  
  switch(comando){
    case 'a':
      ServoA.write(posicion);
      comando = ' ';
      break;
     case 'b':
      ServoB.write(posicion);
      comando = ' ';
      break;
  }
}
  void leer_Dato(){
    if (Serial.available()>0){
      comando = Serial.read();
      posicion = Serial.parseInt();
    }
    Serial.flush();
  }  

    
 
 
