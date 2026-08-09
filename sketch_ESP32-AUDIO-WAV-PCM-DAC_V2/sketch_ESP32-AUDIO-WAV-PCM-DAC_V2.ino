/*
Código gerado com o auxílio de Inteligência Artificial (IA) e refinado conforme as necessidades do projeto em 08/08/2026.
https://github.com/marcelovictorio @MarceloVictorio YouTube
*/

// ==========================================
// CONFIGURAÇÃO DOS PINOS (MAX98357A)
// ==========================================
#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 22
#define PINO_BOTAO 18

// ==========================================
// OBJETOS DO AUDIO TOOLS
// ==========================================
I2SStream i2s;                                       // Controle de saída de áudio para o módulo
MemoryStream memoriaAudio(SomWav, tamanhoSom);       // Transforma sua matriz em um "fluxo de arquivo"
WAVDecoder decodificador;                            // Decodifica o cabeçalho do arquivo WAV
EncodedAudioStream saidaAudio(&i2s, &decodificador); // Junta a saída I2S com o decodificador
StreamCopy copiador(saidaAudio, memoriaAudio);       // Motor que copia o som da memória para a saída

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
  
  // Inicia os serviços (a taxa de amostragem será definida sozinha pelo WAVDecoder)
  i2s.begin(config);
  saidaAudio.begin();
  
  Serial.println("Sistema da Campainha (AudioTools) Iniciado. Aguardando...");
}

void PlayAudio() {
  Serial.println("Tocando a campainha...");
  
  // Reinicia o fluxo de leitura de memória para o começo do arquivo (posição 0)
  memoriaAudio.begin(); 
  saidaAudio.begin(); 
  
  tocando = true; // Avisa o loop para começar a tocar
}

void loop() {
  // 1. VERIFICA O BOTÃO
  // Só aciona se o botão for pressionado E a campainha não estiver tocando no momento
  if (digitalRead(PINO_BOTAO) == HIGH && !tocando) {
    PlayAudio();
    delay(300); // Pequeno atraso para debounce
  }

  // 2. REPRODUZ O ÁUDIO (De forma não-bloqueante)
  if (tocando) {
    // O copiador passa pequenos pacotes de áudio da memória para o MAX98357A a cada ciclo do loop
    // Se o copy() retornar 0, significa que o arquivo chegou ao fim.
    if (copiador.copy() == 0) {
      tocando = false; // Áudio terminou, libera para o botão ser apertado de novo
      Serial.println("Reprodução concluída.");
    }
  }
}