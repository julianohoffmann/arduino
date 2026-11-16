#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- PARTE 1: OLHOS ----------
int ref_eye_height = 40;
int ref_eye_width = 40;
int ref_space_between_eye = 10;
int ref_corner_radius = 10;

int left_eye_height = ref_eye_height;
int left_eye_width = ref_eye_width;
int left_eye_x = 32;
int left_eye_y = 32;
int right_eye_x = 32 + ref_eye_width + ref_space_between_eye;
int right_eye_y = 32;
int right_eye_height = ref_eye_height;
int right_eye_width = ref_eye_width;

void draw_eyes(bool update = true) {
  display.clearDisplay();
  int x = int(left_eye_x - left_eye_width / 2);
  int y = int(left_eye_y - left_eye_height / 2);
  display.fillRoundRect(x, y, left_eye_width, left_eye_height, ref_corner_radius, SSD1306_WHITE);
  x = int(right_eye_x - right_eye_width / 2);
  y = int(right_eye_y - right_eye_height / 2);
  display.fillRoundRect(x, y, right_eye_width, right_eye_height, ref_corner_radius, SSD1306_WHITE);
  if (update) display.display();
}

void center_eyes(bool update = true) {
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;
  left_eye_x = SCREEN_WIDTH / 2 - ref_eye_width / 2 - ref_space_between_eye / 2;
  left_eye_y = SCREEN_HEIGHT / 2;
  right_eye_x = SCREEN_WIDTH / 2 + ref_eye_width / 2 + ref_space_between_eye / 2;
  right_eye_y = SCREEN_HEIGHT / 2;
  draw_eyes(update);
}

void blink(int speed = 12) {
  draw_eyes();
  for (int i = 0; i < 3; i++) {
    left_eye_height -= speed;
    right_eye_height -= speed;
    draw_eyes();
    delay(1);
  }
  for (int i = 0; i < 3; i++) {
    left_eye_height += speed;
    right_eye_height += speed;
    draw_eyes();
    delay(1);
  }
}

void sleepEyes() {
  left_eye_height = 2;
  right_eye_height = 2;
  draw_eyes(true);
}

void wakeup() {
  sleepEyes();
  for (int h = 0; h <= ref_eye_height; h += 2) {
    left_eye_height = h;
    right_eye_height = h;
    draw_eyes(true);
  }
}

void happy_eye() {
  center_eyes(false);
  int offset = ref_eye_height / 2;
  for (int i = 0; i < 10; i++) {
    display.fillTriangle(left_eye_x - left_eye_width / 2 - 1, left_eye_y + offset,
                         left_eye_x + left_eye_width / 2 + 1, left_eye_y + 5 + offset,
                         left_eye_x - left_eye_width / 2 - 1, left_eye_y + left_eye_height + offset, SSD1306_BLACK);
    display.fillTriangle(right_eye_x + right_eye_width / 2 + 1, right_eye_y + offset,
                         right_eye_x - left_eye_width / 2 - 1, right_eye_y + 5 + offset,
                         right_eye_x + right_eye_width / 2 + 1, right_eye_y + right_eye_height + offset, SSD1306_BLACK);
    offset -= 2;
    display.display();
    delay(1);
  }
  delay(1000);
}

void intro_eyes() {
  wakeup();
  delay(400);
  center_eyes(true);
  delay(400);
  blink(10);
  delay(400);
  happy_eye();
  delay(400);
  sleepEyes();
  delay(400);
}

// ---------- PARTE 2: SNAKE ----------
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define JOY_X A0
#define JOY_Y A1
#define JOY_BTN 8

#define BTN_UP     2
#define BTN_RIGHT  3
#define BTN_DOWN   4
#define BTN_LEFT   5

#define BUZZER_PIN 11

int snakeX[100], snakeY[100];
int snakeLength = 5;
int dir = RIGHT;
int dirNext = RIGHT;
int foodX, foodY;
bool jogoPausado = false;

// ---------- Funções Snake ----------
void gerarComida() {
  foodX = (random(0, SCREEN_WIDTH / 4)) * 4;
  foodY = (random(0, SCREEN_HEIGHT / 4)) * 4;
}

void desenharSnake() {
  display.clearDisplay();
  display.fillRect(foodX, foodY, 4, 4, SSD1306_WHITE);
  for (int i = 0; i < snakeLength; i++) {
    display.fillRect(snakeX[i], snakeY[i], 4, 4, SSD1306_WHITE);
  }
  display.display();
}

void moverSnake() {
  dir = dirNext;
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  if (dir == UP) snakeY[0] -= 4;
  else if (dir == DOWN) snakeY[0] += 4;
  else if (dir == LEFT) snakeX[0] -= 4;
  else if (dir == RIGHT) snakeX[0] += 4;
}

bool colidiu() {
  if (snakeX[0] < 0 || snakeX[0] >= SCREEN_WIDTH || snakeY[0] < 0 || snakeY[0] >= SCREEN_HEIGHT) return true;
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) return true;
  }
  return false;
}

