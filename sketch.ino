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
    5. Monta a leitura no formato que a API do Elo recebe (POST /leituras),
       mostra no Monitor Serial e envia pela rede (Wi-Fi), sem parar o aviso local.

  A API do Elo (elo-api-csharp) recebe essa leitura, e é dela que o app tira o
  nível que mostra na tela ("1,42 m"). O limite ("1,20 m") e o nível de atenção
  ("1,00 m") ficam guardados no servidor, com os mesmos valores daqui.
  A API está publicada na nuvem (Azure, plano gratuito): depois de um tempo parada,
  a primeira resposta pode demorar até ela acordar. Sem rede, a régua continua
  funcionando sozinha, no local.
*/

#include <algorithm>  // std::sort, usado para ordenar as leituras do sensor
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>        // Wi-Fi do ESP32
#include <HTTPClient.h>  // pedido HTTP (POST /leituras)

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
// A altura, o limite e o nível de atenção são valores de exemplo. O limite e o
// nível de atenção são os mesmos do cadastro da régua no servidor, que o app mostra.
const char* REGUA_ID = "regua-exemplo";  // mesmo id do cadastro no servidor e do exemplo do app
const int ALTURA_SENSOR_CM = 300;        // sensor a 3,00 m do fundo do córrego
const int LIMITE_CM = 120;               // 1,20 m: o mesmo limite que o app mostra
const int ATENCAO_CM = 100;              // 1,00 m: aviso antes do limite

// ---------- Tempos ----------
const unsigned long INTERVALO_LEITURA_MS = 200;   // uma leitura do sensor a cada 200 ms
const int LEITURAS_POR_MEDIDA = 5;                // 5 leituras formam 1 medida (1 por segundo)
const unsigned long INTERVALO_NORMAL_MS = 30000;  // contrato da API: leitura a cada 30 s
const unsigned long INTERVALO_ACIMA_MS = 10000;   // contrato da API: a cada 10 s acima do limite

// ---------- Envio pela rede ----------
// Rede Wi-Fi. No simulador Wokwi existe a rede aberta "Wokwi-GUEST", sem senha.
const char* WIFI_REDE = "Wokwi-GUEST";
const char* WIFI_SENHA = "";
// Para onde vai o POST /leituras: a API do Elo publicada (HTTP simples, como o ESP32 precisa).
// Vazio ("") desliga o envio: a leitura só aparece no Monitor Serial. Para testar sem a API,
// um serviço público de eco serve: "http://httpbin.org/post" (responde 200 com o que recebeu).
const char* URL_LEITURAS = "http://elo-api-fiap.azurewebsites.net/leituras";
// Chave do dispositivo, enviada no header X-Chave-Dispositivo. Precisa ser igual à Dispositivos__Chave
// configurada na API. Este projeto é público, então esta é uma chave só de demonstração, combinada com
// o grupo, que vai ser trocada depois da entrega.
const char* CHAVE_DISPOSITIVO = "elo-demo-d651f252b286";
const unsigned long ESPERA_WIFI_MS = 8000;         // ao ligar, espera o Wi-Fi conectar por até 8 s
const unsigned long TEMPO_MAXIMO_ENVIO_MS = 8000;  // desiste de um envio depois de 8 s (a API no plano gratuito pode demorar a acordar)

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

// Uma leitura esperando para ser enviada. O texto já vem pronto, no formato do contrato.
struct Envio {
  char corpo[64];
};
QueueHandle_t filaDeEnvios = nullptr;  // fila entre o programa principal e a tarefa de envio
volatile char marcadorEnvio = ' ';     // canto do LCD: '*' último envio OK, '!' falhou, ' ' nenhum ainda

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

// Mesma regra do app e do servidor: está acima do limite quando nível >= limite.
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
  String linha1 = "Nivel: --";
  if (situacao != SEM_LEITURA) linha1 = "Nivel: " + emMetros(nivelCm, ',') + " m";
  while (linha1.length() < 15) linha1 += ' ';
  linha1 += (char)marcadorEnvio;  // canto direito: como foi o último envio pela rede
  escreverLinha(0, linha1);
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

