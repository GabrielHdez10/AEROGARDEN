# certs/

Aquí va tu certificado CA para conectarte a la base de datos en la nube
(Aiven, PlanetScale, etc.) por SSL.

**No se sube a GitHub** (ver `.gitignore`) — cada quien descarga el suyo.

## Cómo obtenerlo (Aiven)
1. Entra a tu proyecto en https://console.aiven.io
2. Ve a tu servicio de MySQL → pestaña "Overview"
3. Descarga el "CA Certificate"
4. Guárdalo aquí mismo como: `certs/aiven-ca.pem`

Con el archivo en esta ruta, `app.py` lo detecta automáticamente — no
necesitas tocar tu `.env` para nada relacionado al certificado.

Si prefieres guardarlo en otro lugar, puedes forzar la ruta con la
variable `DB_SSL_CA` en tu `.env`.
