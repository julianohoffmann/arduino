/*******************************************************/
/* Robô Rádio Controlado - Receptor (estável)          */
/*******************************************************/
#include <VirtualWire.h>
#include <AFMotor.h>

#define velocidade 2000
#define Pino_Receptor 9

byte comando[VW_MAX_MESSAGE_LEN];
byte tamanho_comando = VW_MAX_MESSAGE_LEN;
char recebido = 'N';
char ultimo_comando = 'N';

unsigned long ultimoSinal = 0;              
const unsigned long timeout = 400;          // mais seguro que 300ms

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

  // Se parar de receber, o robô para
  if (millis() - ultimoSinal > timeout) {
    recebido = 'N';
  }

  // Só executa se realmente mudou (anti ruído)
  if (recebido != ultimo_comando) {
    processar_comando(recebido);
    ultimo_comando = recebido;
  }
}

void receber() {
  tamanho_comando = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(comando, &tamanho_comando)) {
    recebido = comando[0];
    ultimoSinal = millis();
  }
}

void processar_comando(char c) {
  switch(c) {

    // DIREÇÕES
    case 'F': mover_para_frente();  break;
    case 'R': mover_para_tras();    break;
    case 'E': virar_a_esquerda();   break;
    case 'D': virar_a_direita();    break;
    case 'N': permanecer_parado();  break;

    // VELOCIDADES
    case '1': Motor_Direita.setSpeed(130); Motor_Esquerda.setSpeed(130); break;
    case '2': Motor_Direita.setSpeed(155); Motor_Esquerda.setSpeed(155); break;
    case '3': Motor_Direita.setSpeed(200); Motor_Esquerda.setSpeed(200); break;
    case '4': Motor_Direita.setSpeed(255); Motor_Esquerda.setSpeed(255); break;
  }
}

/********* Movimentação *********/

void mover_para_frente() {
  Motor_Direita.run(FORWARD);
  Motor_Esquerda.run(FORWARD);
}

void mover_para_tras() {
  Motor_Direita.run(BACKWARD);
  Motor_Esquerda.run(BACKWARD);
}

void virar_a_esquerda() {
  Motor_Direita.run(FORWARD);
  Motor_Esquerda.run(BACKWARD);
}

void virar_a_direita() {
  Motor_Direita.run(BACKWARD);
  Motor_Esquerda.run(FORWARD);
}

void permanecer_parado() {
  Motor_Direita.run(RELEASE);
  Motor_Esquerda.run(RELEASE);
}
