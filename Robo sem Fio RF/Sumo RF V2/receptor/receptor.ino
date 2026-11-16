#include <VirtualWire.h>

#define VELOCIDADE 2000
#define PINO_RECEPTOR 9

// Pinos L298N (com jumpers em ENA/ENB)
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

byte comando[VW_MAX_MESSAGE_LEN];
byte tamanho_comando = VW_MAX_MESSAGE_LEN;
char recebido;

void setup() {
  vw_set_rx_pin(PINO_RECEPTOR);
  vw_setup(VELOCIDADE);
  vw_rx_start();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  parar();
}

void loop() {
  receber();

  if (recebido == 'R') mover_para_tras();
  else if (recebido == 'F') mover_para_frente();
  else if (recebido == 'E') virar_a_esquerda();
  else if (recebido == 'D') virar_a_direita();
  else if (recebido == 'N') parar();
}

void receber() {
  if (vw_get_message(comando, &tamanho_comando)) recebido = comando[0];
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
