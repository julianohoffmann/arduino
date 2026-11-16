#include <RH_ASK.h>
#include <SPI.h> // Necessária para RH_ASK, mesmo sem uso direto

#define VELOCIDADE 2001
#define PINO_RX 10

// Pinos L298N (com ENA/ENB jumpeados)
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

RH_ASK driver(VELOCIDADE, PINO_RX, 0, 0); // velocidade, RX, TX, PTT, inverte = 0

char recebido;

void setup() {
  driver.init();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  parar();
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) {
    recebido = buf[0];
    executarComando(recebido);
  }
}

void executarComando(char cmd) {
  if (cmd == 'F') mover_para_frente();
  else if (cmd == 'R') mover_para_tras();
  else if (cmd == 'E') virar_a_esquerda();
  else if (cmd == 'D') virar_a_direita();
  else if (cmd == 'N') parar();
}

void mover_para_frente() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void mover_para_tras() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void virar_a_direita() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void virar_a_esquerda() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void parar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
