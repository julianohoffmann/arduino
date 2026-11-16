#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN

const byte endereco[6] = "SUMO1";

// Joystick
const int PIN_X = A0;
const int PIN_Y = A1;
const int PIN_SW = 9;

// Estrutura enviada
struct DadosControle {

  int16_t x;
  int16_t y;

  uint8_t botao;

  uint8_t modo;

  uint16_t contador;
};

DadosControle dados;

unsigned long ultimoEnvio = 0;

const unsigned long intervalo = 15;

void setup() {

  Serial.begin(115200);

  pinMode(PIN_SW, INPUT_PULLUP);

  radio.begin();

  radio.setChannel(108);

  radio.setDataRate(RF24_250KBPS);

  radio.setPALevel(RF24_PA_HIGH);

  radio.setRetries(5, 15);

  radio.openWritingPipe(endereco);

  radio.stopListening();

  dados.x = 0;
  dados.y = 0;
  dados.botao = 0;
  dados.modo = 0;
  dados.contador = 0;

  Serial.println("TRANSMISSOR SUMO");

}

int lerJoystick(int valor) {

  int centro = 512;

  int zonaMorta = 45;

  valor -= centro;

  if (abs(valor) < zonaMorta) {
    return 0;
  }

  valor = constrain(valor, -512, 511);

  return valor;

}

void loop() {

  if (millis() - ultimoEnvio >= intervalo) {

    ultimoEnvio = millis();

    int x = lerJoystick(analogRead(PIN_X));
    int y = lerJoystick(analogRead(PIN_Y));

    /*
       Joystick Shield:

       frente  = Y positivo
       direita = X positivo
    */

    dados.x = x;
    dados.y = y;

    if (digitalRead(PIN_SW) == LOW) {
      dados.botao = 1;
    }
    else {
      dados.botao = 0;
    }

    dados.modo = 0;

    dados.contador++;

    bool sucesso = radio.write(&dados, sizeof(dados));

    Serial.print("X: ");
    Serial.print(dados.x);

    Serial.print(" Y: ");
    Serial.print(dados.y);

    Serial.print(" B: ");
    Serial.print(dados.botao);

    Serial.print(" TX: ");
    Serial.println(sucesso ? "OK" : "FALHA");
  }
}