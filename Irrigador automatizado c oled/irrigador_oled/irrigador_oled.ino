
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ==========================================
// CONFIGURACAO OLED
// ==========================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ==========================================
// SENSOR DHT11
// ==========================================

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// ==========================================
// SENSOR DE UMIDADE DO SOLO
// ==========================================

#define PINO_SOLO A0

// CALIBRACAO DO SENSOR

const int SOLO_SECO = 800;
const int SOLO_MOLHADO = 350;

// ==========================================
// BOMBA DE AGUA
// ==========================================

#define PINO_BOMBA 7

#define BOMBA_LIGADA HIGH
#define BOMBA_DESLIGADA LOW

// ==========================================
// PARAMETROS DE IRRIGACAO
// ==========================================

const int LIMITE_IRRIGACAO = 35;

const unsigned long TEMPO_BOMBA = 1000;
const unsigned long TEMPO_ABSORCAO = 60000;

// ==========================================
// INTERVALOS
// ==========================================

const unsigned long INTERVALO_DHT = 2500;
const unsigned long INTERVALO_SOLO = 500;
const unsigned long INTERVALO_TELA = 1000;

// ==========================================
// VARIAVEIS GERAIS
// ==========================================

float temperatura = 0;
float umidadeAr = 0;

int leituraSolo = 0;
int umidadeSolo = 0;

bool dhtOK = false;
bool soloOK = false;
bool bombaLigada = false;

// CONTADOR DE ACIONAMENTOS

unsigned long acionamentosBomba = 0;

// TEMPORIZADORES

unsigned long ultimaLeituraDHT = 0;
unsigned long ultimaLeituraSolo = 0;
unsigned long ultimaTela = 0;
unsigned long ultimaSerial = 0;

unsigned long inicioBomba = 0;
unsigned long fimUltimaIrrigacao = 0;

bool primeiraIrrigacao = true;

// ==========================================
// LEITURA DO SENSOR DE SOLO
// ==========================================

int lerSolo() {

  long soma = 0;

  for (int i = 0; i < 10; i++) {

    soma += analogRead(PINO_SOLO);

    delay(2);
  }

  return soma / 10;
}

// ==========================================
// CONVERSAO PARA PORCENTAGEM
// ==========================================

int calcularUmidade(int valor) {

  int percentual = map(
    valor,
    SOLO_SECO,
    SOLO_MOLHADO,
    0,
    100
  );

  return constrain(percentual, 0, 100);
}

// ==========================================
// ATUALIZACAO DO DHT11
// ==========================================

void atualizarDHT() {

  unsigned long agora = millis();

  if (agora - ultimaLeituraDHT >= INTERVALO_DHT) {

    ultimaLeituraDHT = agora;

    float t = dht.readTemperature();
    float u = dht.readHumidity();

    if (!isnan(t) && !isnan(u)) {

      temperatura = t;
      umidadeAr = u;

      dhtOK = true;

    } else {

      dhtOK = false;
    }
  }
}

// ==========================================
// ATUALIZACAO DO SENSOR DE SOLO
// ==========================================

void atualizarSolo() {

  unsigned long agora = millis();

  if (agora - ultimaLeituraSolo >= INTERVALO_SOLO) {

    ultimaLeituraSolo = agora;

    leituraSolo = lerSolo();

    umidadeSolo = calcularUmidade(leituraSolo);

    soloOK = true;
  }
}

// ==========================================
// CONTROLE DA BOMBA
// ==========================================

void controlarBomba() {

  unsigned long agora = millis();

  // DESLIGA A BOMBA APOS 3 SEGUNDOS

  if (bombaLigada) {

    if (agora - inicioBomba >= TEMPO_BOMBA) {

      digitalWrite(PINO_BOMBA, BOMBA_DESLIGADA);

      bombaLigada = false;

      fimUltimaIrrigacao = agora;

      Serial.println(F("BOMBA DESLIGADA"));
    }

    return;
  }

  // VERIFICA SE EXISTE LEITURA DO SOLO

  if (!soloOK) return;

  // VERIFICA NECESSIDADE DE IRRIGACAO

  if (umidadeSolo <= LIMITE_IRRIGACAO) {

    // AGUARDA A ABSORCAO DA AGUA

    if (primeiraIrrigacao ||
        agora - fimUltimaIrrigacao >= TEMPO_ABSORCAO) {

      digitalWrite(PINO_BOMBA, BOMBA_LIGADA);

      bombaLigada = true;

      inicioBomba = agora;

      primeiraIrrigacao = false;

      // INCREMENTA O CONTADOR

      acionamentosBomba++;

      Serial.println(F("IRRIGACAO INICIADA"));

      Serial.print(F("ACIONAMENTOS: "));
      Serial.println(acionamentosBomba);
    }
  }
}

