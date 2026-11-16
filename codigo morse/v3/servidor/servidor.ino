// =====================================================
// PROTOTIPO 1 = SERVIDOR CORRIGIDO
// UNO + ESP01 + OLED
// PINOS MANTIDOS
// Botao = 7
// LED = 6
// Buzzer = 5
// ESP TX -> Arduino 2
// ESP RX -> Arduino 3 (com divisor)
// =====================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
SoftwareSerial esp(2, 3);   // RX,TX

int botao = 7;
int led = 6;
int buzzer = 5;

String enviado = "";
String recebido = "";

bool pressionado = false;
unsigned long tempoInicio = 0;
unsigned long ultimoDisplay = 0;

int canal = 0;

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
  cmd("AT+CWMODE=2", 1000);
  cmd("AT+CIPMUX=1", 1000);
  cmd("AT+CWSAP=\"ESP_SERVER\",\"12345678\",1,3", 3000);
  cmd("AT+CIPSERVER=1,333", 3000);
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

    String s = "AT+CIPSEND=" + String(canal) + ",1";
    esp.println(s);
    delay(250);
    esp.print(simb);
  }
}

// -----------------------------------------------------
void lerESP() {

  while (esp.available()) {

    char c = esp.read();

    Serial.write(c);

    if (c == ',' || c == ':') continue;

    if (c >= '0' && c <= '4') {
      canal = c - '0';
    }

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
  display.print("SERVIDOR");

  display.setCursor(0,18);
  display.print("TX:");
  display.print(enviado);

  display.setCursor(0,42);
  display.print("RX:");
  display.print(recebido);

  display.display();
}