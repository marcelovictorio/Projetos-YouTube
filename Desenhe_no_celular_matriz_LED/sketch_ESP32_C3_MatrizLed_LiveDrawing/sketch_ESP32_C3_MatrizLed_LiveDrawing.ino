/*
Código gerado com o auxílio de Inteligência Artificial (IA) e refinado conforme as necessidades do projeto em 07/05/2026.
https://github.com/marcelovictorio @MarceloVictorio YouTube
*/
#include <WiFi.h>
#include <WebServer.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// ===== CONFIG MATRIX =====
//#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

#define MAX_DEVICES 4

#define DATA_PIN 7
#define CLK_PIN  6
#define CS_PIN   10

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// ===== WIFI AP =====
const char* ssid = "ESP32-LED-DRW";
const char* password = "";

WebServer server(80);

// ===== HTML (APP NO NAVEGADOR) =====
const char MAIN_page[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { text-align:center; font-family:sans-serif; }
.grid { display:grid; grid-template-columns: repeat(8,40px); justify-content:center; }
.cell { width:40px; height:40px; border:1px solid #ccc; }
.active { background:black; }
</style>
</head>
<body>
<h2>Desenhe aqui!</h2>
<div class="grid" id="grid"></div>
<button onclick="clearGrid()">Limpar</button>

<script>
let grid = document.getElementById("grid");
let state = [];

for(let i=0;i<64;i++){
  let d=document.createElement("div");
  d.className="cell";
  d.onclick=()=>{
    d.classList.toggle("active");
    send();
  };
  grid.appendChild(d);
}

function send(){
  let data=[];
  document.querySelectorAll(".cell").forEach((c,i)=>{
    data.push(c.classList.contains("active")?1:0);
  });
  fetch("/draw?d="+data.join(""));
}

function clearGrid(){
  document.querySelectorAll(".cell").forEach(c=>c.classList.remove("active"));
  send();
}
</script>
</body>
</html>
)====";

// ===== HANDLERS =====
void handleRoot() {
  server.send_P(200, "text/html", MAIN_page);
}

void handleDraw() {
  String d = server.arg("d");

  mx.clear();

  for (int i = 0; i < d.length(); i++) {
    if (d[i] == '1') {
      int x = i % 8;
      int y = i / 8;
      //mx.setPoint(y, x, true);
      //mx.setPoint(7 - y, 7 - x, true); //(rotação 180°)
      //mx.setPoint(y, 7 - x, true);  // (Espelhado horizontal)
      mx.setPoint(7 - y, x, true);  // (Espelhado vertical)
    }
  }

  server.send(200, "text/plain", "OK");
}

// ===== SETUP =====
void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 5);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/draw", handleDraw);

  server.begin();
}

// ===== LOOP =====
void loop() {
  server.handleClient();
}
