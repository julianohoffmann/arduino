#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- Controles ----------
#define JOY_X A0
#define JOY_Y A1
#define JOY_BTN 8
#define BTN_UP 2
#define BTN_DOWN 3
#define BUZZER_PIN 11

// ---------- Flappy Bird ----------
float birdYf;
int birdX = 30;
float velocity = 0;
float gravity = 0.4;             // gravidade normal
float gravityAfterJump = 0.2;    // queda mais lenta logo após pulo
int framesAfterJump = 0;
int framesToNormalGravity = 5;

float jumpStrength = -2.0;       // pulo mais leve
bool birdVisible = true;

struct Pipe {
  float x;
  int gapY;
  int gapHeight;
};
Pipe pipes[3];
float pipeSpeed = 1.0;           // horizontal levemente mais lento
int score = 0;
bool gameOver = false;
bool paused = false;
int loopDelay = 40;               // suavidade

// ---------- Funções ----------
void gerarPipes() {
  for (int i = 0; i < 3; i++) {
    pipes[i].x = SCREEN_WIDTH + i * 50;
    pipes[i].gapHeight = 20 + random(10);
    pipes[i].gapY = random(5, SCREEN_HEIGHT - pipes[i].gapHeight - 5);
  }
}

void resetGame() {
  birdYf = SCREEN_HEIGHT / 2;
  velocity = 0;
  score = 0;
  gameOver = false;
  paused = false;
  framesAfterJump = 0;
  gerarPipes();
}

void desenharTela() {
  display.clearDisplay();

  // Pássaro piscando
  if (birdVisible) display.fillRect(birdX, (int)birdYf, 4, 4, SSD1306_WHITE);

  // Pipes
  for (int i = 0; i < 3; i++) {
    display.fillRect((int)pipes[i].x, 0, 6, pipes[i].gapY, SSD1306_WHITE);
    display.fillRect((int)pipes[i].x, pipes[i].gapY + pipes[i].gapHeight, 6, SCREEN_HEIGHT - (pipes[i].gapY + pipes[i].gapHeight), SSD1306_WHITE);
  }

  // Pontuação
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Score: ");
  display.print(score);

  if (paused) {
    display.setCursor(40, SCREEN_HEIGHT/2 - 4);
    display.println("PAUSED");
  }

  display.display();
}

void moverPassaro() {
  // Gravidade dinâmica
  if (framesAfterJump > 0) {
    velocity += gravityAfterJump;
    framesAfterJump--;
  } else {
    velocity += gravity;
  }

  if (velocity > 3.5) velocity = 3.5; // limite da queda
  birdYf += velocity;

  if (birdYf < 0) birdYf = 0;
  if (birdYf > SCREEN_HEIGHT - 4) birdYf = SCREEN_HEIGHT - 4;
}

void moverPipes() {
  for (int i = 0; i < 3; i++) {
    pipes[i].x -= pipeSpeed;
    if (pipes[i].x < -6) {
      pipes[i].x = SCREEN_WIDTH;
      pipes[i].gapHeight = 20 + random(10);
      pipes[i].gapY = random(5, SCREEN_HEIGHT - pipes[i].gapHeight - 5);
      score++;
      tone(BUZZER_PIN, 1500, 80);
      delay(80);
      noTone(BUZZER_PIN);
    }
  }
}

bool colidiu() {
  if (birdYf <= 0 || birdYf >= SCREEN_HEIGHT - 4) return true;
  for (int i = 0; i < 3; i++) {
    if (birdX + 4 > pipes[i].x && birdX < pipes[i].x + 6) {
      if (birdYf < pipes[i].gapY || birdYf + 4 > pipes[i].gapY + pipes[i].gapHeight) return true;
    }
  }
  return false;
}

void tocarGameOver() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1000 - i*200, 150);
    delay(150);
  }
  noTone(BUZZER_PIN);
}

void verificarControles() {
  int xValue = analogRead(JOY_X);
  int yValue = analogRead(JOY_Y);
  yValue = 1023 - yValue;

  // Pausa com joystick
  if (digitalRead(JOY_BTN) == LOW) {
    paused = !paused;
    delay(300);
  }

  // Controle do pássaro
  if (!paused && !gameOver) {
    bool jump = false;

    if (digitalRead(BTN_UP) == LOW) jump = true;
    if (digitalRead(BTN_DOWN) == LOW) { velocity = 2; jump = false; } // descer rápido
    if (xValue < 300) jump = true;
    if (yValue > 700) jump = true;

    if (jump) {
      velocity = jumpStrength;
      framesAfterJump = framesToNormalGravity; // queda mais lenta logo após pulo
      tone(BUZZER_PIN, 2000, 50);
      delay(50);
      noTone(BUZZER_PIN);
    }
  }

  // Retoma de pausa
  if (paused) {
    if (digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW ||
        xValue < 300 || xValue > 700 || yValue < 300 || yValue > 700 ||
        digitalRead(JOY_BTN) == LOW) {
      paused = false;
      delay(200);
    }
  }
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  pinMode(JOY_BTN, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  randomSeed(analogRead(A3));
  resetGame();
}

void loop() {
  if (!gameOver) {
    verificarControles();

    if (!paused) {
      moverPassaro();
      moverPipes();
      birdVisible = !birdVisible; // piscando
      desenharTela();

      if (colidiu()) {
        gameOver = true;
        tocarGameOver();
        delay(1000);
      }
    } else {
      desenharTela(); // PAUSED
      delay(loopDelay/2);
    }

    delay(loopDelay);
  } else {
    // Game Over
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, SCREEN_HEIGHT / 2 - 8);
    display.println("Game Over!");
    display.setCursor(20, SCREEN_HEIGHT / 2 + 8);
    display.print("Score: ");
    display.println(score);
    display.display();
    delay(2000);
    resetGame();
  }
}

void jogoFlappy() {
  setup();
  while (true) {
    loop();
  }
}
void startFlappy() {
  setup();
  while (true) loop();
}
