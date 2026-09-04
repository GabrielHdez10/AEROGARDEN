# AeroGarden — Sistema de Monitoreo IoT para Cultivos Aeropónicos

Sistema web de monitoreo en tiempo real para cultivos aeropónicos/hidropónicos a escala residencial. Desarrollado como proyecto de Residencia Profesional.

Permite visualizar lecturas de sensores (temperatura, humedad, pH, luz, nivel de agua), configurar alertas, controlar el relevador de riego y gestionar cultivos desde cualquier navegador.

---

## Tecnologías utilizadas

- **Backend:** Python + Flask
- **Base de datos:** MySQL
- **Frontend:** HTML, CSS, JavaScript
- **Hardware:** Arduino Mega + ESP-01

---

## Requisitos previos

- Python 3.8 o superior
- XAMPP (o cualquier servidor MySQL local)
- pip

---

## Instalación local

**1. Descomprimir el proyecto**
Descomprímelo en una carpeta de tu elección.

**2. Instalar dependencias**
```bash
pip install -r requirements.txt
```

**3. Configurar variables de entorno**
Copia `env.example` y renómbralo a `.env`, luego llena tus datos de MySQL:

```
SECRET_KEY=cualquier_frase_larga_y_aleatoria
FLASK_DEBUG=False
DB_HOST=127.0.0.1
DB_PORT=3306
DB_USER=root
DB_PASS=tu_contraseña_mysql
DB_NAME=mydb
ARDUINO_TOKEN=token_secreto_para_arduino
```

**4. Crear la base de datos**
- Abre XAMPP y arranca MySQL
- Abre phpMyAdmin en `http://localhost/phpmyadmin`
- Crea una base de datos llamada `mydb`
- Importa el archivo `schema.sql` incluido en el proyecto

**5. Correr el proyecto**
```bash
python app.py
```
Abre tu navegador en `http://localhost:5000`

---

## Estructura del proyecto

```
aerogarden/
├── app.py                  # Backend principal (Flask)
├── requirements.txt        # Dependencias de Python
├── env.example             # Plantilla de variables de entorno
├── schema.sql              # Estructura de la base de datos
├── static/
│   ├── css/
│   │   └── estilos.css     # Estilos del dashboard
│   ├── js/
│   │   ├── dashboard.js    # Lógica del dashboard
│   │   ├── login.js        # Lógica del login
│   │   ├── registro.js     # Lógica del registro
│   │   └── i18n.js         # Soporte de idiomas
│   └── img/                # Imágenes y logos
└── templates/
    ├── dashboard.html      # Página principal
    ├── login.html          # Página de login
    └── registro.html       # Página de registro
```
## Antes de compilar

El sketch del Mega requiere un archivo `secrets.h` que no está en el
repositorio, porque contiene credenciales.

Para crearlo, copia la plantilla dentro de `sketch_hidroponia_MEGA/`:

    Copy-Item secrets.example.h secrets.h

Y llena los tres valores:

- `PAIRING_CODE` — lo genera la interfaz web al registrar un dispositivo
- `ARDUINO_TOKEN` — debe coincidir con el del archivo `.env` del backend
- `SERVER_HOST` — dominio del servidor, sin `https://` al inicio

Sin este archivo el sketch no compila.

La configuración de pines y tiempos está en `config.h`, que sí forma
parte del repositorio.