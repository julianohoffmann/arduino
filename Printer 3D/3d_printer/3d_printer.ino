// ===== PINOS =====

// Eixo Y
int y1 = 2;
int y2 = 3;
int y3 = 4;
int y4 = 5;

// Eixo X
int x1 = 6;
int x2 = 7;
int x3 = 8;
int x4 = 9;

// Eixo Z (A4988)
int stepPin = 10;
int dirPin = 11;

// Cola quente
int cola = 12;

// ===== SETUP =====
void setup() {
  pinMode(y1, OUTPUT);
  pinMode(y2, OUTPUT);
  pinMode(y3, OUTPUT);
  pinMode(y4, OUTPUT);

  pinMode(x1, OUTPUT);
  pinMode(x2, OUTPUT);
  pinMode(x3, OUTPUT);
  pinMode(x4, OUTPUT);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(cola, OUTPUT);
}

// ===== MOVIMENTOS =====

void parar() {
  digitalWrite(x1, LOW); digitalWrite(x2, LOW);
  digitalWrite(x3, LOW); digitalWrite(x4, LOW);
  digitalWrite(y1, LOW); digitalWrite(y2, LOW);
  digitalWrite(y3, LOW); digitalWrite(y4, LOW);
}

void frente(int t) {
  digitalWrite(y1, HIGH); digitalWrite(y2, LOW);
  digitalWrite(y3, HIGH); digitalWrite(y4, LOW);
  delay(t);
  parar();
}

void tras(int t) {
  digitalWrite(y1, LOW); digitalWrite(y2, HIGH);
  digitalWrite(y3, LOW); digitalWrite(y4, HIGH);
  delay(t);
  parar();
}

void direita(int t) {
  digitalWrite(x1, HIGH); digitalWrite(x2, LOW);
  digitalWrite(x3, HIGH); digitalWrite(x4, LOW);
  delay(t);
  parar();
}

void esquerda(int t) {
  digitalWrite(x1, LOW); digitalWrite(x2, HIGH);
  digitalWrite(x3, LOW); digitalWrite(x4, HIGH);
  delay(t);
  parar();
}

// ===== Z =====

void subirZ() {
  digitalWrite(dirPin, HIGH);
  for(int i=0;i<100;i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }
}

// ===== COLA =====

void ligaCola() {
  digitalWrite(cola, HIGH);
}

void desligaCola() {
  digitalWrite(cola, LOW);
}

// ===== DESENHO =====

void desenharQuadrado() {
  frente(2000);
  direita(2000);
  tras(2000);
  esquerda(2000);
}

// ===== LOOP =====

void loop() {

  for(int camada = 0; camada < 3; camada++) {

    ligaCola();

    desenharQuadrado();

    desligaCola();

    subirZ();

    delay(2000);
  }

  while(true); // para execução
}