#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick
#define JOY_X A0
#define JOY_Y A1
#define JOY_BUTTON 2

// Buzzer
#define BUZZER 11

// Jogo
int playerX = SCREEN_WIDTH / 2;
int playerY = SCREEN_HEIGHT / 2;
int targetX, targetY;
int score = 0;

unsigned long lastTargetTime = 0;
unsigned long reactionTime = 10000; // tempo para acertar o alvo (ms)

void setup() {
  pinMode(JOY_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Tela inicial centralizada
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  int16_t xInit = (SCREEN_WIDTH - 6*16) / 2; // "Jogo de Reflexos" tem 16 caracteres
  display.setCursor(xInit, 25);
  display.println("Jogo de Reflexos");
  display.display();
  delay(2000);

  spawnTarget();
}

// Gera novo alvo
void spawnTarget() {
  targetX = random(0, SCREEN_WIDTH - 5);
  targetY = random(0, SCREEN_HEIGHT - 5);
  lastTargetTime = millis();
}

// Loop principal
void loop() {
  readJoystick();

  display.clearDisplay();

  // Desenhar alvo
  display.fillRect(targetX, targetY, 5, 5, SSD1306_WHITE);

  // Desenhar jogador
  display.fillCircle(playerX, playerY, 3, SSD1306_WHITE);

  // Mostrar pontuação (canto superior esquerdo)
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Score: ");
  display.print(score);

  display.display();

  // Verifica colisão
  if (abs(playerX - targetX) <= 3 && abs(playerY - targetY) <= 3) {
    score++;
    spawnTarget();
    tone(BUZZER, 1000, 150); // som ao pegar o alvo
  }

  // Verifica tempo limite
  if (millis() - lastTargetTime > reactionTime) {
    gameOver();
  }

  delay(50);
}

// Ler joystick com eixo Y invertido
void readJoystick() {
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  // Horizontal normal
  if (xVal < 400) playerX -= 2;
  if (xVal > 600) playerX += 2;

  // Vertical invertido para a tela
  if (yVal < 400) playerY += 2; // empurrar para cima → Y aumenta
  if (yVal > 600) playerY -= 2; // empurrar para baixo → Y diminui

  // Limites da tela
  if (playerX < 0) playerX = 0;
  if (playerX > SCREEN_WIDTH) playerX = SCREEN_WIDTH;
  if (playerY < 0) playerY = 0;
  if (playerY > SCREEN_HEIGHT) playerY = SCREEN_HEIGHT;
}

// Tela de Game Over centralizada
void gameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // "GAME OVER" centralizado
  int16_t xGameOver = (SCREEN_WIDTH - 6*9) / 2; // 9 caracteres
  display.setCursor(xGameOver, 25);
  display.println("GAME OVER");

  // Pontuação centralizada
  String scoreText = "Score: " + String(score);
  int16_t xScore = (SCREEN_WIDTH - 6*scoreText.length()) / 2;
  display.setCursor(xScore, 40);
  display.println(scoreText);

  display.display();
  tone(BUZZER, 500, 500); // som de game over
  delay(3000);

  // Reiniciar jogo
  score = 0;
  playerX = SCREEN_WIDTH / 2;
  playerY = SCREEN_HEIGHT / 2;
  spawnTarget();
}
