/********************************************************/ 
/* Aula 41  - Robô radiocontrolado - Transmissor        */
/* Joystick shield + nRF24L01                           */
/********************************************************/
#include <RF24.h>

/* Endereço de comunicação (igual ao receptor) */
const byte endereco[6] = "00008"; 

/* Pinos do rádio */
#define CE 9
#define CSN 10

/* Pinos do joystick e botões */
#define eixoX A0
#define eixoY A1
#define botaoA 2
#define botaoB 3
#define botaoC 4
#define botaoD 5

RF24 radio(CE, CSN);

/* Comandos */
#define CMD_PARAR    'P'
#define CMD_FRENTE   'F'
#define CMD_RE       'R'
#define CMD_DIREITA  'D'
#define CMD_ESQUERDA 'E'
#define CMD_POT1     '1'
#define CMD_POT2     '2'
#define CMD_POT3     '3'
#define CMD_POT4     '4'

char ultimoComando = CMD_PARAR;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(endereco);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(108);
  radio.stopListening();

  pinMode(botaoA, INPUT_PULLUP);
  pinMode(botaoB, INPUT_PULLUP);
  pinMode(botaoC, INPUT_PULLUP);
  pinMode(botaoD, INPUT_PULLUP);
}

void loop() {
  int joyX = analogRead(eixoX);
  int joyY = analogRead(eixoY);
  char comando = CMD_PARAR;

  // Botões de potência mantidos
  if (digitalRead(botaoA) == LOW) comando = CMD_POT1;
  else if (digitalRead(botaoB) == LOW) comando = CMD_POT2;
  else if (digitalRead(botaoC) == LOW) comando = CMD_POT3;
  else if (digitalRead(botaoD) == LOW) comando = CMD_POT4;

  // Direções (somente frente e ré foram invertidas)
  else if (joyY > 700) comando = CMD_RE;          // invertido (antes era FRENTE)
  else if (joyY < 300) comando = CMD_FRENTE;      // invertido (antes era RÉ)
  else if (joyX > 700) comando = CMD_DIREITA;
  else if (joyX < 300) comando = CMD_ESQUERDA;

  // Envia só se mudou
  if (comando != ultimoComando) {
    radio.write(&comando, sizeof(comando));
    Serial.print("Comando enviado: ");
    Serial.println(comando);
    ultimoComando = comando;
  }

  delay(50); // reduz tráfego no rádio
}
