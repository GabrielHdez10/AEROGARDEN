#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// Configuración de hardware — AeroGarden
// Aquí van los pines, tiempos y constantes del sistema.
// Los datos sensibles (token, servidor) están en secrets.h
// ============================================================

// ── Sensores ────────────────────────────────────────────────
#define DHTPIN        8      // DHT22: temperatura y humedad del aire
#define DHTTYPE       DHT22
#define TRIG_PIN      53     // HC-SR04: disparo
#define ECHO_PIN      51     // HC-SR04: eco
#define PH_PIN        A0     // Sonda de pH (valor crudo, 0-1023)
#define LUZ_PIN       A1     // LDR (valor crudo, 0-1023)

// ── Actuadores ──────────────────────────────────────────────
#define RELAY_PIN     2      // Bomba de riego

// El módulo de relevador es activo en bajo:
// LOW enciende la bomba, HIGH la apaga.
#define RELAY_ON      LOW
#define RELAY_OFF     HIGH

// ── Muestreo ────────────────────────────────────────────────
#define INTERVALO_DATOS   10000UL  // ms entre envíos al servidor
#define MUESTRAS_ANALOG   10       // lecturas a promediar
#define DELAY_MUESTRA     10       // ms entre cada muestra

// ── Comunicación ────────────────────────────────────────────
#define BAUD_DEBUG        9600     // Serial: monitor USB
#define BAUD_ESP          115200   // Serial1: enlace con el ESP8266
#define TIMEOUT_ESP       8000     // ms de espera por respuesta
#define ESPERA_ARRANQUE   6000     // ms para que el ESP conecte al WiFi

// ── EEPROM ──────────────────────────────────────────────────
// El byte mágico distingue una EEPROM virgen (0xFF) de una
// que ya contiene datos válidos.
#define EEPROM_ADDR        0       // device_id
#define EEPROM_MAGIC_ADDR  10
#define EEPROM_MAGIC       0xAB

// ── Valores por defecto del riego ───────────────────────────
// Se usan hasta que el servidor envía la configuración real.
#define RIEGO_ON_DEFAULT   30      // segundos encendida
#define RIEGO_OFF_DEFAULT  60      // segundos apagada

#endif

