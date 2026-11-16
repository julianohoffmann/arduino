/*
==================================================
EXTRUSOR PARA IMPRESSORA 3D
Arduino Uno dedicado ao motor do extrusor
Controle por L298N
==================================================

Ligação:

Arduino        L298N

D5 ---------- ENA (PWM)
D6 ---------- IN1
D7 ---------- IN2

Motor DC 12V -> OUT1 e OUT2
*/

// -----------------------------
#define ENA 5
#define IN1 6
#define IN2 7

// -----------------------------
// Ajustes

const int PWM_CONTINUO = 60;     // velocidade normal

const int PWM_PULSO = 120;       // força extra

const unsigned long TEMPO_PULSO = 120;      // duração do pulso

const unsigned long INTERVALO = 800;        // intervalo entre pulsos

// -----------------------------

unsigned long ultimoPulso = 0;

void setup() {

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pararMotor();

  // Aguarda 5 segundos
  delay(5000);

  iniciarExtrusao();
}

void loop() {

  // Mantém girando lentamente
  analogWrite(ENA, PWM_CONTINUO);

  unsigned long agora = millis();

  if (agora - ultimoPulso >= INTERVALO) {

    analogWrite(ENA, PWM_PULSO);

    delay(TEMPO_PULSO);

    analogWrite(ENA, PWM_CONTINUO);

    ultimoPulso = millis();
  }

}

// -----------------------------
void iniciarExtrusao() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  analogWrite(ENA, PWM_CONTINUO);
}

// -----------------------------
void pararMotor() {

  analogWrite(ENA, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}