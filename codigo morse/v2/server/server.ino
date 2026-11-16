#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
SoftwareSerial esp(2, 3);

int botao = 7;
int led = 6;
int buzzer = 5;

String enviado = "";
String recebido = "";
String bufferESP = "";

unsigned long tempoPressionado;
bool pressionado = false;

unsigned long ultimoUpdate = 0;

bool esperandoEnvio = false;

void setup() {
  pinMode(botao, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(115200);
  esp.begin(19200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  configurarESP();
}

void loop() {
  lerESP();
  enviarMorse();

  if (millis() - ultimoUpdate > 150) {
    atualizarDisplay();
    ultimoUpdate = millis();
  }
}

void configurarESP() {
  delay(2000);
  esp.println("AT");
  delay(500);
  esp.println("AT+CWMODE=2");
  delay(500);
  esp.println("AT+CWSAP=\"MORSE\",\"12345678\",5,3");
  delay(2000);
  esp.println("AT+CIPMUX=0");
  delay(500);
  esp.println("AT+CIPSERVER=1,80");
  delay(1000);
}

void lerESP() {
  while (esp.available()) {
    char c = esp.read();

    // detectar prompt de envio
    if (c == '>') {
      esperandoEnvio = true;
    }

    bufferESP += c;

    int ipd = bufferESP.indexOf("+IPD,");
    if (ipd >= 0) {
      int dp = bufferESP.indexOf(":", ipd);

      if (dp > 0 && bufferESP.length() > dp + 1) {
        for (int i = dp + 1; i < bufferESP.length(); i++) {
          char d = bufferESP[i];

          if (d == '.' || d == '-') {
            recebido += d;
            beep(60);
          }
        }
        bufferESP = "";
      }
    }

    if (bufferESP.length() > 100) bufferESP = "";
  }
}

void enviarMorse() {
  int estado = digitalRead(botao);

  if (estado == LOW) {
    if (!pressionado) {
      pressionado = true;
      tempoPressionado = millis();
      tone(buzzer, 1000);
    }
    digitalWrite(led, HIGH);
  }

  if (estado == HIGH && pressionado) {
    pressionado = false;

    digitalWrite(led, LOW);
    noTone(buzzer);

    char simbolo = (millis() - tempoPressionado < 300) ? '.' : '-';
    enviado += simbolo;

    esp.println("AT+CIPSEND=1");
    esperandoEnvio = false;

    unsigned long t = millis();
    while (!esperandoEnvio && millis() - t < 1000) {
      lerESP(); // 🔥 importante
    }

    if (esperandoEnvio) {
      esp.print(simbolo);
    }
  }
}

void beep(int tempo) {
  digitalWrite(led, HIGH);
  tone(buzzer, 1000);
  delay(tempo);
  noTone(buzzer);
  digitalWrite(led, LOW);
}

void atualizarDisplay() {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.print("SERVER");

  display.setCursor(0, 10);
  display.print("TX:");
  display.setCursor(0, 20);
  display.print(enviado);

  display.setCursor(0, 35);
  display.print("RX:");
  display.setCursor(0, 45);
  display.print(recebido);

  display.display();
}