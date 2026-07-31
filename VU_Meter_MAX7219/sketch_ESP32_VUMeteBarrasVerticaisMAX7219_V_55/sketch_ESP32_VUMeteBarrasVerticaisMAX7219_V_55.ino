/*
Código gerado com o auxílio de Inteligência Artificial (IA) e refinado conforme as necessidades do projeto em 30/07/2026.
https://github.com/marcelovictorio @MarceloVictorio YouTube
*/
#include <Arduino.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// --- Configurações do Hardware da Matriz MAX7219 ---
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // Altere para GENERIC_HW se os LEDs ficarem invertidos
#define MAX_DEVICES  4                    // Matriz 8x32 (4 módulos 8x8)
#define CLK_PIN      18                   // SPI CLK
#define DATA_PIN     23                   // SPI MOSI
#define CS_PIN       5                    // SPI CS

// Inicializa o controle do display MAX7219 / MD_Parola
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// --- Configurações de Áudio (AudioTools + Bluetooth) ---
I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

// --- Variáveis Globais do VU Meter ---
const int NUM_COLUMNS = 32;
int raw_volume = 0;
int last_heights[NUM_COLUMNS] = {0};

bool bt_connected = false;
bool status_changed = false;

// --- Função para Exibir Texto Rolante ---
//==========================================================================
// --- Função auxiliar para inverter os bits de um byte ---
uint8_t reverseBits(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}
//==========================================================================
void printText(const char *pMsg, uint16_t delayTime = 50) {
    mx.clear();
    int cLen = strlen(pMsg);
    int totalWidth = cLen * 8;
    
    uint8_t cBuf[8]; // Buffer para armazenar os pixels de 1 caractere

    for (int i = -32; i < totalWidth; i++) {
    //for (int i = 32; i >= -totalWidth; i--) {
        mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
        mx.clear();
        
        int col = -i;
        
        for (int charIndex = 0; charIndex < cLen; charIndex++) {
            // 1. Obtém a matriz de pixels do caractere atual
            uint8_t cWidth = mx.getChar(pMsg[charIndex], sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
            
            // 2. Desenha cada coluna do caractere aplicando a inversão de bits
            for (uint8_t c = 0; c < cWidth; c++) {
                int targetCol = col + c;
                if (targetCol >= 0 && targetCol < 32) {
                    // Inverte os bits do byte para corrigir letras invertidas, problema relacionado ao drive do display
                    mx.setColumn(targetCol, reverseBits(cBuf[c]));
                }
            }
            col += cWidth + 1; // Espaçamento de 1 pixel entre letras
        }
        
        // --- CORREÇÃO DO ESPELHAMENTO VIA CÓDIGO ---
        // Aplica o espelhamento horizontal no buffer antes de enviar à tela
        mx.transform(MD_MAX72XX::TFLR);   // Corrige espelhamento horizontal (Direita para esquerda)
        mx.transform(MD_MAX72XX::TFUD); // Corrige orientação vertical (se necessário)

        mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
        delay(delayTime);
        
        if (bt_connected && status_changed) break;
    }
}
//==========================================================================

// --- Teste do Display na Inicialização ---
void runDisplayTest() {
    Serial.println("Executando teste do display...");
    
    // Acende todos os LEDs
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    for (uint8_t dev = 0; dev < MAX_DEVICES; dev++) {
        for (uint8_t row = 0; row < 8; row++) {
            mx.setRow(dev, row, 0xFF);
        }
    }
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
    delay(1200);

    mx.clear();
    delay(200);

    printText("Display OK!", 45);

}

// --- Callback de Eventos do Bluetooth ---
void connection_state_changed(esp_a2d_connection_state_t state, void *ptr) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        bt_connected = true;
        status_changed = true;
        Serial.println("\n[BT] Conectado com sucesso!");
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        bt_connected = false;
        status_changed = true;
        Serial.println("\n[BT] Desconectado!");
    }
}

// --- Callback para Captura e Cálculo do Volume do Áudio ---
void audio_data_callback(const uint8_t *data, uint32_t length) {
    if (!bt_connected) return;

    int16_t *samples = (int16_t *)data;
    uint32_t sample_count = length / 4; // Amostras de 16 bits estéreo
    
    int32_t sum_audio = 0;

    for (uint32_t i = 0; i < sample_count; i++) {
        int16_t left = samples[2 * i];
        int16_t right = samples[2 * i + 1];
        sum_audio += (abs(left) + abs(right)) / 2;
    }

    if (sample_count > 0) {
        raw_volume = sum_audio / sample_count;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    // 1. Inicializa o Display MAX7219
    if (!mx.begin()) {
        Serial.println("Erro ao inicializar o MAX7219!");
        while (1);
    }
    mx.control(MD_MAX72XX::INTENSITY, 3); // Brilho inicial (0 a 15)
    mx.clear();

    runDisplayTest();

    // 2. Configura o barramento I2S para o MAX98357A via AudioTools
    auto config = i2s.defaultConfig(TX_MODE);
    config.pin_bck = 26;   // BCLK -> GPIO 26
    config.pin_ws  = 25;   // LRC  -> GPIO 25
    config.pin_data = 22;  // DIN  -> GPIO 22
    config.sample_rate = 44100;
    config.bits_per_sample = 16;
    config.channels = 2;
    
    i2s.begin(config);

    // 3. Configura o Receptor Bluetooth A2DP
    a2dp_sink.set_on_connection_state_changed(connection_state_changed);
    a2dp_sink.set_stream_reader(audio_data_callback, true); // Captura o áudio mantendo o envio ao I2S

    a2dp_sink.start("ESP32_VU_Meter_8x32");
    Serial.println("Serviço A2DP iniciado: 'ESP32_VU_Meter_8x32'");

}

void loop() {
    // --- Trata Mudança no Status do Bluetooth ---
    if (status_changed) {
        status_changed = false;
        if (bt_connected) {
            printText("BT Conectado!", 45);

            delay(200);
            mx.clear();
        } else {
            raw_volume = 0;
            mx.clear();
        }
    }

    // --- MODO 1: Bluetooth DESCONECTADO ---
    if (!bt_connected) {
        printText("Aguardando BT...", 60);
      
        return;
    }

    // --- MODO 2: Bluetooth CONECTADO (Visualizador VU Meter) ---
    // Mapeia a amplitude RMS do áudio para a altura de 0 a 8 LEDs
    int target_height = map(raw_volume, 0, 12000, 0, 8);
    target_height = constrain(target_height, 0, 8);
    
    int vol = raw_volume; // Copia a variável volatile para escopo local
    int base_height = map(vol, 0, 12000, 0, 8);
    base_height = constrain(base_height, 0, 8);

    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    mx.clear();

//============================================================================
    for (int col = 0; col < NUM_COLUMNS; col++) {
        // Dá um toque dinâmico para as colunas não subirem todas retas em bloco
        int target_height = base_height;
        if (base_height > 0) {
            int variation = random(-1, 2); 
            target_height = constrain(base_height + variation, 0, 8);
        }

        // Suavização (Decay)
        if (target_height > last_heights[col]) {
            last_heights[col] = target_height;
        } else if (last_heights[col] > 0) {
            last_heights[col]--;
        }

        // Desenha a coluna de baixo para cima
        for (int row = 0; row < last_heights[col]; row++) {
            mx.setPoint(7 - row, col, true);
        }
    }
//============================================================================

    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
    delay(15); // Taxa de atualização do display (~60 FPS)
}