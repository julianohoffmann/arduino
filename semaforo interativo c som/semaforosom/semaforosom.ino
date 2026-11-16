/*
  PROJETO: ESTACAO SEMAFORICA INTERATIVA
  ACESSIBILIDADE SONORA
  ARDUINO UNO R3

  CICLO CONVENCIONAL:
  CARROS VERDE       / PEDESTRES VERMELHO
  CARROS AMARELO     / PEDESTRES VERMELHO
  CARROS VERMELHO    / PEDESTRES VERDE
  CARROS VERMELHO    / PEDESTRES AMARELO
  CARROS VERMELHO    / PEDESTRES VERMELHO

  MODO INTERATIVO:
  BOTAO PRESSIONADO
  TODOS VERMELHOS + SEGURANCA
  PEDESTRES VERDE + BUZZER
  PEDESTRES AMARELO + AVISO SONORO FINAL
  TODOS VERMELHOS
  RETORNO AO CICLO CONVENCIONAL

*/

// PINAGEM DOS CARROS
#define CARRO_VERMELHO 13
#define CARRO_AMARELO  12
#define CARRO_VERDE    11

// PINAGEM DOS PEDESTRES
#define PEDESTRE_VERMELHO 10
#define PEDESTRE_AMARELO  9
#define PEDESTRE_VERDE    8

// ACESSIBILIDADE E BOTAO
#define BUZZER 7
#define BOTAO  2

// TEMPOS
const unsigned long TEMPO_CARRO_VERDE = 8000;
const unsigned long TEMPO_CARRO_AMARELO = 3000;

const unsigned long TEMPO_PEDESTRE_VERDE = 8000;
const unsigned long TEMPO_PEDESTRE_AMARELO = 3000;

const unsigned long TEMPO_TODOS_VERMELHOS = 2000;

const unsigned long TEMPO_INTERATIVO = 8000;

const unsigned long TEMPO_DEBOUNCE = 50;
const unsigned long INTERVALO_BUZZER = 500;

// INTERVALO DO ALERTA SONORO FINAL
const unsigned long INTERVALO_ALERTA_SONORO = 250;

// ========================================================
// ESTADOS DO SISTEMA
// ========================================================

enum Estado {

  CONV_CARROS_VERDE,
  CONV_CARROS_AMARELO,
  CONV_PEDESTRES_VERDE,
  CONV_PEDESTRES_AMARELO,
  CONV_TODOS_VERMELHOS,

  INTERATIVO_SEGURANCA,
  INTERATIVO_TRAVESSIA,
  INTERATIVO_ALERTA,
  INTERATIVO_FINAL_SEGURANCA
};

Estado estadoAtual = CONV_CARROS_VERDE;

// ========================================================
// VARIAVEIS
// ========================================================

unsigned long tempoEstado = 0;
unsigned long tempoBuzzer = 0;

// NOVO: CONTROLE DO ALERTA SONORO FINAL
unsigned long tempoAlertaSonoro = 0;
bool alertaSonoroLigado = false;

bool solicitacaoInterativa = false;
bool buzzerLigado = false;

bool leituraAnterior = HIGH;
bool estadoBotao = HIGH;

unsigned long tempoDebounce = 0;

// ========================================================
// DESLIGAR TODOS OS LEDS
// ========================================================

void desligarTodosLEDs() {

  digitalWrite(CARRO_VERMELHO, LOW);
  digitalWrite(CARRO_AMARELO, LOW);
  digitalWrite(CARRO_VERDE, LOW);

  digitalWrite(PEDESTRE_VERMELHO, LOW);
  digitalWrite(PEDESTRE_AMARELO, LOW);
  digitalWrite(PEDESTRE_VERDE, LOW);
}

// ========================================================
// INICIAR ESTADO
// ========================================================

void iniciarEstado(Estado novoEstado) {

  desligarTodosLEDs();

  noTone(BUZZER);

  estadoAtual = novoEstado;

  tempoEstado = millis();
}

// ========================================================
// CICLO CONVENCIONAL
// ========================================================

// CARROS VERDE / PEDESTRES VERMELHO

void iniciarCarrosVerde() {

  iniciarEstado(CONV_CARROS_VERDE);

  digitalWrite(CARRO_VERDE, HIGH);
  digitalWrite(PEDESTRE_VERMELHO, HIGH);
}

// CARROS AMARELO / PEDESTRES VERMELHO