// Faz o POST /leituras. Sucesso é qualquer resposta 2xx: a API do Elo responde 204,
// e o serviço de eco, 200. Roda na tarefa de envio, nunca no loop().
void enviarLeitura(const char* corpo) {
  unsigned long inicio = millis();
  if (WiFi.status() != WL_CONNECTED) {
    marcadorEnvio = '!';
    Serial.printf("[%4lu s] Sem Wi-Fi: leitura nao enviada.\n", inicio / 1000);
    return;
  }

  WiFiClient conexao;
  HTTPClient http;
  http.setConnectTimeout(TEMPO_MAXIMO_ENVIO_MS);
  http.setTimeout(TEMPO_MAXIMO_ENVIO_MS);
  http.begin(conexao, URL_LEITURAS);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Chave-Dispositivo", CHAVE_DISPOSITIVO);
  int codigo = http.POST(corpo);
  unsigned long duracao = millis() - inicio;

  if (codigo >= 200 && codigo < 300) {
    marcadorEnvio = '*';
    Serial.printf("[%4lu s] Envio OK: HTTP %d em %lu ms.\n", millis() / 1000, codigo, duracao);
  } else if (codigo > 0) {
    marcadorEnvio = '!';  // o servidor respondeu, mas recusou (por exemplo 401: chave errada)
    Serial.printf("[%4lu s] Envio recusado: HTTP %d em %lu ms.\n", millis() / 1000, codigo, duracao);
  } else {
    marcadorEnvio = '!';  // não houve resposta (sem rede, endereço errado, tempo esgotado)
    Serial.printf("[%4lu s] Falha no envio: %s em %lu ms.\n", millis() / 1000, http.errorToString(codigo).c_str(), duracao);
  }
  http.end();
}

// Roda no outro núcleo do ESP32 (core 0) e fica esperando leituras na fila. Assim o pedido
// HTTP, que pode levar alguns segundos, não para o sensor, os LEDs nem o buzzer do loop().
void tarefaDeEnvio(void*) {
  Envio envio;
  for (;;) {
    if (xQueueReceive(filaDeEnvios, &envio, portMAX_DELAY) == pdTRUE) enviarLeitura(envio.corpo);
  }
}

// Liga o Wi-Fi e espera a conexão por até ESPERA_WIFI_MS. Se não conectar, a régua segue
// funcionando no local; o ESP32 continua tentando a rede sozinho, e o envio volta quando ela voltar.
void ligarEnvio() {
  escreverLinha(1, "Ligando Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_REDE, WIFI_SENHA, 6);  // canal 6: no Wokwi, conecta mais rápido

  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < ESPERA_WIFI_MS) delay(100);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi conectado em " + String(millis() - inicio) + " ms. Envio das leituras para " + String(URL_LEITURAS));
  } else {
    Serial.println("Sem Wi-Fi por enquanto. A regua funciona no local e envia quando a rede conectar.");
  }

  filaDeEnvios = xQueueCreate(4, sizeof(Envio));
  xTaskCreatePinnedToCore(tarefaDeEnvio, "envio", 10240, nullptr, 1, nullptr, 0);
}

// Monta a leitura no formato do contrato da API do Elo (docs/contrato-api.md do app),
// mostra no Monitor Serial e põe na fila de envio. Quem envia é a tarefa de envio.
void publicarLeitura(unsigned long agora) {
  if (situacao == SEM_LEITURA) {
    Serial.printf("[%4lu s] Sem leitura do sensor: nada para enviar.\n", agora / 1000);
    return;
  }

  String leitura = "{\"reguaId\":\"" + String(REGUA_ID) + "\",\"nivelMetros\":" + emMetros(nivelCm, '.') + "}";

  Serial.printf("[%4lu s] Situacao: %s | Leitura para POST /leituras: %s\n", agora / 1000, nomeSituacao(situacao), leitura.c_str());

  if (filaDeEnvios == nullptr) return;  // envio pela rede desligado

  Envio envio;
  strlcpy(envio.corpo, leitura.c_str(), sizeof(envio.corpo));
  if (xQueueSend(filaDeEnvios, &envio, 0) != pdTRUE) {
    Serial.printf("[%4lu s] Fila de envio cheia: esta leitura nao vai.\n", agora / 1000);
  }
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

  if (strlen(URL_LEITURAS) > 0) {
    ligarEnvio();
  } else {
    Serial.println("Envio pela rede desligado (URL_LEITURAS vazia): a leitura so aparece aqui.");
  }
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
