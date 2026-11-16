#include <VirtualWire.h>

#define VELOCIDADE 2000
#define PINO_TRANSMISSOR 9

int JoyX;
int JoyY;
int estadoB1, estadoB2, estadoB3, estadoB4;

void setup() {
  vw_set_tx_pin(PINO_TRANSMISSOR);
  vw_setup(VELOCIDADE);

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
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();
}
