/*
====================================================
IMPRESSORA 3D EDUCACIONAL - ESTÁVEL (SEM SOCO)
✔ pausas entre linhas
✔ velocidade reduzida
✔ subida Z suave
✔ encoder virtual mantido
====================================================
*/

// ===============================
const int PASSOS_POR_CM = 250;

// 🔥 mais lento (reduz vibração)
const int VEL_XY = 4;
const int VEL_Z = 10;

// Z mais suave
const int PASSOS_Z = 20;

// ===============================
long posX = 0;
long posY = 0;
long posZ = 0;

// ===============================
#define ENX 9
#define X1 2
#define X2 3

#define ENY 10
#define Y1 4
#define Y2 5

#define ENZ1 11
#define Z1A 6
#define Z1B 7

#define ENZ2 12
#define Z2A 8
#define Z2B 13

// ===============================
void setup() {

  pinMode(ENX, OUTPUT);
  pinMode(X1, OUTPUT);
  pinMode(X2, OUTPUT);

  pinMode(ENY, OUTPUT);
  pinMode(Y1, OUTPUT);
  pinMode(Y2, OUTPUT);

  pinMode(ENZ1, OUTPUT);
  pinMode(Z1A, OUTPUT);
  pinMode(Z1B, OUTPUT);

  pinMode(ENZ2, OUTPUT);
  pinMode(Z2A, OUTPUT);
  pinMode(Z2B, OUTPUT);

  pararTudo();
}

// ===============================
void loop() {

  for (int camada = 0; camada < 4; camada++) {

    desenharQuadrado();

    delay(1200);   // 🔥 pausa de estabilização (IMPORTANTE)

    subirCamada();
  }

  while (true);
}

// ===============================
// QUADRADO COM PAUSA ENTRE LADOS
// ===============================
void desenharQuadrado() {

  long dist = 2 * PASSOS_POR_CM;

  moverX(posX + dist);
  delay(400);   // 🔥 pausa entre linhas

  moverY(posY + dist);
  delay(400);

  moverX(posX - dist);
  delay(400);

  moverY(posY - dist);
  delay(600);
}

// ===============================
void moverX(long alvo) {

  int dir = (alvo > posX) ? 1 : -1;

  while (posX != alvo) {
    stepX(dir);
    posX += dir;
    delay(VEL_XY);
  }

  pararXY();
}

void moverY(long alvo) {

  int dir = (alvo > posY) ? 1 : -1;

  while (posY != alvo) {
    stepY(dir);
    posY += dir;
    delay(VEL_XY);
  }

  pararXY();
}

// ===============================
void stepX(int dir) {

  digitalWrite(X1, dir > 0 ? HIGH : LOW);
  digitalWrite(X2, dir > 0 ? LOW : HIGH);

  analogWrite(ENX, 130);  // 🔥 menos força
  delay(2);
  analogWrite(ENX, 0);
}

void stepY(int dir) {

  digitalWrite(Y1, dir > 0 ? HIGH : LOW);
  digitalWrite(Y2, dir > 0 ? LOW : HIGH);

  analogWrite(ENY, 130);
  delay(2);
  analogWrite(ENY, 0);
}

// ===============================
// 🔥 Z COM SUBIDA SUAVE (SEM SOCO)
// ===============================
void subirCamada() {

  long alvoZ = posZ + PASSOS_Z;

  digitalWrite(Z1A, HIGH);
  digitalWrite(Z1B, LOW);
  digitalWrite(Z2A, HIGH);
  digitalWrite(Z2B, LOW);

  int pwm = 120;  // começa fraco

  while (posZ < alvoZ) {

    stepZ(pwm);

    posZ++;

    // 🔥 rampa suave (evita tranco)
    pwm += 2;
    if (pwm > 180) pwm = 180;

    delay(VEL_Z);
  }

  manterZ();
}

// ===============================
void stepZ(int pwm) {

  analogWrite(ENZ1, pwm);
  analogWrite(ENZ2, pwm);

  delay(3);

  analogWrite(ENZ1, pwm - 40);
  analogWrite(ENZ2, pwm - 40);
}

// ===============================
void manterZ() {

  analogWrite(ENZ1, 90);
  analogWrite(ENZ2, 90);

  digitalWrite(Z1A, HIGH);
  digitalWrite(Z1B, LOW);

  digitalWrite(Z2A, HIGH);
  digitalWrite(Z2B, LOW);
}

// ===============================
void pararXY() {

  analogWrite(ENX, 0);
  analogWrite(ENY, 0);

  digitalWrite(X1, LOW);
  digitalWrite(X2, LOW);

  digitalWrite(Y1, LOW);
  digitalWrite(Y2, LOW);
}

// ===============================
void pararTudo() {
  pararXY();

  analogWrite(ENZ1, 0);
  analogWrite(ENZ2, 0);

  digitalWrite(Z1A, LOW);
  digitalWrite(Z1B, LOW);
  digitalWrite(Z2A, LOW);
  digitalWrite(Z2B, LOW);
}