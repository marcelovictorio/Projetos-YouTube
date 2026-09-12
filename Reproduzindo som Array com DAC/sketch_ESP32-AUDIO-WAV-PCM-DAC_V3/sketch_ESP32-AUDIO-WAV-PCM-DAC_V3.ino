/*
Código gerado com o auxílio de Inteligência Artificial (IA) e refinado conforme as necessidades do projeto em 11/09/2026.
https://github.com/marcelovictorio @MarceloVictorio YouTube
*/
#include <Arduino.h>
#include "AudioTools.h"   // Importa a nova biblioteca
#include "audio.h"        // O seu array de áudio
//#include "EssaFoiFacil.h" //

// ==========================================
// CONFIGURAÇÃO DOS PINOS (ESP32-C3 -> MAX98357A) ALTERAR PARA OUTROS MÓDULOS
// ==========================================
#define I2S_BCLK 4   // Bit Clock
#define I2S_LRC  5   // Left/Right Clock (Word Select)
#define I2S_DOUT 6   // Data Out
#define PINO_BOTAO 3 // Pino do botão da campainha

// ==========================================
// OBJETOS DO AUDIO TOOLS
// ==========================================
I2SStream i2s;                                          // Controle de saída de áudio físico
VolumeStream volume(i2s);                               // Controle de volume interceptando o I2S
MemoryStream memoriaAudio(SomWav, tamanhoSom);          // Transforma sua matriz em um "fluxo de arquivo"
WAVDecoder decodificador;                               // Decodifica o cabeçalho do arquivo WAV
EncodedAudioStream saidaAudio(&volume, &decodificador); // Junta a saída do VOLUME com o decodificador
StreamCopy copiador(saidaAudio, memoriaAudio);          // Motor que copia o som da memória para a saída

bool tocando = false; // Controle de estado do PlayAudio

void setup() {
  Serial.begin(115200);
  
  // (Opcional) Habilita logs da biblioteca para ajudar a debugar se algo der errado
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);

  pinMode(PINO_BOTAO, INPUT_PULLUP);

  // Configuração do I2S via AudioTools
  auto config = i2s.defaultConfig(TX_MODE);
  config.pin_bck = I2S_BCLK;
  config.pin_ws = I2S_LRC;
  config.pin_data = I2S_DOUT;
  
  // Inicia a saída I2S
  i2s.begin(config);

  // Inicia e aplica o ganho de volume (1.0 = volume original, 2.0 = dobro)
  auto config_vol = volume.defaultConfig();
  volume.begin(config_vol);
  volume.setVolume(2.0); // Ajuste o nível do som aqui
  
  // Inicia o serviço de saída (a taxa de amostragem será definida sozinha pelo WAVDecoder)
  saidaAudio.begin();
  
  Serial.println("Sistema de Som (AudioTools) Iniciado. Aguardando...");
}

void PlayAudio() {
  Serial.println("Tocando som...");
  
  // Reinicia o fluxo de leitura de memória para o começo do arquivo (posição 0)
  memoriaAudio.begin(); 
  //saidaAudio.begin();  // Resolve estalo no inicio do som
  decodificador.begin(); // Avisar o decodificador para processar o cabeçalho novamente.
  
  tocando = true; // Avisa o loop para começar a tocar
}

void loop() {
  // 1. VERIFICA O BOTÃO
  // Só aciona se o botão for pressionado e não houver som no momento
  if (digitalRead(PINO_BOTAO) == HIGH && !tocando) {
    PlayAudio();
    delay(300); // Pequeno atraso para debounce
  }

  // 2. REPRODUZ O ÁUDIO (De forma não-bloqueante)
  if (tocando) {
    if (copiador.copy() == 0) {
      tocando = false; 
      Serial.println("Reprodução concluída.");
    }
  } else {
    // Mantém o clock do I2S ativo enviando silêncio absoluto
    uint8_t silencio[4] = {0, 0, 0, 0};
    i2s.write(silencio, 4);
  }


}