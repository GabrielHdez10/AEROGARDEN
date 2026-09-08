#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <EEPROM.h>
#include "secrets.h"

// ── Prueba unica de la EEPROM ────────────────────────────────
// Pon 1, sube el sketch una vez, y regresalo a 0.
// Ver la seccion "Como comprobarlo" del procedimiento.
#define FORZAR_GUARDADO_PRUEBA 0

// ── Mapa de la EEPROM del ESP8266 ────────────────────────────
// Mismo patron que el Mega: un byte magico distingue una memoria
// virgen (0xFF) de una que ya tiene datos validos.
// El magico se escribe AL FINAL, para que un corte de luz a media
// escritura deje la memoria invalida y no a medias.
#define EEPROM_SIZE     128
#define EE_MAGIC_ADDR   0
#define EE_MAGIC        0xAB
#define EE_SSID_ADDR    1     // 32 bytes
#define EE_SSID_MAX     32
#define EE_PASS_ADDR    34    // 63 bytes
#define EE_PASS_MAX     63

// ── Credenciales activas ─────────────────────────────────────
// Se llenan en setup(): primero se busca en EEPROM, y si no hay
// nada valido se cae a los valores de secrets.h.
char ssidActivo[EE_SSID_MAX + 1];
char passActivo[EE_PASS_MAX + 1];

const char* SERVER_HOST = SERVER_URL;

// ================================================================
// EEPROM
// ================================================================
bool cargarCredencialesEEPROM() {
    if (EEPROM.read(EE_MAGIC_ADDR) != EE_MAGIC) return false;

    for (int i = 0; i < EE_SSID_MAX; i++) ssidActivo[i] = (char)EEPROM.read(EE_SSID_ADDR + i);
    for (int i = 0; i < EE_PASS_MAX; i++) passActivo[i] = (char)EEPROM.read(EE_PASS_ADDR + i);
    ssidActivo[EE_SSID_MAX] = '\0';
    passActivo[EE_PASS_MAX] = '\0';

    // Magico valido pero SSID vacio = dato corrupto
    if (strlen(ssidActivo) == 0) return false;
    return true;
}

void guardarCredencialesEEPROM(const char* ssid, const char* pass) {
    int n = strlen(ssid);
    int m = strlen(pass);

    for (int i = 0; i < EE_SSID_MAX; i++) EEPROM.write(EE_SSID_ADDR + i, i < n ? (uint8_t)ssid[i] : 0);
    for (int i = 0; i < EE_PASS_MAX; i++) EEPROM.write(EE_PASS_ADDR + i, i < m ? (uint8_t)pass[i] : 0);

    EEPROM.write(EE_MAGIC_ADDR, EE_MAGIC);   // el magico, al final
    EEPROM.commit();
}

void usarCredencialesPorDefecto() {
    strncpy(ssidActivo, WIFI_SSID_DEFAULT, EE_SSID_MAX);
    strncpy(passActivo, WIFI_PASS_DEFAULT, EE_PASS_MAX);
    ssidActivo[EE_SSID_MAX] = '\0';
    passActivo[EE_PASS_MAX] = '\0';
}

void setup() {
    // Serial del ESP826
    Serial.begin(115200);
    delay(1000);

    EEPROM.begin(EEPROM_SIZE);

#if FORZAR_GUARDADO_PRUEBA
    guardarCredencialesEEPROM(WIFI_SSID_DEFAULT, WIFI_PASS_DEFAULT);
    Serial.println("PRUEBA: credenciales escritas en EEPROM");
#endif

    if (cargarCredencialesEEPROM()) {
        Serial.println("Credenciales: EEPROM");
    } else {
        usarCredencialesPorDefecto();
        Serial.println("Credenciales: secrets.h");
    }

    Serial.println("=== ESP8266 Puente WiFi ===");
    Serial.print("Conectando a "); Serial.println(ssidActivo);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidActivo, passActivo);

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
        WiFi.begin(ssidActivo, passActivo);
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