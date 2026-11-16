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

  radio.openReadingPipe(1, pipe);
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(76);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char texto[32] = "";
    radio.read(&texto, sizeof(texto));
    Serial.print("Recebido: ");
    Serial.println(texto);
  }
}
