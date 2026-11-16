/*
====================================================
SIMULAÇÃO IMPRESSÃO 3D
QUADRADO EM CAMADAS - LOOP INFINITO

✔ Movimento suave
✔ X invertido corrigido
✔ 20 camadas
✔ Z sobe por camadas
✔ Espera 5 segundos
✔ Z desce suavemente com freio
✔ Reinicia automaticamente
====================================================
*/


// ==================================================
// VELOCIDADES
// ==================================================

int VELOCIDADE_XY = 75;

int KICK_XY = 170;


// ==================================================
// CONTROLE INDIVIDUAL Z
// ==================================================

int VELOCIDADE_Z1 = 255;
int VELOCIDADE_Z2 = 245;


// ==================================================
// DESCIDA Z
// ==================================================

int VELOCIDADE_DESCIDA_Z = 110;


// ==================================================
// TAMANHO DO QUADRADO
// ==================================================

int LADO = 260;


// ==================================================
// CAMADAS
// ==================================================

// DOBRADO
int TOTAL_CAMADAS = 20;

int ALTURA_CAMADA = 55;


// ==================================================
// EIXO X
// ==================================================

int ENX = 9;

int X1 = 2;
int X2 = 3;


// ==================================================
// EIXO Y
// ==================================================

int ENY = 10;

int Y1 = 4;
int Y2 = 5;


// ==================================================
// Z MOTOR 1
// ==================================================

int ENZ1 = 11;

int Z1A = 6;
int Z1B = 7;


// ==================================================
// Z MOTOR 2
// ==================================================

int ENZ2 = 12;

int Z2A = 8;
int Z2B = 13;



// ==================================================
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



// ==================================================
void loop() {

  // =========================================
  // IMPRESSÃO DAS CAMADAS
  // =========================================

  for (int camada = 0; camada < TOTAL_CAMADAS; camada++) {

    desenharQuadradoLento();

    subirCamadaZ();

    delay(500);
  }


  // =========================================
  // PAUSA FINAL
  // =========================================

  delay(5000);


  // =========================================
  // DESCIDA CONTROLADA
  // =========================================

  descerZSuave();


  // =========================================
  // PEQUENA PAUSA
  // =========================================

  delay(2000);
}



// ==================================================
// DESENHA QUADRADO
// ==================================================
void desenharQuadradoLento() {

  moverX(true, LADO);

  moverY(true, LADO);

  moverX(false, LADO);

  moverY(false, LADO);
}



// ==================================================
// MOVIMENTO X (INVERTIDO)
// ==================================================
void moverX(bool sentido, int tempoTotal) {

  moverXY(ENX, X1, X2, !sentido, tempoTotal);
}



// ==================================================
// MOVIMENTO Y
// ==================================================
void moverY(bool sentido, int tempoTotal) {

  moverXY(ENY, Y1, Y2, sentido, tempoTotal);
}



// ==================================================
// MOVIMENTO XY
// ==================================================
void moverXY(int EN, int P1, int P2, bool sentido, int tempoTotal) {

  // DIREÇÃO
  if (sentido) {

    digitalWrite(P1, HIGH);
    digitalWrite(P2, LOW);

  } else {

    digitalWrite(P1, LOW);
    digitalWrite(P2, HIGH);
  }


  // PARTIDA
  analogWrite(EN, KICK_XY);

  delay(20);


  // TRANSIÇÃO
  for (int v = KICK_XY; v >= VELOCIDADE_XY; v -= 5) {

    analogWrite(EN, v);

    delay(2);
  }


  // MOVIMENTO
  analogWrite(EN, VELOCIDADE_XY);

  delay(tempoTotal);


  // FRENAGEM
  for (int v = VELOCIDADE_XY; v >= 0; v -= 5) {

    analogWrite(EN, v);

    delay(2);
  }


  // PARADA
  analogWrite(EN, 0);

  digitalWrite(P1, LOW);
  digitalWrite(P2, LOW);

  delay(20);
}



// ==================================================
// SOBE CAMADA Z
// ==================================================
void subirCamadaZ() {

  // DIREÇÃO
  digitalWrite(Z1A, HIGH);
  digitalWrite(Z1B, LOW);

  digitalWrite(Z2A, HIGH);
  digitalWrite(Z2B, LOW);


  // SUBIDA
  analogWrite(ENZ1, VELOCIDADE_Z1);
  analogWrite(ENZ2, VELOCIDADE_Z2);

  delay(ALTURA_CAMADA);


  // FREIO
  for (int v = 255; v >= 120; v -= 10) {

    int pwm1 = map(v, 120, 255, 120, VELOCIDADE_Z1);
    int pwm2 = map(v, 120, 255, 120, VELOCIDADE_Z2);

    analogWrite(ENZ1, pwm1);
    analogWrite(ENZ2, pwm2);

    delay(6);
  }


  // TRAVA
  analogWrite(ENZ1, 90);
  analogWrite(ENZ2, 90);

  digitalWrite(Z1A, HIGH);
  digitalWrite(Z1B, LOW);

  digitalWrite(Z2A, HIGH);
  digitalWrite(Z2B, LOW);
}



// ==================================================
// DESCIDA SUAVE COM FREIO
// ==================================================
void descerZSuave() {

  // DIREÇÃO DESCIDA
  digitalWrite(Z1A, LOW);
  digitalWrite(Z1B, HIGH);

  digitalWrite(Z2A, LOW);
  digitalWrite(Z2B, HIGH);


  // DESCE DEVAGAR
  analogWrite(ENZ1, VELOCIDADE_DESCIDA_Z);
  analogWrite(ENZ2, VELOCIDADE_DESCIDA_Z);

  delay(TOTAL_CAMADAS * ALTURA_CAMADA);


  // FREIA MILÍMETRO A MILÍMETRO
  for (int v = VELOCIDADE_DESCIDA_Z; v >= 0; v -= 2) {

    analogWrite(ENZ1, v);
    analogWrite(ENZ2, v);

    delay(25);
  }


  // DESLIGA
  analogWrite(ENZ1, 0);
  analogWrite(ENZ2, 0);

  digitalWrite(Z1A, LOW);
  digitalWrite(Z1B, LOW);

  digitalWrite(Z2A, LOW);
  digitalWrite(Z2B, LOW);
}



// ==================================================
void pararTudo() {

  analogWrite(ENX, 0);
  analogWrite(ENY, 0);

  analogWrite(ENZ1, 0);
  analogWrite(ENZ2, 0);

  digitalWrite(X1, LOW);
  digitalWrite(X2, LOW);

  digitalWrite(Y1, LOW);
  digitalWrite(Y2, LOW);

  digitalWrite(Z1A, LOW);
  digitalWrite(Z1B, LOW);

  digitalWrite(Z2A, LOW);
  digitalWrite(Z2B, LOW);
}