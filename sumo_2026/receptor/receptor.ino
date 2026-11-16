#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN

const byte endereco[6] = "SUMO1";

// -------------------------
// L298N
// -------------------------

const int ENA = 5;
const int IN1 = 4;
const int IN2 = 3;

const int ENB = 6;
const int IN3 = 2;
const int IN4 = 10;

// -------------------------
// DADOS
// -------------------------

struct DadosControle {

  int16_t x;
  int16_t y;

  uint8_t botao;

  uint8_t modo;

  uint16_t contador;
};

DadosControle dados;

// -------------------------
// FAILSAFE
// -------------------------

unsigned long ultimoPacote = 0;

const unsigned long TEMPO_FAILSAFE = 250;

// -------------------------
// LIMITES
// -------------------------

const int PWM_MAX = 255;

const int PWM_MIN_MOVIMENTO = 55;

// -------------------------
// SETUP
// -------------------------

void setup() {

  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pararMotores();

  radio.begin();

  radio.setChannel(108);

  radio.setDataRate(RF24_250KBPS);

  radio.setPALevel(RF24_PA_HIGH);

  radio.setRetries(5, 15);

  radio.openReadingPipe(1, endereco);

  radio.startListening();

  ultimoPacote = millis();

  Serial.println("RECEPTOR SUMO");

}

// -------------------------
// LOOP
// -------------------------

void loop() {

  if (radio.available()) {

    radio.read(&dados, sizeof(dados));

    ultimoPacote = millis();

    // Botão do joystick = parada de emergência

    if (dados.botao == 1) {

      pararMotores();

      return;
    }

    controlarMotores(dados.x, dados.y);

  }

  // FAILSAFE

  if (millis() - ultimoPacote > TEMPO_FAILSAFE) {

    pararMotores();

  }

}

// -------------------------
// CONTROLE DOS MOTORES
// -------------------------

void controlarMotores(int x, int y) {

  /*
     Y = avanço / ré
     X = direção
  */

  float frente = y / 512.0;

  float direcao = x / 512.0;

  frente = constrain(frente, -1.0, 1.0);

  direcao = constrain(direcao, -1.0, 1.0);

  // Curva de resposta

  frente = curva(frente);

  direcao = curva(direcao);

  // Mistura diferencial

  float esquerda = frente + direcao;

  float direita = frente - direcao;

  // Normalização

  float maior = max(abs(esquerda), abs(direita));

  if (maior > 1.0) {

    esquerda /= maior;

    direita /= maior;

  }

  int pwmEsquerda = esquerda * PWM_MAX;

  int pwmDireita = direita * PWM_MAX;

  moverMotorEsquerdo(pwmEsquerda);

  moverMotorDireito(pwmDireita);

}

// -------------------------
// CURVA DE RESPOSTA
// -------------------------

float curva(float valor) {

  if (valor > 0) {

    return valor * valor;

  }

  else {

    return -(valor * valor);

  }

}

// -------------------------
// MOTOR ESQUERDO
// -------------------------

void moverMotorEsquerdo(int velocidade) {

  velocidade = constrain(velocidade, -255, 255);

  if (velocidade > 0) {

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    analogWrite(ENA, velocidade);

  }

  else if (velocidade < 0) {

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    analogWrite(ENA, -velocidade);

  }

  else {

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    analogWrite(ENA, 0);

  }

}

// -------------------------
// MOTOR DIREITO
// -------------------------

void moverMotorDireito(int velocidade) {

  velocidade = constrain(velocidade, -255, 255);

  if (velocidade > 0) {

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    analogWrite(ENB, velocidade);

  }

  else if (velocidade < 0) {

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    analogWrite(ENB, -velocidade);

  }

  else {

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    analogWrite(ENB, 0);

  }

}

// -------------------------
// PARADA
// -------------------------

void pararMotores() {

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

}