void iniciarCarrosAmarelo() {

  iniciarEstado(CONV_CARROS_AMARELO);

  digitalWrite(CARRO_AMARELO, HIGH);
  digitalWrite(PEDESTRE_VERMELHO, HIGH);
}

// CARROS VERMELHO / PEDESTRES VERDE

void iniciarPedestresVerde() {

  iniciarEstado(CONV_PEDESTRES_VERDE);

  digitalWrite(CARRO_VERMELHO, HIGH);
  digitalWrite(PEDESTRE_VERDE, HIGH);
}

// CARROS VERMELHO / PEDESTRES AMARELO

void iniciarPedestresAmarelo() {

  iniciarEstado(CONV_PEDESTRES_AMARELO);

  digitalWrite(CARRO_VERMELHO, HIGH);

  digitalWrite(PEDESTRE_AMARELO, HIGH);
}

// TODOS VERMELHOS

void iniciarTodosVermelhos() {

  iniciarEstado(CONV_TODOS_VERMELHOS);

  digitalWrite(CARRO_VERMELHO, HIGH);
  digitalWrite(PEDESTRE_VERMELHO, HIGH);
}

// ========================================================
// MODO INTERATIVO
// ========================================================

// SEGURANCA INICIAL

void iniciarInterativoSeguranca() {

  iniciarEstado(INTERATIVO_SEGURANCA);

  digitalWrite(CARRO_VERMELHO, HIGH);
  digitalWrite(PEDESTRE_VERMELHO, HIGH);
}

// TRAVESSIA INTERATIVA

void iniciarInterativoTravessia() {

  iniciarEstado(INTERATIVO_TRAVESSIA);

  digitalWrite(CARRO_VERMELHO, HIGH);
  digitalWrite(PEDESTRE_VERDE, HIGH);

  tempoBuzzer = millis();
  buzzerLigado = true;

  tone(BUZZER, 1000);
}

// ========================================================
// ALERTA AMARELO INTERATIVO
// ========================================================

void iniciarInterativoAlerta() {

  iniciarEstado(INTERATIVO_ALERTA);

  digitalWrite(CARRO_VERMELHO, HIGH);

  // AMARELO DOS PEDESTRES ACESO CONTINUAMENTE
  digitalWrite(PEDESTRE_AMARELO, HIGH);

  // NOVO: INICIA ALERTA SONORO FINAL
  tempoAlertaSonoro = millis();
  alertaSonoroLigado = false;

  noTone(BUZZER);
}

// ========================================================
// SEGURANCA FINAL INTERATIVA
// ========================================================

void iniciarInterativoFinalSeguranca() {

  iniciarEstado(INTERATIVO_FINAL_SEGURANCA);

  digitalWrite(CARRO_VERMELHO, HIGH);
  digitalWrite(PEDESTRE_VERMELHO, HIGH);
}

// ========================================================
// BOTAO COM DEBOUNCE
// ========================================================

void verificarBotao() {

  bool leitura = digitalRead(BOTAO);

  if (leitura != leituraAnterior) {

    tempoDebounce = millis();
  }

  if (millis() - tempoDebounce >= TEMPO_DEBOUNCE) {

    if (leitura != estadoBotao) {

      estadoBotao = leitura;

      if (estadoBotao == LOW) {

        solicitacaoInterativa = true;
      }
    }
  }

  leituraAnterior = leitura;
}

// ========================================================
// BUZZER DA TRAVESSIA
// ========================================================

void controlarBuzzer() {

  if (millis() - tempoBuzzer >= INTERVALO_BUZZER) {

    tempoBuzzer = millis();

    buzzerLigado = !buzzerLigado;

    if (buzzerLigado) {

      tone(BUZZER, 1000);

    } else {

      noTone(BUZZER);
    }
  }
}

// ========================================================
// NOVO: AVISO SONORO FINAL DE ALERTA
// ========================================================

void controlarAlertaSonoro() {

  if (millis() - tempoAlertaSonoro >= INTERVALO_ALERTA_SONORO) {

    tempoAlertaSonoro = millis();

    alertaSonoroLigado = !alertaSonoroLigado;

    if (alertaSonoroLigado) {

      // BIP DE ALERTA FINAL
      tone(BUZZER, 2000);

    } else {

      noTone(BUZZER);
    }
  }
}

// ========================================================
// SETUP
// ========================================================

