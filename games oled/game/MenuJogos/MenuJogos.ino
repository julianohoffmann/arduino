#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define JOY_X A0
#define JOY_Y A1
#define BTN_UP 2
#define BTN_DOWN 3
#define BTN_LEFT 4
#define BTN_RIGHT 5
#define JOY_BTN 8
#define BUZZER 11

extern void startLabirinto();
extern void startFlappy();
extern void startReflexo();
extern void startSnake();

const char* nomesJogos[] = {
  "1. Labirinto",
  "2. Flappy Bird",
  "3. Reflexo",
  "4. Snake"
};

int opcao = 0;

void setup() {
  pinMode(JOY_BTN, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 25);
  display.println("Menu de Jogos");
  display.display();
  delay(1500);
}

void loop() {
  mostrarMenu();
  lerJoystick();
  delay(150);
}

void mostrarMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 5);
  display.println("Selecione o jogo:");
  for (int i = 0; i < 4; i++) {
    display.setCursor(15, 20 + i * 10);
    if (i == opcao) display.print("> ");
    else display.print("  ");
    display.println(nomesJogos[i]);
  }
  display.display();
}

void lerJoystick() {
  int yVal = analogRead(JOY_Y);
  if (yVal > 700 || digitalRead(BTN_UP) == LOW) {
    opcao = (opcao - 1 + 4) % 4;
    tone(BUZZER, 1200, 80);
    delay(250);
  }

  if (yVal < 300 || digitalRead(BTN_DOWN) == LOW) {
    opcao = (opcao + 1) % 4;
    tone(BUZZER, 1000, 80);
    delay(250);
  }

  if (digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW) {
    tone(BUZZER, 1500, 100);
    delay(300);
    iniciarJogo(opcao);
  }
}

void iniciarJogo(int i) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(25, 25);
  display.println("Iniciando...");
  display.display();
  delay(800);

  switch (i) {
    case 0: startLabirinto(); break;
    case 1: startFlappy();    break;
    case 2: startReflexo();   break;
    case 3: startSnake();     break;
  }
}
