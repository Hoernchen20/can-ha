-- phpMyAdmin SQL Dump
-- version 3.4.11.1deb2+deb7u1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Erstellungszeit: 25. Apr 2015 um 10:26
-- Server Version: 5.5.43
-- PHP-Version: 5.4.39-0+deb7u2

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Datenbank: `raspi_control`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `meldung`
--

CREATE TABLE IF NOT EXISTS `meldung` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `can_id` mediumint(8) unsigned DEFAULT NULL,
  `tw1` varchar(50) NOT NULL,
  `tw2` varchar(50) NOT NULL,
  `tw3` varchar(50) DEFAULT NULL,
  `dm` bit(1) NOT NULL,
  `prioritaet` tinyint(3) unsigned NOT NULL,
  `status_00` varchar(40) NOT NULL,
  `status_01` varchar(40) NOT NULL,
  `status_10` varchar(40) NOT NULL DEFAULT '',
  `status_11` varchar(40) NOT NULL DEFAULT '',
  `akt_status` tinyint(3) unsigned NOT NULL,
  `akt_status_zeitstempel` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `can_id` (`can_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `meldung_archiv`
--

CREATE TABLE IF NOT EXISTS `meldung_archiv` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `meldung_id` mediumint(8) unsigned NOT NULL,
  `status` tinyint(3) unsigned NOT NULL,
  `info` varchar(100) DEFAULT NULL,
  `zeitstempel` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `meldung_id` (`meldung_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `messwert`
--

CREATE TABLE IF NOT EXISTS `messwert` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `can_id` mediumint(8) unsigned DEFAULT NULL,
  `can_laenge` tinyint(3) unsigned NOT NULL,
  `tw1` varchar(50) NOT NULL,
  `tw2` varchar(50) NOT NULL,
  `tw3` varchar(50) DEFAULT NULL,
  `einheit` varchar(10) NOT NULL,
  `grenze_unten` decimal(10,3) NOT NULL,
  `warnung_unten` decimal(10,3) NOT NULL,
  `warnung_oben` decimal(10,3) NOT NULL,
  `grenze_oben` decimal(10,3) NOT NULL,
  `grenze_meldung_id` mediumint(8) unsigned NOT NULL,
  `warnung_meldung_id` mediumint(8) unsigned NOT NULL,
  `akt_messwert` decimal(10,3) NOT NULL,
  `akt_messwert_zeitstempel` datetime NOT NULL,
  `archiv_level` tinyint(3) unsigned NOT NULL DEFAULT '4' COMMENT '0: keine Archivierung; 1: 1min; 2: 15min; 3: 1h; 4: 1d',
  PRIMARY KEY (`id`),
  UNIQUE KEY `can_id` (`can_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `messwert_archiv`
--

CREATE TABLE IF NOT EXISTS `messwert_archiv` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `messwert_id` mediumint(6) unsigned NOT NULL,
  `wert` decimal(10,3) NOT NULL,
  `zeitstempel` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `messwert_id` (`messwert_id`),
  KEY `zeitstempel` (`zeitstempel`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `messwert_archiv_1d`
--

CREATE TABLE IF NOT EXISTS `messwert_archiv_1d` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `messwert_id` mediumint(6) unsigned NOT NULL,
  `min` decimal(10,3) NOT NULL,
  `max` decimal(10,3) NOT NULL,
  `avg` decimal(10,3) NOT NULL,
  `datum` date NOT NULL,
  PRIMARY KEY (`id`),
  KEY `messwert_id` (`messwert_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `messwert_archiv_1h`
--

CREATE TABLE IF NOT EXISTS `messwert_archiv_1h` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `messwert_id` mediumint(6) unsigned NOT NULL,
  `min` decimal(10,3) NOT NULL,
  `max` decimal(10,3) NOT NULL,
  `avg` decimal(10,3) NOT NULL,
  `zeitstempel` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `messwert_id` (`messwert_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `messwert_archiv_15m`
--

CREATE TABLE IF NOT EXISTS `messwert_archiv_15m` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `messwert_id` mediumint(8) unsigned NOT NULL,
  `min` decimal(10,3) NOT NULL,
  `max` decimal(10,3) NOT NULL,
  `avg` decimal(10,3) NOT NULL,
  `zeitstempel` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `messwert_id` (`messwert_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `sollwert`
--

CREATE TABLE IF NOT EXISTS `sollwert` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `can_id` mediumint(8) unsigned DEFAULT NULL,
  `can_laenge` tinyint(3) unsigned NOT NULL,
  `tw1` varchar(50) NOT NULL,
  `tw2` varchar(50) NOT NULL,
  `tw3` varchar(50) DEFAULT NULL,
  `einheit` varchar(10) NOT NULL,
  `grenze_unten` decimal(10,3) NOT NULL,
  `warnung_unten` decimal(10,3) NOT NULL,
  `warnung_oben` decimal(10,3) NOT NULL,
  `grenze_oben` decimal(10,3) NOT NULL,
  `akt_sollwert` decimal(10,3) NOT NULL,
  `akt_sollwert_zeitstempel` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `can_id` (`can_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `tabellenverwendung`
--

CREATE TABLE IF NOT EXISTS `tabellenverwendung` (
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `tabellenname` varchar(100) NOT NULL,
  `verwendung` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

--
-- Constraints der exportierten Tabellen
--

--
-- Constraints der Tabelle `meldung_archiv`
--
ALTER TABLE `meldung_archiv`
  ADD CONSTRAINT `meldung_archiv_ibfk_1` FOREIGN KEY (`meldung_id`) REFERENCES `meldung` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE;

--
-- Constraints der Tabelle `messwert_archiv`
--
ALTER TABLE `messwert_archiv`
  ADD CONSTRAINT `messwert_archiv_ibfk_1` FOREIGN KEY (`messwert_id`) REFERENCES `messwert` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE;

--
-- Constraints der Tabelle `messwert_archiv_1d`
--
ALTER TABLE `messwert_archiv_1d`
  ADD CONSTRAINT `messwert_archiv_1d_ibfk_1` FOREIGN KEY (`messwert_id`) REFERENCES `messwert` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE;

--
-- Constraints der Tabelle `messwert_archiv_1h`
--
ALTER TABLE `messwert_archiv_1h`
  ADD CONSTRAINT `messwert_archiv_1h_ibfk_1` FOREIGN KEY (`messwert_id`) REFERENCES `messwert` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE;

--
-- Constraints der Tabelle `messwert_archiv_15m`
--
ALTER TABLE `messwert_archiv_15m`
  ADD CONSTRAINT `messwert_archiv_15m_ibfk_1` FOREIGN KEY (`messwert_id`) REFERENCES `messwert` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
