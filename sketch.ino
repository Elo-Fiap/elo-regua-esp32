/*
  Elo - Régua de nível do córrego
  FIAP Global Solution 2026.2 (CitizenTech) - Physical Computing: IoT & IoB

  O Elo é o app em que a liderança de cada rua responde, em um toque, como a
  rua está durante a chuva forte. Esta régua é a parte física do Elo: fica no
  ponto crítico do córrego e mede o nível da água.

  O que o circuito faz:
    1. Mede a distância do sensor até a água (HC-SR04).
    2. Calcula o nível: altura do sensor menos a distância medida.
    3. Mostra o nível e a situação no LCD e nos LEDs.
    4. Toca o buzzer sem depender de internet: um bip lento a partir de 1,00 m
       (atenção) e um bip rápido a partir do limite de 1,20 m.
    5. Monta a leitura no formato que a API do Elo recebe (POST /leituras) e
       mostra no Monitor Serial.

  O envio da leitura por Wi-Fi entra quando a API do Elo tiver um endereço
  público. Quando isso acontecer, é dessa leitura que o app vai tirar o nível
  que mostra na tela ("1,42 m"); o limite ("1,20 m") fica guardado no servidor.
  Até lá, a régua funciona sozinha, no local, e o app mostra um exemplo.
*/

#include <algorithm>  // std::sort, usado para ordenar as leituras do sensor
#include <LiquidCrystal_I2C.h>

// ---------- Pinos do ESP32 ----------
const int PINO_TRIG = 5;           // HC-SR04: dispara o pulso de ultrassom
const int PINO_ECHO = 18;          // HC-SR04: recebe o eco
const int PINO_BUZZER = 19;
const int PINO_LED_VERMELHO = 25;  // acima do limite
const int PINO_LED_AMARELO = 26;   // atenção
const int PINO_LED_VERDE = 27;     // normal
// O LCD usa I2C nos pinos padrão do ESP32: SDA no 21 e SCL no 22.
// No circuito real, o ESP32 trabalha em 3,3 V e não pode receber 5 V. O ECHO do
// HC-SR04 precisa de um divisor de tensão, e os fios SDA e SCL do LCD, de um
// conversor de nível. No simulador não precisa.

// ---------- Ajustes da régua ----------
// As medidas ficam em centímetros, em números inteiros, para a comparação com
// o limite ser exata. Na tela e na leitura elas aparecem em metros.
// A altura, o limite e o nível de atenção são valores de exemplo. O limite é o
// mesmo do exemplo do app; o nível de atenção existe só na régua.
const char* REGUA_ID = "regua-exemplo";  // mesmo id do exemplo do app
const int ALTURA_SENSOR_CM = 300;        // sensor a 3,00 m do fundo do córrego
const int LIMITE_CM = 120;               // 1,20 m: o mesmo limite que o app mostra
const int ATENCAO_CM = 100;              // 1,00 m: aviso antes do limite

// ---------- Tempos ----------
const unsigned long INTERVALO_LEITURA_MS = 200;   // uma leitura do sensor a cada 200 ms
const int LEITURAS_POR_MEDIDA = 5;                // 5 leituras formam 1 medida (1 por segundo)
const unsigned long INTERVALO_NORMAL_MS = 30000;  // contrato da API: leitura a cada 30 s
const unsigned long INTERVALO_ACIMA_MS = 10000;   // contrato da API: a cada 10 s acima do limite

// ---------- Situações da régua ----------
enum Situacao { SEM_LEITURA, NORMAL, ATENCAO, ACIMA_DO_LIMITE };

LiquidCrystal_I2C lcd(0x27, 16, 2);  // endereço I2C 0x27, 16 colunas, 2 linhas

Situacao situacao = SEM_LEITURA;           // situação da última medida
Situacao situacaoPublicada = SEM_LEITURA;  // situação da última leitura publicada
int nivelCm = 0;                           // nível da água na última medida

float leituras[LEITURAS_POR_MEDIDA];  // leituras válidas da medida em andamento
int leiturasValidas = 0;
int leiturasFeitas = 0;

unsigned long ultimaLeituraMs = 0;
unsigned long ultimaPublicacaoMs = 0;
int frequenciaBuzzer = 0;  // 0 = em silêncio

// Transforma centímetros em texto de metros com 2 casas: 142 vira "1,42".
// O separador é vírgula no LCD (como no app) e ponto no JSON (regra do formato).
String emMetros(int centimetros, char separador) {
  char texto[12];
  snprintf(texto, sizeof(texto), "%d%c%02d", centimetros / 100, separador, centimetros % 100);
  return String(texto);
}

// Nome da situação, sem acento: o LCD não tem letras acentuadas.
const char* nomeSituacao(Situacao s) {
  if (s == ACIMA_DO_LIMITE) return "ACIMA DO LIMITE";
  if (s == ATENCAO) return "ATENCAO";
  if (s == NORMAL) return "NORMAL";
  return "SEM LEITURA";
}

// Mesma regra do app: está acima do limite quando nível >= limite.
Situacao classificar(int nivel) {
  if (nivel >= LIMITE_CM) return ACIMA_DO_LIMITE;
  if (nivel >= ATENCAO_CM) return ATENCAO;
  return NORMAL;
}

// Faz uma leitura do HC-SR04 e devolve a distância em centímetros.
// Devolve 0 quando o eco não volta (sensor desligado ou com defeito).
float lerDistanciaCm() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);  // um pulso de 10 microssegundos dispara o ultrassom
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  // Tempo que o eco levou para voltar, em microssegundos (espera no máximo 30 ms).
  unsigned long duracao = pulseIn(PINO_ECHO, HIGH, 30000);

  // O som anda 0,034 cm por microssegundo (340 m/s). Como o eco vai e volta,
  // a distância até a água é a metade do caminho.
  return duracao * 0.034 / 2;
}

