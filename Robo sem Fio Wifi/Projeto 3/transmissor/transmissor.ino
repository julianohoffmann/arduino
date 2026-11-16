#include <RF24.h>
#include <SPI.h>

RF24 radio(9, 10);
const byte pipe[6] = "00001";

void setup() {
  Serial.begin(9600);

  if (!radio.begin()) {
    Serial.println("NRF NÃO INICIOU!");
    while (1);
  }

  radio.openWritingPipe(pipe);
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(76);
  radio.stopListening();
}

void loop() {
  const char teste[] = "PING";
  bool ok = radio.write(&teste, sizeof(teste));

  if (ok) Serial.println("Enviado!");
  else   Serial.println("Falhou!");

  delay(500);
}