// ---------- Ler joystick + botões ----------
void lerJoystick() {
  int xValue = analogRead(JOY_X);
  int yValue = analogRead(JOY_Y);

  // Corrige eixo Y invertido
  yValue = 1023 - yValue;

  if (!jogoPausado) {
    if (xValue < 300 && dir != RIGHT) dirNext = LEFT;
    else if (xValue > 700 && dir != LEFT) dirNext = RIGHT;

    if (yValue < 300 && dir != DOWN) dirNext = UP;
    else if (yValue > 700 && dir != UP) dirNext = DOWN;

    if (digitalRead(BTN_UP) == LOW && dir != DOWN) dirNext = UP;
    if (digitalRead(BTN_DOWN) == LOW && dir != UP) dirNext = DOWN;
    if (digitalRead(BTN_LEFT) == LOW && dir != RIGHT) dirNext = LEFT;
    if (digitalRead(BTN_RIGHT) == LOW && dir != LEFT) dirNext = RIGHT;

    if (digitalRead(JOY_BTN) == LOW) {
      jogoPausado = !jogoPausado;
      delay(200);
    }
  } else {
    // Qualquer input despausa
    if (xValue < 300 || xValue > 700 || yValue < 300 || yValue > 700 ||
        digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW ||
        digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW ||
        digitalRead(JOY_BTN) == LOW) {
      jogoPausado = false;
      delay(200);
    }
  }
}

// ---------- Som Game Over ----------
void tocarGameOver() {
  tone(BUZZER_PIN, 1000, 150);
  delay(200);
  tone(BUZZER_PIN, 800, 150);
  delay(200);
  tone(BUZZER_PIN, 600, 200);
  delay(200);
  noTone(BUZZER_PIN);
}

// ---------- Iniciar Snake ----------
void iniciarSnake() {
  snakeLength = 5;
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = 40 - i * 4;
    snakeY[i] = 20;
  }
  dir = RIGHT;
  dirNext = RIGHT;
  gerarComida();

  while (true) {
    lerJoystick();

    if (jogoPausado) {
      display.clearDisplay();
      display.setTextSize(1);
      const char* msg = "PAUSADO";
      int16_t x1, y1;
      uint16_t w1, h1;
      display.getTextBounds(msg, 0, 0, &x1, &y1, &w1, &h1);
      display.setCursor((SCREEN_WIDTH - w1)/2, (SCREEN_HEIGHT - h1)/2);
      display.println(msg);
      display.display();
      delay(200);
      continue;
    }

    moverSnake();

    if (snakeX[0] == foodX && snakeY[0] == foodY) {
      snakeLength++;
      gerarComida();

      // ---------- SOM AO COMER ----------
      tone(BUZZER_PIN, 1500, 100); // som de 1500 Hz por 100ms
      delay(100);
      noTone(BUZZER_PIN);
    }

    if (colidiu()) {
      display.clearDisplay();
      display.setTextSize(1);
      const char* msg = "Game Over!";
      int16_t x1, y1;
      uint16_t w1, h1;
      display.getTextBounds(msg, 0, 0, &x1, &y1, &w1, &h1);
      display.setCursor((SCREEN_WIDTH - w1)/2, (SCREEN_HEIGHT - h1)/2);
      display.println(msg);
      display.display();

      tocarGameOver();
      delay(1200);
      return;
    }

    desenharSnake();
    delay(120);
  }
}

// ---------- SETUP E LOOP ----------
bool introMostrada = false;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

  pinMode(JOY_BTN, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  randomSeed(analogRead(A3));

  // ---------- INTRODUÇÃO CENTRALIZADA ----------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  const char* linha1 = "juliano";
  const char* linha2 = "hoffmann";
  const char* linha3 = "2025";

  int16_t x1, y1;
  uint16_t w1, h1;

  // Linha 1
  display.getTextBounds(linha1, 0, 0, &x1, &y1, &w1, &h1);
  int cursorX1 = (SCREEN_WIDTH - w1)/2;
  int cursorY1 = 15;

  // Linha 2
  display.getTextBounds(linha2, 0, 0, &x1, &y1, &w1, &h1);
  int cursorX2 = (SCREEN_WIDTH - w1)/2;
  int cursorY2 = cursorY1 + h1 + 2;

  // Linha 3
  display.getTextBounds(linha3, 0, 0, &x1, &y1, &w1, &h1);
  int cursorX3 = (SCREEN_WIDTH - w1)/2;
  int cursorY3 = cursorY2 + h1 + 2;

  display.setCursor(cursorX1, cursorY1);
  display.println(linha1);
  display.setCursor(cursorX2, cursorY2);
  display.println(linha2);
  display.setCursor(cursorX3, cursorY3);
  display.println(linha3);

  display.display();
  delay(1500);
}

void loop() {
  if (!introMostrada) {
    intro_eyes();
    introMostrada = true;
  }
  iniciarSnake(); // reinicia automaticamente após Game Over
}
