#include <Servo.h>
#include <SoftwareSerial.h>

const int trigPin = 9;
const int echoPin = 10;
const int servoPin = 11;
long duration;
int distance;
int angle = 0;
bool goingUp = true;

Servo servoMotor;

// Comunicação com ESP-01 (RX=2, TX=3)
SoftwareSerial espSerial(2, 3);

void setup() {
  Serial.begin(9600);      // Comunicação com PC (Processing)
  espSerial.begin(9600);   // Comunicação com ESP-01
  servoMotor.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Movimento de varredura do radar
  if (goingUp) {
    angle++;
    if (angle >= 180) goingUp = false;
  } else {
    angle--;
    if (angle <= 0) goingUp = true;
  }

  servoMotor.write(angle);
  delay(15);

  distance = getDistance();
  if (distance > 40) distance = 40; // Limite máximo de leitura

  // --- Envio via USB (para Processing) ---
  Serial.print(angle);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(".");

  // --- Envio via ESP-01 ---
  espSerial.print(angle);
  espSerial.print(",");
  espSerial.print(distance);
  espSerial.print(".");
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}
