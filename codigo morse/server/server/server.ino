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

unsigned long tempoPressionado;
bool pressionado = false;

unsigned long ultimoUpdate = 0;

void setup() {
  pinMode(botao, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);
  esp.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(true);
  }

  display.clearDisplay();
  display.display();
  delay(500);

  configurarESP();
}

void loop() {
  enviarMorse();
  receberMorse();

  if (enviado.length() > 20) enviado = "";
  if (recebido.length() > 20) recebido = "";

  if (millis() - ultimoUpdate > 200) {
    atualizarDisplay();
    ultimoUpdate = millis();
  }
}

void configurarESP() {
  delay(2000);
  esp.println("AT");
  delay(1000);

  esp.println("AT+CWMODE=2");
  delay(1000);

  esp.println("AT+CWSAP=\"ESP_SERVER\",\"12345678\",5,3");
  delay(1000);

  esp.println("AT+CIPMUX=1");
  delay(1000);

  esp.println("AT+CIPSERVER=1,80");
  delay(1000);
}

void enviarMorse() {
  int estado = digitalRead(botao);

  if (estado == LOW) {
    if (!pressionado) {
      pressionado = true;
      tempoPressionado = millis();
      tone(buzzer, 1000); // começa som
    }

    digitalWrite(led, HIGH);
  }

  if (estado == HIGH && pressionado) {
    pressionado = false;

    digitalWrite(led, LOW);
    noTone(buzzer); // para som

    unsigned long duracao = millis() - tempoPressionado;

    char simbolo;

    if (duracao < 300) {
      simbolo = '.';
      enviado += ".";
    } else {
      simbolo = '-';
      enviado += "-";
    }

    esp.print("AT+CIPSEND=0,1\r\n");
    delay(100);
    esp.print(simbolo);
  }
}

void receberMorse() {
  while (esp.available()) {
    char c = esp.read();

    if (c == '.' || c == '-') {
      recebido += c;
      beep(200);
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

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("TX:");
  display.setCursor(0, 10);
  display.print(enviado);

  display.setCursor(0, 35);
  display.print("RX:");
  display.setCursor(0, 45);
  display.print(recebido);

  display.display();
}