# Firmware AeroGarden

Programas del sistema embebido. Son dos sketches independientes que se
cargan en dos placas distintas.

> Antes de compilar el sketch del Mega, revisa la sección
> "Antes de compilar" del README principal del repositorio.

---

## sketch_hidroponia_MEGA

Corre en el **Arduino Mega 2560**. Lee los sensores, controla el relevador
de la bomba y se comunica con el servidor a través del módulo WiFi.

### Conexiones

| Componente | Pin | Función |
|---|---|---|
| DHT22 | Digital 8 | Temperatura y humedad del aire |
| HC-SR04 (Trig) | Digital 53 | Disparo del ultrasónico |
| HC-SR04 (Echo) | Digital 51 | Lectura del eco |
| Sonda de pH | Analógico A0 | Potencial de hidrógeno |
| LDR | Analógico A1 | Luminosidad |
| Relevador | Digital 2 | Bomba de riego |
| ESP8266 | Serial1 (TX1 18, RX1 19) | Enlace con el módulo WiFi |

El relevador es **activo en bajo**: `LOW` enciende la bomba, `HIGH` la apaga.
En el código se usan las constantes `RELAY_ON` y `RELAY_OFF` de `config.h`.

### Librerías

- `DHT sensor library` (Adafruit)
- `Adafruit Unified Sensor` (dependencia de la anterior)
- `EEPROM` (incluida con el IDE)

### Comportamiento

Cada 10 segundos lee los sensores y envía las lecturas al servidor.
Los valores analógicos se transmiten **crudos** (0 a 1023); la conversión
a pH y a luminosidad se hace del lado del servidor.

Después de cada envío consulta la configuración del relevador, que puede
operar en modo automático (alterna según dos tiempos configurables) o
manual (mantiene el estado que indique el usuario).

### Identificación del dispositivo

El sketch no tiene un identificador fijo. Al arrancar consulta la EEPROM:
si encuentra un byte de validación, recupera el ID guardado; si no, se
empareja con el servidor usando el código definido en `PAIRING_CODE` y
guarda el ID que recibe.

**Antes de cargar el programa hay que editar `PAIRING_CODE`** en
`secrets.h` con el código que genera la interfaz web al registrar el
dispositivo.

### Archivos de configuración

- `config.h` — pines, tiempos y constantes. Sí se versiona.
- `secrets.h` — token, servidor y código de emparejamiento. No se versiona.
- `secrets.example.h` — plantilla del anterior.

---

## sketch_esp8266_PUENTE

Corre en el **ESP8266 (ESP-01)**. Funciona únicamente como puente entre
el puerto serie y la red. No contiene lógica del cultivo.

### Protocolo

Recibe una línea de texto por el serial y la traduce a una petición HTTPS.
Devuelve el cuerpo de la respuesta en una sola línea.

    GET:<ruta>
    POST:<ruta>:<cuerpo JSON>

### Configuración antes de cargar

Editar directamente en el sketch:

- `WIFI_SSID` y `WIFI_PASS` — credenciales de la red
- `SERVER_HOST` — dirección del servidor

> Pendiente: estas credenciales siguen dentro del sketch y deberían
> moverse a un `secrets.h` propio, como se hizo con el del Mega.

---

## Notas de conexión

Dos cosas a considerar al armar el prototipo:

**Niveles lógicos.** El Mega trabaja a 5 V y el ESP8266 a 3.3 V. La línea
TX del Mega hacia el RX del ESP requiere un divisor resistivo o un
convertidor de nivel. Sin eso, el módulo puede fallar de forma
intermitente o dañarse con el tiempo.

**Alimentación.** El ESP-01 demanda picos cercanos a 300 mA al transmitir,
más de lo que entrega