/*
LCD 16x2 (com I2C)

VCC → 5V
GND → GND
SDA → A4
SCL → A5
*/


/*

Segmento	Arduino
a	8
b	9
c	3
d	4
e	5
f	7
g	6
dp	2 (não vamos usar, pois está no sensor)

*/

/*

CÓDIGO COMPLETO (sensor IR + interrupção + LCD I2C + Serial)

*/

// ========================
// CONFIGURAÇÃO DO DISPLAY
// ========================
int a = 8;
int b = 9;
int c = 3;
int d = 4;
int e = 5;
int f = 7;
int g = 6;

const int pinoSensor = 2;  // interrupção INT0

volatile unsigned int contador = 0;
volatile bool bloqueado = false;

// Tabela de segmentos (comum catodo)
int numeros[10][7] = {
  {1,1,1,1,1,1,0},  // 0
  {0,1,1,0,0,0,0},  // 1
  {1,1,0,1,1,0,1},  // 2
  {1,1,1,1,0,0,1},  // 3
  {0,1,1,0,0,1,1},  // 4
  {1,0,1,1,0,1,1},  // 5
  {1,0,1,1,1,1,1},  // 6
  {1,1,1,0,0,0,0},  // 7
  {1,1,1,1,1,1,1},  // 8
  {1,1,1,1,0,1,1}   // 9
};

void mostrarNumero(int n) {
  digitalWrite(a, numeros[n][0]);
  digitalWrite(b, numeros[n][1]);
  digitalWrite(c, numeros[n][2]);
  digitalWrite(d, numeros[n][3]);
  digitalWrite(e, numeros[n][4]);
  digitalWrite(f, numeros[n][5]);
  digitalWrite(g, numeros[n][6]);
}

void detectar() {
  int estado = digitalRead(pinoSensor);

  if (estado == LOW && !bloqueado) {
    contador++;
    if (contador > 9) contador = 0;  
    bloqueado = true;
  }

  if (estado == HIGH) {
    bloqueado = false;
  }
}

void setup() {
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);

  pinMode(pinoSensor, INPUT);

  attachInterrupt(digitalPinToInterrupt(pinoSensor), detectar, CHANGE);

  Serial.begin(9600);
}

void loop() {
  mostrarNumero(contador);
  Serial.println(contador);
  delay(50);
}
