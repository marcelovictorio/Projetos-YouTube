💬 Objetivo do Projeto
Reproduzir arquivos de áudio digital com alta fidelidade, mantendo uma arquitetura de código otimizada, modular e não-bloqueante,
baseada no microcontrolador ESP32 e Arduino.

📌 Arquitetura de Hardware
✔️ Unidade de Processamento: ESP32.
✔️ Módulo de Áudio: Amplificador MAX98357A (Classe D). Substituiu o DAC interno inicial para fornecer um áudio potente e limpo através de comunicação digital via protocolo I2S.
✔️ Interface de Acionamento: Botão físico (push-button) conectado ao pino GND e ao GPIO 18, utilizando o resistor de INPUT_PULLUP interno do ESP32 para garantir estabilidade elétrica e evitar falsos disparos por pinos flutuantes.

📌 Arquitetura de Software e Código
✔️ Modularidade: Separação da matriz de dados do áudio em um arquivo de cabeçalho independente (audio.h), mantendo o código principal limpo e focado na lógica de negócio.

✔️ Otimização de Memória: Aplicação do atributo PROGMEM no array de áudio, transferindo o armazenamento estático da RAM para a memória Flash, garantindo estabilidade e evitando travamentos por falta de memória.

✔️ Processamento de Áudio: Integração com a biblioteca arduino-audio-tools. O sistema decodifica o cabeçalho (WAV) de forma automática para identificar a taxa de amostragem correta e isolar os dados brutos (PCM) para o amplificador.

✔️ Execução Assíncrona: Implementação de uma lógica baseada em máquina de estados e stream de dados (via StreamCopy). O áudio é reproduzido em segundo plano durante o loop(), o que significa que o ESP32 não fica "congelado" durante o toque e pode realizar outras operações simultaneamente.

✔️ Tratamento de Exceções: Inclusão de regras de validação no código (!tocando) para impedir que a função seja acionada repetidas vezes e encavale o áudio caso o botão seja pressionado várias vezes em sequência.

Sobre o arquivo de áudio:
🎵 O formato ideal de um arquivo .WAV (Waveform Audio File Format) depende diretamente da aplicação para a qual ele será utilizado. O .WAV não é apenas um formato de áudio, mas um "contêiner" (baseado no padrão RIFF) que pode armazenar áudio em diferentes qualidades. No entanto, na grande maioria dos casos, ele utiliza a codificação PCM (Pulse-Code Modulation) sem compressão.

📌 Preparando o áudio (Via Audacity)

Se você estiver utilizando o áudio em projetos de hardware com memória ou processamento limitados, é necessário reduzir o tamanho do arquivo drasticamente. 
🎵 Preparando o áudio (Via Audacity):
✔️ Codificação: PCM (Não comprimido)
✔️ Taxa de Amostragem: 16 kHz (8 kHz / 11 kHz)
✔️ Profundidade de Bits: 8 bits (ou 16 bits se houver um DAC melhor disponível)
✔️ Canais: 1 (Mono)
✔️ Exporte: Vá em Arquivo > Exportar > Exportar como WAV.

📌 Extraindo o código hexadecimal com o HxD
Agora vamos usar o HxD para transformar esse arquivo de áudio em linhas de código.

✔️ Abra o HxD. 
HxD Copyright© 2002-2021 by Maël Hörz. All rights reserved.
✔️ Arraste e solte o arquivo campainha_pronta.wav para dentro da janela do HxD (ou vá em Arquivo > Abrir). Você verá uma matriz enorme de números e letras.
✔️ No menu superior, clique em Arquivo (File).
✔️ Vá até Exportar (Export) e clique na opção C.
✔️ O HxD pedirá para você salvar um novo arquivo. Salve-o na sua Área de Trabalho com qualquer nome (ex: audio.h).
✔️ Abra o arquivo gerado pelo HxD usando o Bloco de Notas ou um editor de texto.
✔️ Para que o código funcione perfeitamente com a nossa otimização de memória Flash, renomeie a variável rawData para o nome que preferir e adicione o atributo PROGMEM. (otimizada para memória Flash)

<img width="960" height="720" alt="ESP32 Reproduzindo som Array-PCM-WAV com DAC Interno" src="https://github.com/user-attachments/assets/b55cd85b-e3c4-45ee-9b51-5afaca3efaa5" />



