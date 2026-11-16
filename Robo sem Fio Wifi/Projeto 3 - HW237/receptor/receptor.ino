/*******************************************************/
/* Aula 41  - Robô radiocontrolado - Receptor          */
/* Robô com chassi 2WD, nRF24L01 e ponte H             */
/*******************************************************/
#include <RF24.h>

/* Endereço de comunicação (igual ao transmissor) */
const byte endereco[6] = "00006";

/* Definições dos pinos da ponte H (PWM) */
#define IN1 10
#define IN2 9
#define IN3 4
#define IN4 3

/* Definições dos LEDs estrobo */
#define LED1 7
#define LED2 8

/* Potência inicial dos motores */
int potA = 0;
int potB = 0;

/* Pinos do módulo rádio */
#define CE 6
#define CSN 5

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

char comando = CMD_PARAR;
unsigned long ultimoComando = 0;

/* Controle do estrobo */
unsigned long ultimoPisca = 0;
const int intervaloPisca = 100; // ms -> 10 vezes por segundo
bool estadoLed = false;

/* === Funções de movimento === */
void Frente() {
  analogWrite(IN1, potA);
  analogWrite(IN2, 0);
  analogWrite(IN3, potB);
  analogWrite(IN4, 0);
}

void Re() {
  analogWrite(IN1, 0);
  analogWrite(IN2, potA);
  analogWrite(IN3, 0);
  analogWrite(IN4, potB);
}

void GiroHorario() {
  analogWrite(IN1, potA);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, potB);
}

void GiroAntiHorario() {
  analogWrite(IN1, 0);
  analogWrite(IN2, potA);
  analogWrite(IN3, potB);
  analogWrite(IN4, 0);
}

void Pare() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

/* Função estrobo */
void piscaEstrobo() {
  unsigned long agora = millis();
  if (agora - ultimoPisca >= intervaloPisca) {
    ultimoPisca = agora;
    estadoLed = !estadoLed;
    digitalWrite(LED1, estadoLed);
    digitalWrite(LED2, !estadoLed); // pisca invertido
  }
}

void setup() {
  Pare();
  Serial.begin(9600);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT); 

  radio.begin();
  radio.openReadingPipe(1, endereco);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(108);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&comando, sizeof(comando));
    ultimoComando = millis();

    Serial.print("Comando recebido: ");
    Serial.println(comando);
  }

  // Timeout: para se não receber nada em 1s
  if (millis() - ultimoComando > 10000) {
    Pare();
  } else {
    switch (comando) {
      case CMD_POT1: potA = 100; potB = 100; break;
      case CMD_POT2: potA = 150; potB = 150; break;
      case CMD_POT3: potA = 200; potB = 200; break;
      case CMD_POT4: potA = 255; potB = 255; break;
      case CMD_FRENTE:   Frente(); break;
      case CMD_DIREITA:  GiroHorario(); break;
      case CMD_ESQUERDA: GiroAntiHorario(); break;
      case CMD_RE:       Re(); break;
      default:           Pare(); break;
    }
  }

  /* Mantém os LEDs piscando sempre*/
  piscaEstrobo();
}
