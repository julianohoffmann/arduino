#include <RH_ASK.h>
#include <SPI.h>

#define VELOCIDADE 2001
#define PINO_TX 10

RH_ASK driver(VELOCIDADE, 0, PINO_TX, 0);

int JoyX, JoyY;
int estadoB1, estadoB2, estadoB3, estadoB4;

void setup() {
  driver.init();

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
}

void loop() {
  JoyX = analogRead(A0);
  JoyY = analogRead(A1);
  estadoB1 = digitalRead(2);
  estadoB2 = digitalRead(3);
  estadoB3 = digitalRead(4);
  estadoB4 = digitalRead(5);

  if (JoyX < 300) enviar("E");
  else if (JoyX > 700) enviar("D");
  else if (JoyY < 300) enviar("R");
  else if (JoyY > 700) enviar("F");
  else enviar("N");

  if (estadoB1 == LOW) enviar("4");
  if (estadoB2 == LOW) enviar("3");
  if (estadoB3 == LOW) enviar("2");
  if (estadoB4 == LOW) enviar("1");

  delay(100);
}

void enviar(const char *msg) {
  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
}
