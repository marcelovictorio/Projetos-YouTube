Este projeto é um Visualizador de Áudio e Reprodutor Bluetooth Integrado para ESP32. Ele combina a recepção de áudio sem fio via Bluetooth A2DP, a reprodução sonora de alta qualidade em um amplificador digital I2S e a exibição visual do áudio em tempo real através de uma matriz de LEDs.

🛠️ Arquitetura e Componentes Principais
Processamento (ESP32): Gerencia a conexão Bluetooth, decodifica o fluxo de áudio A2DP, processa a amplitude dos canais em tempo real (RMS) e controla os periféricos digitais.
Saída de Áudio (MAX98357A): DAC / Amplificador Classe D conectado via barramento I2S, garantindo reprodução limpa no alto-falante sem os ruídos típicos do DAC analógico interno.
Display Visual (Matriz 8x32 MAX7219): Conjunto de 4 matrizes $8 \times 8$ conectadas via SPI, operando em dois modos distintos:
Modo Standby (Desconectado): Exibe mensagens de status e orientações com texto rolante na tela.
Modo Ativo (Conectado): Transforma-se em um VU Meter de 32 colunas, exibindo a intensidade sonora da música com animação de queda suave dos picos (peak decay).

💻 Destaques do Software
Biblioteca AudioTools.h: Garante compatibilidade total com as atualizações recentes do ESP32 Core (v3.x / ESP-IDF v5), contornando as mudanças estruturais do driver I2S nativo.
Biblioteca BluetoothA2DPSink: Trata o pareamento Bluetooth e o recebimento de áudio via streaming do celular ou computador.
Tratamento Customizado de Display (MD_MAX72XX): Inclui manipulação direta dos dados no buffer SPI com funções de inversão bit a bit (bit-reversal), garantindo a orientação correta das letras e animação suave a 60 FPS no VU Meter.

<img width="960" height="720" alt="ESP32_VUMeteBarrasVerticaisMAX7219" src="https://github.com/user-attachments/assets/aedd01b9-0d77-4646-85bb-a6a36762d74a" />

