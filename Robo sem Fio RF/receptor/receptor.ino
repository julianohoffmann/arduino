/*******************************************************/
/* Aula 41 - Robô Rádio Controlado - Receptor          */
/*******************************************************/
#include <VirtualWire.h>
#include <AFMotor.h>

#define velocidade 2000
#define Pino_Receptor 9

byte comando[VW_MAX_MESSAGE_LEN];
byte tamanho_comando = VW_MAX_MESSAGE_LEN;
char recebido;

// Motores nas portas M2 e M3 da Motor Shield
AF_DCMotor Motor_Direita(3);
AF_DCMotor Motor_Esquerda(2);

void setup() {
  vw_set_rx_pin(Pino_Receptor);
  vw_setup(velocidade);
  vw_rx_start();

  Motor_Direita.setSpeed(150);
  Motor_Esquerda.setSpeed(150);
}

void loop() {
  receber();

  // Movimentos
  if (recebido == 'R') mover_para_tras();
  else if (recebido == 'F') mover_para_frente();
  else if (recebido == 'E') virar_a_esquerda();
  else if (recebido == 'D') virar_a_direita();
  else if (recebido == 'N') permanecer_parado();

  // Velocidades
  if (recebido == '1') { Motor_Direita.setSpeed(130); Motor_Esquerda.setSpeed(130); }
  else if (recebido == '2') { Motor_Direita.setSpeed(155); Motor_Esquerda.setSpeed(155); }
  else if (recebido == '3') { Motor_Direita.setSpeed(200); Motor_Esquerda.setSpeed(200); }
  else if (recebido == '4') { Motor_Direita.setSpeed(255); Motor_Esquerda.setSpeed(255); }
}

void receber() {
  if (vw_get_message(comando, &tamanho_comando)) {
    recebido = comando[0];
  }
}

void mover_para_frente() {
  Motor_Direita.run(FORWARD);
  Motor_Esquerda.run(FORWARD);
}

void permanecer_parado() {
  Motor_Direita.run(RELEASE);
  Motor_Esquerda.run(RELEASE);
}

void mover_para_tras() {
  Motor_Direita.run(BACKWARD);
  Motor_Esquerda.run(BACKWARD);
}

void virar_a_direita() {
  Motor_Direita.run(BACKWARD);
  Motor_Esquerda.run(FORWARD);
}

void virar_a_esquerda() {
  Motor_Direita.run(FORWARD);
  Motor_Esquerda.run(BACKWARD);
}
