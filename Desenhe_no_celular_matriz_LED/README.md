Desenhe no celular e veja aparecer na matriz LED em TEMPO REAL 😱 
#esp32 #wireless #arduino #iot

Descrição:
🚀 Desenhe no celular e veja aparecer na matriz LED em TEMPO REAL (ESP32 + Wi-Fi, sem app!)

Neste projeto eu usei um ESP32-C3 para criar um “mini painel inteligente” que você controla direto do navegador.
Sem instalar nada, sem Bluetooth — só conectar no Wi-Fi e começar a desenhar.

👉 Ideal para quem curte IoT, Arduino, automação ou quer criar projetos visuais que chamam atenção.

🧠 COMO FUNCIONA (resumo rápido)
O ESP32 cria uma rede Wi-Fi própria (modo AP) e roda um servidor web interno.
Quando você desenha no celular, os dados são enviados em tempo real e exibidos na matriz LED.

🛠️ COMPONENTES

* ESP32-C3 (SuperMini ou DevKit)
* Módulo matriz LED MAX7219 (8x8 ou 32x8)
* Jumpers
* Fonte 5V (ou USB)

🔌 LIGAÇÕES (ESP32-C3 → MAX7219)

* VCC → 5V
* GND → GND
* DIN → GPIO 7
* CS → GPIO 10
* CLK → GPIO 6

💻 PASSO A PASSO

1️⃣ Instale as bibliotecas na Arduino IDE:
* MD_MAX72XX
* MD_Parola

2️⃣ Copie e grave o código no ESP32-C3

3️⃣ No celular, conecte no Wi-Fi:
👉 ESP32-LED

4️⃣ Abra o navegador e acesse: o endereço IP conforme sua rede.

5️⃣ Desenhe na tela e veja aparecer na matriz em tempo real 🤯

#ESP32 #Arduino #IoT #Eletrônica #Programação #DIY #Maker #Automação #Tecnologia #LEDMatrix
