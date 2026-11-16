#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick (analógico)
#define JOY_X A0
#define JOY_Y A1
#define JOY_BUTTON 9  // botão do joystick (no Funduino original)

// Botões Funduino (A,B,C,D)
#define BTN_UP 2     // A
#define BTN_RIGHT 3  // B
#define BTN_DOWN 4   // C
#define BTN_LEFT 5   // D

// Buzzer
#define BUZZER 11

// Labirinto
#define CELL_SIZE 8
#define ROWS 8
#define COLS 16

byte maze[ROWS][COLS] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1},
  {1,0,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
  {1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1},
  {1,0,1,1,1,1,0,1,1,1,1,0,1,0,1,1},
  {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1},
  {1,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Posição do jogador
int playerRow = 1;
int playerCol = 1;

// Objetivo
int goalRow = 6;
int goalCol = 14;

// Controle de tempo
unsigned long lastMove = 0;
const int moveDelay = 180;  // tempo entre movimentos (ms)

void setup() {
  pinMode(JOY_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Tela inicial centralizada
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds("Labirinto", 0, 0, &x1, &y1, &w, &h);
  int16_t xInit = (SCREEN_WIDTH - w) / 2;
  int16_t yInit = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(xInit, yInit);
  display.println("Labirinto");
  display.display();
  delay(2000);
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastMove >= moveDelay) {
    readControls();  // lê joystick e botões
    lastMove = currentTime;
  }

  drawMaze();

  if (playerRow == goalRow && playerCol == goalCol) {
    displayVictory();
    resetGame();
  }
}

// Ler joystick e botões (A,B,C,D)
void readControls() {
  int newRow = playerRow;
  int newCol = playerCol;

  // --- Joystick ---
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  if (xVal < 400) newCol--;   // esquerda
  if (xVal > 600) newCol++;   // direita
  if (yVal < 400) newRow++;   // desce
  if (yVal > 600) newRow--;   // sobe

  // --- Botões Funduino (LOW = pressionado) ---
  if (digitalRead(BTN_UP) == LOW) newRow--;     // botão A → sobe
  if (digitalRead(BTN_RIGHT) == LOW) newCol++;  // botão B → direita
  if (digitalRead(BTN_DOWN) == LOW) newRow++;   // botão C → desce
  if (digitalRead(BTN_LEFT) == LOW) newCol--;   // botão D → esquerda

  // --- Colisão com paredes ---
  if (maze[newRow][newCol] == 0) {
    playerRow = newRow;
    playerCol = newCol;
  } else {
    tone(BUZZER, 1000, 50);
  }
}

// Desenhar labirinto e jogador
void drawMaze() {
  display.clearDisplay();
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      int x = c * CELL_SIZE;
      int y = r * CELL_SIZE;
      if (maze[r][c] == 1) {
        display.fillRect(x, y, CELL_SIZE, CELL_SIZE, SSD1306_WHITE);
      }
    }
  }

  // Objetivo
  display.fillRect(goalCol * CELL_SIZE + 2, goalRow * CELL_SIZE + 2, CELL_SIZE - 4, CELL_SIZE - 4, SSD1306_WHITE);
  // Jogador
  display.fillCircle(playerCol * CELL_SIZE + CELL_SIZE / 2, playerRow * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 2 - 1, SSD1306_WHITE);
  display.display();
}

// Tela de vitória
void displayVictory() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  String text = "VOCE VENCEU!";
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text.c_str(), 0, 0, &x1, &y1, &w, &h);
  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(x, y);
  display.println(text);
  display.display();
  tone(BUZZER, 1500, 500);
  delay(3000);
}

// Reiniciar jogo
void resetGame() {
  playerRow = 1;
  playerCol = 1;
}
