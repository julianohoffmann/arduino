#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "Radar_WIFI";
const char* password = "12345678";

ESP8266WebServer servidor(80);

String angle = "0";
String distance = "0";

void setup() {
  Serial.begin(9600); // Comunicação com Arduino UNO
  WiFi.softAP(ssid, password);
  servidor.on("/", Pagina_Requisitada);
  servidor.begin();
}

void loop() {
  servidor.handleClient();

  // Lê dados do Arduino
  if (Serial.available()) {
    String dados = Serial.readStringUntil('.');
    if (dados.indexOf(',') > 0) {
      angle = dados.substring(0, dados.indexOf(','));
      distance = dados.substring(dados.indexOf(',') + 1);
    }
  }
}

void Pagina_Requisitada() {
  servidor.send(200, "text/html", Monta_HTML());
}

String Monta_HTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='0.5'>";
  html += "<title>Radar Ultrassônico</title>";
  html += "<style>";
  html += "body{background:#009541;color:white;text-align:center;font-family:Arial;margin-top:30px;}";
  html += "h1{font-size:28px;}p{font-size:22px;}";
  html += "</style></head><body>";
  html += "<h1>🤖 Radar Ultrassônico</h1>";
  html += "<p><b>Ângulo:</b> " + angle + "°</p>";
  html += "<p><b>Distância:</b> " + distance + " cm</p>";
  html += "<p>Rede: <b>" + String(ssid) + "</b></p>";
  html += "</body></html>";
  return html;
}
