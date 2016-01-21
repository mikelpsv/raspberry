CREATE SCHEMA `smarthome`;
ALTER SCHEMA `smarthome`  DEFAULT CHARACTER SET utf8  DEFAULT COLLATE utf8_general_ci ;

CREATE TABLE `smarthome`.`buildings` (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '',
  `name` VARCHAR(45) NULL COMMENT '',
  `caption` VARCHAR(45) NULL COMMENT '',
  PRIMARY KEY (`id`)  COMMENT '');

INSERT INTO `smarthome`.`buildings` (`name`, `caption`) VALUES ('MyHouse', 'Мой дом');
INSERT INTO `smarthome`.`buildings` (`name`, `caption`) VALUES ('Оutdoors', 'На улице');

CREATE TABLE `smarthome`.`rooms` (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '',
  `name` VARCHAR(45) NULL COMMENT '',
  `caption` VARCHAR(45) NULL COMMENT '',
  `building_id` INT NULL COMMENT '',
  PRIMARY KEY (`id`)  COMMENT '');

CREATE TABLE `smarthome`.`devices` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) DEFAULT NULL,
  `caption` varchar(45) DEFAULT NULL,
  `room_id` int(11) DEFAULT NULL,
  `disabled` tinyint(4) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `bus` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
);

CREATE TABLE `smarthome`.`sensor_data` (
  `date` DATETIME NOT NULL COMMENT '',
  `device_id` INT NOT NULL COMMENT '',
  `value` DECIMAL NULL COMMENT '');