// Escreve um texto em uma linha do LCD e completa com espaços até a coluna 16,
// para apagar o que estava escrito antes.
void escreverLinha(int linha, String texto) {
  while (texto.length() < 16) texto += ' ';
  lcd.setCursor(0, linha);
  lcd.print(texto);
}

void atualizarLcd() {
  if (situacao == SEM_LEITURA) {
    escreverLinha(0, "Nivel: --");
  } else {
    escreverLinha(0, "Nivel: " + emMetros(nivelCm, ',') + " m");
  }
  escreverLinha(1, nomeSituacao(situacao));
}

// Acende só o LED da situação atual (true acende, false apaga).
void atualizarLeds() {
  digitalWrite(PINO_LED_VERDE, situacao == NORMAL);
  digitalWrite(PINO_LED_AMARELO, situacao == ATENCAO);
  digitalWrite(PINO_LED_VERMELHO, situacao == ACIMA_DO_LIMITE);
}

// Toca o buzzer conforme a situação, sem parar o resto do programa:
//   acima do limite: bip rápido e agudo (0,2 s tocando, 0,2 s em silêncio)
//   atenção:         um bip curto e grave a cada 1,5 s
//   normal:          silêncio
void atualizarBuzzer(unsigned long agora) {
  int desejada = 0;  // frequência que o buzzer deve tocar agora (0 = silêncio)
  if (situacao == ACIMA_DO_LIMITE && (agora % 400) < 200) desejada = 2000;
  if (situacao == ATENCAO && (agora % 1500) < 150) desejada = 1000;

  if (desejada != frequenciaBuzzer) {  // só mexe no buzzer quando algo muda
    frequenciaBuzzer = desejada;
    if (desejada > 0) tone(PINO_BUZZER, desejada);
    else noTone(PINO_BUZZER);
  }
}

// Mostra a leitura no Monitor Serial, no formato do contrato da API do Elo
// (docs/contrato-api.md do app). É este texto que a régua vai enviar no
// POST /leituras quando a API tiver um endereço público.
void publicarLeitura(unsigned long agora) {
  if (situacao == SEM_LEITURA) {
    Serial.printf("[%4lu s] Sem leitura do sensor: nada para enviar.\n", agora / 1000);
    return;
  }

  String leitura = "{\"reguaId\":\"" + String(REGUA_ID) + "\",\"nivelMetros\":" + emMetros(nivelCm, '.') + "}";

  Serial.printf("[%4lu s] Situacao: %s | Leitura para POST /leituras: %s\n", agora / 1000, nomeSituacao(situacao), leitura.c_str());
}

// Junta as leituras do último segundo em uma medida: calcula o nível, decide
// a situação e atualiza os LEDs e o LCD.
void fecharMedida() {
  if (leiturasValidas == 0) {
    situacao = SEM_LEITURA;
  } else {
    // Ordena as leituras e fica com a do meio (a mediana). Assim, uma leitura
    // fora da curva, ou feita bem na hora em que o nível mudou, não estraga a medida.
    std::sort(leituras, leituras + leiturasValidas);
    int distanciaCm = lround(leituras[leiturasValidas / 2]);
    nivelCm = ALTURA_SENSOR_CM - distanciaCm;
    if (nivelCm < 0) nivelCm = 0;  // distância maior que a altura do sensor: conta como 0
    situacao = classificar(nivelCm);
  }

  leiturasValidas = 0;
  leiturasFeitas = 0;

  atualizarLeds();
  atualizarLcd();
}

void setup() {
  Serial.begin(115200);

  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_LED_AMARELO, OUTPUT);
  pinMode(PINO_LED_VERMELHO, OUTPUT);

  lcd.init();
  lcd.backlight();
  escreverLinha(0, "Elo - Regua");
  escreverLinha(1, "Iniciando...");

  Serial.println();
  Serial.println("Elo - regua de nivel do corrego");
  Serial.println("Regua " + String(REGUA_ID) + " | sensor a " + emMetros(ALTURA_SENSOR_CM, ',') + " m | limite " + emMetros(LIMITE_CM, ',') + " m | atencao a partir de " + emMetros(ATENCAO_CM, ',') + " m");
  Serial.println("A leitura aparece aqui no formato que a API do Elo recebe (POST /leituras).");
}

void loop() {
  unsigned long agora = millis();

  // 1) Sensor: uma leitura a cada 200 ms. Com 5 leituras, fecha uma medida.
  if (agora - ultimaLeituraMs >= INTERVALO_LEITURA_MS) {
    ultimaLeituraMs = agora;

    float distancia = lerDistanciaCm();
    if (distancia > 0) {  // 0 = o eco não voltou: a leitura é ignorada
      leituras[leiturasValidas] = distancia;
      leiturasValidas++;
    }
    leiturasFeitas++;

    if (leiturasFeitas == LEITURAS_POR_MEDIDA) {
      fecharMedida();

      // 2) Publicação: na hora em que a situação muda e, depois, no intervalo do contrato.
      unsigned long intervalo = (situacao == ACIMA_DO_LIMITE) ? INTERVALO_ACIMA_MS : INTERVALO_NORMAL_MS;
      if (situacao != situacaoPublicada || agora - ultimaPublicacaoMs >= intervalo) {
        publicarLeitura(agora);
        situacaoPublicada = situacao;
        ultimaPublicacaoMs = agora;
      }
    }
  }

  // 3) Buzzer: confere a todo instante se é hora de tocar ou de silenciar.
  atualizarBuzzer(agora);

  // 4) Pausa de 1 ms: deixa a simulação do Wokwi mais leve.
  delay(1);
}
