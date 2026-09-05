
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// ── WiFi ─────────────────────────────────────────────────────
const char* WIFI_SSID = "investigacion_itsx";
const char* WIFI_PASS = "itsx-2025";
// ─────────────────────────────────────────────────────────────

const char* SERVER_HOST = "https://web-production-4a25c.up.railway.app";

void setup() {
    // Serial del ESP826
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== ESP8266 Puente WiFi ===");
    Serial.print("Conectando a "); Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 30) {
        delay(500);
        intentos++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi Conectado!");
        Serial.print("IP: "); Serial.println(WiFi.localIP());
    } else {
        Serial.println("ERROR: No se pudo conectar al WiFi");
    }
}

void loop() {
    // Reconectar WiFi si se pierde
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        int intentos = 0;
        while (WiFi.status() != WL_CONNECTED && intentos < 20) {
            delay(500);
            intentos++;
        }
    }

    // Leer comando del Mega
    if (Serial.available()) {
        String linea = Serial.readStringUntil('\n');
        linea.trim();
        if (linea.length() == 0) return;

        String respuesta = procesarComando(linea);
        Serial.println(respuesta);
    }
}

// ================================================================
// Procesar comando recibido del Mega
// ================================================================
String procesarComando(String cmd) {
    if (WiFi.status() != WL_CONNECTED) {
        return "{\"error\":\"sin_wifi\"}";
    }

    String metodo   = "";
    String endpoint = "";
    String body     = "";

    int sep1 = cmd.indexOf(':');
    if (sep1 == -1) return "{\"error\":\"cmd_invalido\"}";

    metodo = cmd.substring(0, sep1);

    if (metodo == "POST") {
        int sep2 = cmd.indexOf(':', sep1 + 1);
        if (sep2 == -1) return "{\"error\":\"cmd_invalido\"}";
        endpoint = cmd.substring(sep1 + 1, sep2);
        body     = cmd.substring(sep2 + 1);
    } else if (metodo == "GET") {
        endpoint = cmd.substring(sep1 + 1);
    } else {
        return "{\"error\":\"metodo_invalido\"}";
    }

    String url = String(SERVER_HOST) + endpoint;

    // Cliente HTTPS sin verificar certificado (Railway usa certificado válido)
    std::unique_ptr<BearSSL::WiFiClientSecure> clienteSSL(new BearSSL::WiFiClientSecure);
    clienteSSL->setInsecure(); // No verifica certificado SSL

    HTTPClient http;
    http.begin(*clienteSSL, url);
    http.setTimeout(8000);

    int httpCode = -1;
    String payload = "";

    if (metodo == "POST") {
        http.addHeader("Content-Type", "application/json");
        httpCode = http.POST(body);
    } else {
        httpCode = http.GET();
    }

    if (httpCode > 0) {
        payload = http.getString();
        payload.replace("\n", "");
        payload.replace("\r", "");
    } else {
        payload = "{\"error\":\"http_" + String(httpCode) + "\"}";
    }

    http.end();
    return payload;
}
