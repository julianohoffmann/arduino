// =====================================================
// PROTOTIPO 2 = CLIENTE CORRIGIDO
// UNO + ESP01 + OLED
// PINOS MANTIDOS
// =====================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
SoftwareSerial esp(2, 3);

int botao = 7;
int led = 6;
int buzzer = 5;

String enviado = "";
String recebido = "";

bool pressionado = false;
unsigned long tempoInicio = 0;
unsigned long ultimoDisplay = 0;

// -----------------------------------------------------
void setup() {

  pinMode(botao, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(led, LOW);

  Serial.begin(9600);
  esp.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  iniciarESP();
}

// -----------------------------------------------------
void loop() {

  lerESP();
  enviarBotao();

  if (millis() - ultimoDisplay > 200) {
    tela();
    ultimoDisplay = millis();
  }
}

// -----------------------------------------------------
void iniciarESP() {

  delay(4000);

  cmd("AT+RST", 4000);
  cmd("AT", 1000);
  cmd("ATE0", 1000);
  cmd("AT+CWMODE=1", 1000);
  cmd("AT+CWJAP=\"ESP_SERVER\",\"12345678\"", 12000);
  cmd("AT+CIPSTART=\"TCP\",\"192.168.4.1\",333", 5000);
}

// -----------------------------------------------------
void cmd(String c, int t) {
  esp.println(c);
  delay(t);
}

// -----------------------------------------------------
void enviarBotao() {

  int estado = digitalRead(botao);

  if (estado == LOW && !pressionado) {

    pressionado = true;
    tempoInicio = millis();

    digitalWrite(led, HIGH);
    tone(buzzer, 1000);
  }

  if (estado == HIGH && pressionado) {

    pressionado = false;

    digitalWrite(led, LOW);
    noTone(buzzer);

    unsigned long duracao = millis() - tempoInicio;

    char simb;

    if (duracao < 300) {
      simb = '.';
      enviado += ".";
    } else {
      simb = '-';
      enviado += "-";
    }

    esp.println("AT+CIPSEND=1");
    delay(250);
    esp.print(simb);
  }
}

// -----------------------------------------------------
void lerESP() {

  while (esp.available()) {

    char c = esp.read();

    Serial.write(c);

    if (c == '.' || c == '-') {
      recebido += c;
      beep();
    }
  }
}

// -----------------------------------------------------
void beep() {
  digitalWrite(led, HIGH);
  tone(buzzer, 1200);
  delay(120);
  noTone(buzzer);
  digitalWrite(led, LOW);
}

// -----------------------------------------------------
void tela() {

  if (enviado.length() > 20) enviado = "";
  if (recebido.length() > 20) recebido = "";

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);
  display.print("CLIENTE");

  display.setCursor(0,18);
  display.print("TX:");
  display.print(enviado);

  display.setCursor(0,42);
  display.print("RX:");
  display.print(recebido);

  display.display();
}