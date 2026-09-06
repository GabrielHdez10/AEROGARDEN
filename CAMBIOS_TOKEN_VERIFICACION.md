# Implementación de verificación por token — AeroGarden

Base: `main` de https://github.com/GabrielHdez10/AEROGARDEN al momento de
generar esto (incluye ya el merge de `firmware/separar-configuracion` y
`fix/cargar-variables-entorno` — python-dotenv y separación de credenciales
del firmware de tu amigo). La rama `firmware/agregar-sketches` es un cabo
suelto: git confirma que ya es ancestro de `main`, no aporta nada nuevo, se
puede borrar sin perder trabajo.

Archivos tocados (solo estos 4, nada más del árbol se modificó):
- `app.py`
- `env.example`
- `templates/login.html`
- `templates/registro.html`

## 🔴 Hallazgo crítico (ya corregido en este entregable)
`/api/auth/restablecer-password` cambiaba la contraseña de **cualquier cuenta**
con solo mandar el correo, sin validar nada más. Cualquiera podía tomar
cualquier cuenta sabiendo el email. Ahora exige un código verificado.

## Qué se agregó

1. **`migraciones/001_verificacion_email.sql`** (nuevo, correr aparte contra tu BD)
   - `usuarios.email_verified` (TINYINT) y `usuarios.verified_at`
   - Tabla `verificaciones_email` (correo, codigo, proposito ENUM('registro','reset'), usado, verificado, expira_en)

2. **`app.py`**
   - `generar_codigo()`, `enviar_correo_codigo()`, `crear_codigo_verificacion()`
   - `registrar_usuario`: ya no deja al usuario logueado directo; crea el
     código, lo envía, y responde `requiere_verificacion: true`
   - `POST /api/auth/enviar-codigo` — reenvía código (registro o reset)
   - `POST /api/auth/verificar-codigo` — valida el código:
     - `proposito=registro` → marca `email_verified=1` y autologuea al usuario
     - `proposito=reset` → solo marca el código como verificado, no lo consume
   - `verificar-correo` ahora dispara el envío del código de reset (antes solo
     confirmaba que el correo existía)
   - `restablecer-password` ahora exige `correo + codigo` verificado, no
     expirado y no usado antes de tocar la contraseña
   - `login`: si `email_verified=0`, responde 403 con `requiere_verificacion: true`
     en vez de dejar entrar
   - Se conservó intacto todo lo de tu amigo: `from dotenv import load_dotenv`
     y `load_dotenv()` siguen ahí tal cual estaban en `main`

3. **Frontend** (`templates/registro.html`, `templates/login.html`)
   - Registro: paso 2 con input de código de 6 dígitos + botón "Reenviar código"
   - Login: si el correo no está verificado, redirige automáticamente al
     flujo de verificación y reenvía el código
   - Recuperar contraseña: ahora son 3 pasos — correo → código → nueva contraseña

4. **`env.example`** — variables `EMAIL_HOST`, `EMAIL_PORT`, `EMAIL_USE_TLS`,
   `EMAIL_HOST_USER`, `EMAIL_HOST_PASSWORD`, `DEFAULT_FROM_EMAIL`,
   `CODIGO_EXP_MINUTOS`. Pensado para un SMTP dedicado (Brevo, SendGrid, etc.),
   no un Gmail personal. `requirements.txt` no cambió: `python-dotenv` ya
   estaba ahí por el trabajo previo de tu amigo.

## Pendiente de tu lado
- Correr la migración SQL contra tu base local/servidor.
- Crear una cuenta en Brevo (u otro SMTP transaccional) y llenar las
  variables de entorno reales en tu `.env` (no subir ese archivo).
- Avisarle a tu amigo / hacer pull antes de aplicar esto, por si él tiene
  cambios locales sin subir todavía en su copia de `app.py`.
- Decidir si quieren rate limiting en los reintentos de código — no lo
  agregué para no meter dependencias nuevas sin que lo vean primero.
- Se puede borrar la rama `firmware/agregar-sketches` en GitHub cuando
  quieran; no tiene nada que no esté ya en `main`.
