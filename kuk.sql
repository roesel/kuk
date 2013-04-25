-- phpMyAdmin SQL Dump
-- version 3.5.2.2
-- http://www.phpmyadmin.net
--
-- Počítač: 127.0.0.1
-- Vygenerováno: Stř 24. dub 2013, 23:42
-- Verze MySQL: 5.5.27
-- Verze PHP: 5.4.7

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Databáze: `kuk`
--

-- --------------------------------------------------------

--
-- Struktura tabulky `speed`
--

CREATE TABLE IF NOT EXISTS `speed` (
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `speed` float NOT NULL DEFAULT '0',
  KEY `time` (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
