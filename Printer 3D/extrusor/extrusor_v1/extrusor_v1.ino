/*
==========================================
EXTRUSOR PARA IMPRESSORA 3D
Arduino dedicado

L298
==========================================
*/

#define ENA 5
#define IN1 6
#define IN2 7

// velocidade do motor (0~255)
const int velocidade = 90;

void setup() {

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pararMotor();

  // aguarda aquecimento
  delay(5000);
}

void loop() {

  empurrarFilamento(250);

  delay(180);

}

void empurrarFilamento(int tempo){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  analogWrite(ENA,velocidade);

  delay(tempo);

  pararMotor();
}

void pararMotor(){

  analogWrite(ENA,0);

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
}