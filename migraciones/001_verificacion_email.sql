-- ============================================================
-- Migración: verificación de correo por token (registro y reset)
-- Ejecutar contra la BD `mydb` ya existente (mysql -u root -p mydb < 001_verificacion_email.sql)
-- ============================================================

USE `mydb`;

-- 1) Marca de correo verificado en usuarios
ALTER TABLE `usuarios`
  ADD COLUMN `email_verified` TINYINT(1) NOT NULL DEFAULT 0 AFTER `contraseña`,
  ADD COLUMN `verified_at` DATETIME NULL AFTER `email_verified`;

-- 2) Tabla de códigos de verificación (registro y restablecimiento de contraseña)
CREATE TABLE IF NOT EXISTS `verificaciones_email` (
  `idVerificacion` INT NOT NULL AUTO_INCREMENT,
  `correo`      VARCHAR(150) NOT NULL,
  `codigo`      VARCHAR(6)   NOT NULL,
  `proposito`   ENUM('registro','reset') NOT NULL DEFAULT 'registro',
  `usado`       TINYINT(1) NOT NULL DEFAULT 0,
  `verificado`  TINYINT(1) NOT NULL DEFAULT 0,
  `creado_en`   DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `expira_en`   DATETIME NOT NULL,
  PRIMARY KEY (`idVerificacion`),
  KEY `idx_correo_proposito` (`correo`, `proposito`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