void setup() {

  pinMode(CARRO_VERMELHO, OUTPUT);
  pinMode(CARRO_AMARELO, OUTPUT);
  pinMode(CARRO_VERDE, OUTPUT);

  pinMode(PEDESTRE_VERMELHO, OUTPUT);
  pinMode(PEDESTRE_AMARELO, OUTPUT);
  pinMode(PEDESTRE_VERDE, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  pinMode(BOTAO, INPUT_PULLUP);

  desligarTodosLEDs();

  // INICIO AUTOMATICO DO CICLO CONVENCIONAL
  iniciarCarrosVerde();
}

// ========================================================
// LOOP PRINCIPAL
// ========================================================

void loop() {

  verificarBotao();

  unsigned long agora = millis();

  // ======================================================
  // SOLICITACAO DO MODO INTERATIVO
  // ======================================================

  if (solicitacaoInterativa) {

    if (estadoAtual == CONV_CARROS_VERDE ||
        estadoAtual == CONV_CARROS_AMARELO ||
        estadoAtual == CONV_PEDESTRES_VERDE ||
        estadoAtual == CONV_PEDESTRES_AMARELO ||
        estadoAtual == CONV_TODOS_VERMELHOS) {

      solicitacaoInterativa = false;

      iniciarInterativoSeguranca();
    }
  }

  // ======================================================
  // CONTROLE DOS ESTADOS
  // ======================================================

  switch (estadoAtual) {

    // ----------------------------------------------------
    // CARROS VERDE
    // ----------------------------------------------------

    case CONV_CARROS_VERDE:

      if (agora - tempoEstado >= TEMPO_CARRO_VERDE) {

        iniciarCarrosAmarelo();
      }

      break;

    // ----------------------------------------------------
    // CARROS AMARELO
    // ----------------------------------------------------

    case CONV_CARROS_AMARELO:

      if (agora - tempoEstado >= TEMPO_CARRO_AMARELO) {

        iniciarPedestresVerde();
      }

      break;

    // ----------------------------------------------------
    // PEDESTRES VERDE
    // ----------------------------------------------------

    case CONV_PEDESTRES_VERDE:

      if (agora - tempoEstado >= TEMPO_PEDESTRE_VERDE) {

        iniciarPedestresAmarelo();
      }

      break;

    // ----------------------------------------------------
    // PEDESTRES AMARELO
    // ----------------------------------------------------

    case CONV_PEDESTRES_AMARELO:

      if (agora - tempoEstado >= TEMPO_PEDESTRE_AMARELO) {

        iniciarTodosVermelhos();
      }

      break;

    // ----------------------------------------------------
    // TODOS VERMELHOS - CICLO CONVENCIONAL
    // ----------------------------------------------------

    case CONV_TODOS_VERMELHOS:

      if (agora - tempoEstado >= TEMPO_TODOS_VERMELHOS) {

        iniciarCarrosVerde();
      }

      break;

    // ----------------------------------------------------
    // INTERATIVO - SEGURANCA INICIAL
    // ----------------------------------------------------

    case INTERATIVO_SEGURANCA:

      if (agora - tempoEstado >= TEMPO_TODOS_VERMELHOS) {

        iniciarInterativoTravessia();
      }

      break;

    // ----------------------------------------------------
    // INTERATIVO - TRAVESSIA
    // ----------------------------------------------------

    case INTERATIVO_TRAVESSIA:

      controlarBuzzer();

      if (agora - tempoEstado >= TEMPO_INTERATIVO) {

        iniciarInterativoAlerta();
      }

      break;

    // ----------------------------------------------------
    // INTERATIVO - ALERTA FINAL
    // ----------------------------------------------------

    case INTERATIVO_ALERTA:

      // NOVO: BIPS RAPIDOS DE ALERTA FINAL
      controlarAlertaSonoro();

      if (agora - tempoEstado >= TEMPO_PEDESTRE_AMARELO) {

        iniciarInterativoFinalSeguranca();
      }

      break;

    // ----------------------------------------------------
    // INTERATIVO - SEGURANCA FINAL
    // ----------------------------------------------------

    case INTERATIVO_FINAL_SEGURANCA:

      if (agora - tempoEstado >= TEMPO_TODOS_VERMELHOS) {

        // RETORNA AO CICLO CONVENCIONAL
        iniciarCarrosVerde();
      }

      break;
  }
}