// ==========================================
// ATUALIZACAO DO OLED
// ==========================================

void atualizarDisplay() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // CABECALHO

  display.setTextSize(1);

  display.setCursor(10, 0);
  display.println(F("JARDIM INTELIGENTE"));

  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  // TEMPERATURA

  display.setCursor(2, 14);
  display.print(F("TEMPERATURA"));

  display.setTextSize(2);

  display.setCursor(2, 23);

  if (dhtOK) {

    display.print(temperatura, 1);
    display.print(F("C"));

  } else {

    display.print(F("--"));
  }

  // UMIDADE DO AR

  display.setTextSize(1);

  display.setCursor(76, 14);
  display.print(F("UMIDADE"));

  display.setTextSize(2);

  display.setCursor(76, 23);

  if (dhtOK) {

    display.print(umidadeAr, 0);
    display.print(F("%"));

  } else {

    display.print(F("--"));
  }

  // DIVISORIA

  display.drawLine(0, 43, 127, 43, SSD1306_WHITE);

  // UMIDADE DO SOLO

  display.setTextSize(1);

  display.setCursor(2, 46);
  display.print(F("SOLO"));

  display.setTextSize(1);

  display.setCursor(2, 56);

  if (soloOK) {

    display.print(umidadeSolo);
    display.print(F("%"));

  } else {

    display.print(F("--"));
  }

  // CONTADOR DE ACIONAMENTOS

  display.setCursor(76, 46);
  display.print(F("BOMBA"));

  display.setTextSize(1);

  display.setCursor(76, 56);

  display.print(acionamentosBomba);
  display.print(F(" VEZES"));

  display.display();
}

// ==========================================
// MONITOR SERIAL
// ==========================================

void monitorSerial() {

  Serial.println(F("--------------------------"));

  Serial.print(F("Temperatura: "));

  if (dhtOK) {

    Serial.print(temperatura);
    Serial.print(F(" C"));

  } else {

    Serial.print(F("ERRO"));
  }

  Serial.print(F(" | Umidade ar: "));

  if (dhtOK) {

    Serial.print(umidadeAr);
    Serial.print(F("%"));

  } else {

    Serial.print(F("ERRO"));
  }

  Serial.print(F(" | Solo: "));
  Serial.print(umidadeSolo);
  Serial.print(F("%"));

  Serial.print(F(" | ADC: "));
  Serial.print(leituraSolo);

  Serial.print(F(" | Acionamentos: "));
  Serial.println(acionamentosBomba);
}

// ==========================================
// SETUP
// ==========================================

void setup() {

  Serial.begin(9600);

  // BOMBA DESLIGADA NA INICIALIZACAO

  digitalWrite(PINO_BOMBA, BOMBA_DESLIGADA);

  pinMode(PINO_BOMBA, OUTPUT);

  dht.begin();

  // INICIALIZA OLED

  if (!display.begin(
    SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println(F("ERRO NO OLED"));

    while (true);
  }

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(38, 10);
  display.println(F("IRRIGADOR"));

  display.setTextSize(1);

  display.setCursor(28, 38);
  display.println(F("AUTOMATIZADO"));

  display.display();

  delay(2000);

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(42, 10);
  display.println(F("JULIANO"));

  display.setTextSize(1);

  display.setCursor(40, 38);
  display.println(F("HOFFMANN"));
  
  display.display();

  delay(2000);

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(4);

  display.setCursor(15, 28);
  display.println(F("2026"));

  display.display();

  delay(2000);

  display.clearDisplay();
  display.display();

  Serial.println(F("JARDIM INTELIGENTE"));
  Serial.println(F("MONITORAMENTO INICIADO"));
  Serial.println(F("CONTADOR INICIADO EM ZERO"));
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================

void loop() {

  atualizarDHT();

  atualizarSolo();

  controlarBomba();

  unsigned long agora = millis();

  // ATUALIZA OLED

  if (agora - ultimaTela >= INTERVALO_TELA) {

    ultimaTela = agora;

    atualizarDisplay();
  }

  // ATUALIZA MONITOR SERIAL

  if (agora - ultimaSerial >= 2000) {

    ultimaSerial = agora;

    monitorSerial();
  }
}
