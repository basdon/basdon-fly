-- phpMyAdmin SQL Dump
-- version 4.0.0
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Dec 25, 2019 at 10:54 PM
-- Server version: 5.5.16
-- PHP Version: 7.2.6

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `basdonfly`
--

-- --------------------------------------------------------

--
-- Table structure for table `acl`
--

CREATE TABLE IF NOT EXISTS `acl` (
  `t` int(11) NOT NULL,
  `j` char(15) NOT NULL,
  `u` int(11) DEFAULT NULL,
  `l` int(11) NOT NULL,
  `type` int(11) NOT NULL DEFAULT '0',
  `e` varchar(144) NOT NULL,
  KEY `u` (`u`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `acl`
--

-- --------------------------------------------------------

--
-- Table structure for table `apt`
--

CREATE TABLE IF NOT EXISTS `apt` (
  `i` int(11) NOT NULL,
  `c` char(4) NOT NULL,
  `e` int(11) NOT NULL,
  `n` char(24) NOT NULL,
  `b` char(4) NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `flags` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`i`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `apt`
--

INSERT INTO `apt` (`i`, `c`, `e`, `n`, `b`, `x`, `y`, `z`, `flags`) VALUES
(0, 'XXXX', 1, 'Area 69', 'BASE', 305.829, 1926.86, 17.6406, 1),
(1, 'SALV', 1, 'Las Venturas Airport', 'LV', 1432.75, 1463.36, 10.8203, 1),
(2, 'SAVM', 1, 'Verdant Meadows Airfield', 'VMA', 177.575, 2503.03, 16.4922, 1),
(3, 'SASF', 1, 'San Fierro Airport', 'SF', -1275.71, 53.5765, 89.3318, 1),
(4, 'SALS', 1, 'Los Santos Airport', 'LS', 1744.22, -2542.66, 13.5469, 1),
(5, 'SFCA', 1, 'San Fierro Carrier', 'SFCA', -1335.59, 493.757, 61.2869, 1),
(6, 'LSIF', 1, 'Los Santos Inlet Field', 'LSIF', -239.506, -2167.82, 29.271, 0),
(7, 'CALI', 1, 'Caligula''s Palace', 'CALI', 2239.42, 1562.17, 69.5118, 0);

-- --------------------------------------------------------

--
-- Table structure for table `art`
--

CREATE TABLE IF NOT EXISTS `art` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cat` int(11) DEFAULT NULL,
  `name` varchar(64) NOT NULL,
  `title` varchar(64) NOT NULL,
  `pageviews` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `cat` (`cat`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `art`
--

INSERT INTO `art` (`id`, `cat`, `name`, `title`, `pageviews`) VALUES
(1, NULL, 'Main_Page', 'Main Page', 0),
(2, 1, 'List_of_aircraft', 'List of aircraft', 0),
(3, 1, 'Fuel', 'Fuel', 0),
(4, 0, 'Los_Santos_Airport', 'Los Santos Airport', 0),
(5, 3, 'Map_File_Format', 'Map file format', 0),
(6, 4, 'Echo', 'Echo', 0),
(7, 0, 'Las_Venturas_Airport', 'Las Venturas Airport', 0),
(8, 0, 'San_Fierro_Airport', 'San Fierro Airport', 0),
(9, 0, 'Area_69', 'Area 69', 0),
(10, 0, 'Verdant_Meadows', 'Verdant Meadows', 0),
(11, 1, 'How_To_Join', 'How To Join', 0),
(12, 4, 'Flighttracker', 'Flighttracker', 0),
(13, 3, 'Flight_File_Format', 'Flight file format', 0),
(14, 1, 'Keys', 'Keys', 0),
(15, 1, 'Service_points', 'Service points', 0),
(16, 5, 'Preprocessor_Definitions', 'Preprocessor Definitions', 0),
(17, 0, 'Caligulas_Palace', 'Caligula''s Palace', 0),
(18, 0, 'Los_Santos_Inlet_Field', 'Los Santos Inlet Field', 0),
(19, 2, 'Tables', 'Tables', 0);

-- --------------------------------------------------------

--
-- Table structure for table `artalt`
--

CREATE TABLE IF NOT EXISTS `artalt` (
  `art` int(11) NOT NULL,
  `alt` varchar(64) NOT NULL,
  PRIMARY KEY (`art`,`alt`),
  KEY `art` (`art`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `artalt`
--

INSERT INTO `artalt` (`art`, `alt`) VALUES
(4, 'LSAP'),
(4, 'SALS'),
(7, 'SALV'),
(8, 'SASF');

-- --------------------------------------------------------

--
-- Table structure for table `artcat`
--

CREATE TABLE IF NOT EXISTS `artcat` (
  `id` int(11) NOT NULL,
  `parent` int(11) DEFAULT NULL,
  `name` char(16) NOT NULL,
  `color` char(6) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `parent` (`parent`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `artcat`
--

INSERT INTO `artcat` (`id`, `parent`, `name`, `color`) VALUES
(0, NULL, 'Airports', 'ccccff'),
(1, NULL, 'Basics', 'ffccff'),
(2, NULL, 'Internal docs', 'c0c0c0'),
(3, 2, 'File formats', 'd8d8d8'),
(4, 2, 'Services', 'd8d8d8'),
(5, 2, 'Code', 'd8d8d8');

-- --------------------------------------------------------

--
-- Table structure for table `cmdlog`
--

CREATE TABLE IF NOT EXISTS `cmdlog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `player` int(11) DEFAULT NULL,
  `loggedstatus` int(11) NOT NULL,
  `stamp` int(10) unsigned NOT NULL,
  `cmd` varchar(128) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `player` (`player`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `cmdlog`
--

-- --------------------------------------------------------

--
-- Table structure for table `deathlog`
--

CREATE TABLE IF NOT EXISTS `deathlog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `stamp` int(10) unsigned NOT NULL,
  `killee` int(11) DEFAULT NULL,
  `killer` int(11) DEFAULT NULL,
  `reason` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `killee` (`killee`),
  KEY `killer` (`killer`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `deathlog`
--

-- --------------------------------------------------------

--
-- Table structure for table `fal`
--

CREATE TABLE IF NOT EXISTS `fal` (
  `u` int(11) DEFAULT NULL,
  `stamp` int(11) NOT NULL,
  `ip` char(45) NOT NULL,
  `isweb` int(11) NOT NULL DEFAULT '0',
  KEY `u` (`u`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `fal`
--

-- --------------------------------------------------------

--
-- Table structure for table `flg`
--

CREATE TABLE IF NOT EXISTS `flg` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `player` int(11) DEFAULT NULL,
  `vehicle` int(11) NOT NULL,
  `missiontype` int(11) NOT NULL,
  `fapt` int(11) NOT NULL,
  `tapt` int(11) NOT NULL,
  `fmsp` int(11) NOT NULL,
  `tmsp` int(11) NOT NULL,
  `distance` float NOT NULL,
  `adistance` float NOT NULL DEFAULT '0',
  `state` int(11) NOT NULL DEFAULT '1',
  `tstart` int(11) NOT NULL,
  `tlastupdate` int(11) NOT NULL,
  `tload` int(11) NOT NULL DEFAULT '0',
  `tunload` int(11) NOT NULL DEFAULT '0',
  `satisfaction` int(11) NOT NULL DEFAULT '100',
  `ptax` int(11) NOT NULL DEFAULT '0',
  `pweatherbonus` int(11) NOT NULL DEFAULT '0',
  `psatisfaction` int(11) NOT NULL DEFAULT '0',
  `pdistance` int(11) NOT NULL DEFAULT '0',
  `paymp` float NOT NULL DEFAULT '0',
  `pdamage` int(11) NOT NULL DEFAULT '0',
  `pcheat` int(11) NOT NULL DEFAULT '0',
  `pbonus` int(11) NOT NULL DEFAULT '0',
  `ptotal` int(11) NOT NULL DEFAULT '0',
  `fuel` float NOT NULL DEFAULT '0',
  `damage` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user` (`player`,`vehicle`,`fapt`,`tapt`,`fmsp`,`tmsp`),
  KEY `fapt` (`fapt`),
  KEY `tapt` (`tapt`),
  KEY `fmsp` (`fmsp`),
  KEY `tmsp` (`tmsp`),
  KEY `vehicle` (`vehicle`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `flg`
--

-- --------------------------------------------------------

--
-- Table structure for table `gpci`
--

CREATE TABLE IF NOT EXISTS `gpci` (
  `u` int(11) NOT NULL,
  `v` char(40) NOT NULL,
  `c` int(11) NOT NULL,
  `first` int(10) unsigned NOT NULL,
  `last` int(10) unsigned NOT NULL,
  PRIMARY KEY (`u`,`v`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `gpci`
--

-- --------------------------------------------------------

--
-- Table structure for table `heartbeat`
--

CREATE TABLE IF NOT EXISTS `heartbeat` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `tstart` int(11) NOT NULL,
  `tlast` int(11) NOT NULL,
  `cleanexit` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `heartbeat`
--

-- --------------------------------------------------------

--
-- Table structure for table `kck`
--

CREATE TABLE IF NOT EXISTS `kck` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `usr` int(11) DEFAULT NULL,
  `ip` varchar(45) NOT NULL,
  `stamp` int(10) unsigned NOT NULL,
  `issuer` int(11) DEFAULT NULL,
  `reason` varchar(144) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `issuer` (`issuer`),
  KEY `usr` (`usr`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `kck`
--

-- --------------------------------------------------------

--
-- Table structure for table `map`
--

CREATE TABLE IF NOT EXISTS `map` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ap` int(11) DEFAULT NULL,
  `name` varchar(24) NOT NULL,
  `filename` varchar(24) NOT NULL,
  `midx` float NOT NULL,
  `midy` float NOT NULL,
  `radi` float NOT NULL,
  `rado` float NOT NULL,
  PRIMARY KEY (`id`),
  KEY `ap` (`ap`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `map`
--

INSERT INTO `map` (`id`, `ap`, `name`, `filename`, `midx`, `midy`, `radi`, `rado`) VALUES
(1, 4, 'LSAP lights fuel', 'lsa-lights-fuel', 1744.22, -2542.66, 900, 1000),
(2, 6, 'LSIF', 'lsif', -239.506, -2167.82, 900, 1000),
(3, 7, 'CALI', 'cali', 2206.81, 1597.62, 900, 1000);

-- --------------------------------------------------------

--
-- Table structure for table `msp`
--

CREATE TABLE IF NOT EXISTS `msp` (
  `i` int(11) NOT NULL AUTO_INCREMENT,
  `a` int(11) NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `t` int(11) NOT NULL,
  `name` varchar(10) NOT NULL,
  `o` int(11) NOT NULL DEFAULT '0',
  `p` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`i`),
  KEY `a` (`a`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `msp`
--

INSERT INTO `msp` (`i`, `a`, `x`, `y`, `z`, `t`, `name`, `o`, `p`) VALUES
(1, 1, 1561.11, 1411.12, 10.7611, 4, 'gate 1', 0, 0),
(2, 1, 1561.29, 1482.51, 10.7425, 4, 'gate 2', 0, 0),
(3, 1, 1555.26, 1537.87, 10.7418, 4, 'gate 3', 0, 0),
(4, 1, 1552.6, 1357.36, 10.7827, 4, 'gate 4', 0, 0),
(5, 1, 1595.7, 1291.33, 12.0098, 56, 'cargo A', 0, 0),
(6, 1, 1555.28, 1627.77, 12.0164, 56, 'cargo B', 0, 0),
(7, 1, 1652.05, 1551.79, 10.9704, 448, 'H0', 0, 0),
(8, 1, 1334.85, 1685.98, 10.9962, 320, 'H1', 0, 0),
(9, 1, 1617.42, 1546.34, 10.9644, 448, 'H2', 0, 0),
(10, 1, 1340.5, 1615.12, 11.742, 2, 'gate P1', 0, 0),
(11, 1, 1340.5, 1571.26, 11.742, 2, 'gate P2', 0, 0),
(12, 0, 280.356, 2060.93, 19.0179, 512, 'M1', 0, 0),
(13, 0, 336.371, 1845.45, 19.0707, 512, 'M2', 0, 0),
(14, 0, 233.4, 1966.37, 19.2479, 256, 'MH1', 0, 0),
(15, 0, 233.4, 2010.74, 19.2877, 256, 'MH2', 0, 0),
(16, 3, -1480.01, -168.973, 14.0639, 4, 'gate 1', 0, 0),
(17, 3, -1355.17, -193.736, 14.0641, 4, 'gate 2', 0, 0),
(18, 3, -1315, -246.987, 13.474, 4, 'gate 3', 0, 0),
(19, 3, -1567.87, -249.855, 15.0677, 2, 'gate P1', 0, 0),
(20, 3, -1592.29, -276.546, 15.0706, 2, 'gate P2', 0, 0),
(21, 3, -1617.36, -302.545, 15.0701, 2, 'gate P3', 0, 0),
(22, 3, -1186.24, 25.6557, 14.1484, 448, 'H1', 0, 0),
(23, 3, -1223.65, -11.2649, 14.1484, 448, 'H2', 0, 0),
(24, 3, -1139.7, -198.8, 15.3438, 56, 'cargo A', 0, 0),
(25, 5, -1311.61, 493.816, 18.9563, 512, 'M1', 0, 0),
(26, 5, -1282.98, 493.816, 18.9649, 256, 'MH1', 0, 0),
(27, 4, 1727.71, -2443, 13.4706, 4, 'gate 1', 0, 0),
(28, 4, 1645.69, -2443, 13.4704, 4, 'gate 2', 0, 0),
(29, 4, 1564.33, -2443, 13.4699, 4, 'gate 3', 0, 0),
(30, 4, 1886.62, -2369.34, 14.4757, 2, 'gate P1', 0, 0),
(31, 4, 1834.39, -2420.57, 14.4772, 2, 'gate P2', 0, 0),
(32, 4, 1964.94, -2314.4, 14.7407, 56, 'cargo A', 0, 0),
(33, 4, 1765.69, -2286.26, 26.796, 448, 'H1', 0, 0),
(34, 6, -270.772, -2154.65, 28.7241, 1, 'gate 1', 0, 0),
(35, 7, 2237.68, 1574.79, 68.746, 1, 'gate 2', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `refuellog`
--

CREATE TABLE IF NOT EXISTS `refuellog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `stamp` int(11) NOT NULL,
  `vehicle` int(11) NOT NULL,
  `player` int(11) NOT NULL,
  `svp` int(11) NOT NULL,
  `paid` int(11) NOT NULL,
  `fuel` float NOT NULL,
  PRIMARY KEY (`id`),
  KEY `player` (`player`),
  KEY `svp` (`svp`),
  KEY `vehicle` (`vehicle`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `refuellog`
--

-- --------------------------------------------------------

--
-- Table structure for table `repairlog`
--

CREATE TABLE IF NOT EXISTS `repairlog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `stamp` int(11) NOT NULL,
  `vehicle` int(11) NOT NULL,
  `player` int(11) NOT NULL,
  `svp` int(11) NOT NULL,
  `paid` int(11) NOT NULL,
  `damage` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `player` (`player`),
  KEY `svp` (`svp`),
  KEY `vehicle` (`vehicle`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `repairlog`
--

-- --------------------------------------------------------

--
-- Table structure for table `rnw`
--

CREATE TABLE IF NOT EXISTS `rnw` (
  `a` int(11) NOT NULL,
  `i` int(11) NOT NULL,
  `s` char(1) NOT NULL,
  `h` float NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `w` float NOT NULL,
  `n` int(11) NOT NULL,
  `type` int(11) NOT NULL DEFAULT '1',
  `surf` int(11) NOT NULL,
  KEY `a` (`a`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `rnw`
--

INSERT INTO `rnw` (`a`, `i`, `s`, `h`, `x`, `y`, `z`, `w`, `n`, `type`, `surf`) VALUES
(1, 0, '', 360, 1477.54, 1245.31, 10.8203, 35, 6, 1, 1),
(1, 0, '', 180, 1477.54, 1697.39, 10.8125, 35, 6, 1, 1),
(0, 0, '', 360, 305.829, 1820.92, 17.6406, 26, 6, 1, 3),
(0, 0, '', 180, 305.829, 2033.43, 17.6406, 26, 6, 1, 3),
(4, 0, 'L', 90, 1503.41, -2493.89, 13.5469, 35, 6, 1, 1),
(4, 1, 'R', 90, 1503.41, -2593.32, 13.5469, 35, 6, 1, 1),
(4, 1, 'L', 270, 1998.25, -2593.32, 13.5469, 35, 6, 1, 1),
(4, 0, 'R', 270, 1998.25, -2493.89, 13.5469, 35, 6, 1, 1),
(2, 0, '', 270, 371.56, 2503.11, 16.4844, 52, 0, 1, 2),
(2, 0, '', 90, 6.9207, 2503.11, 16.4844, 52, 0, 1, 2),
(3, 0, '', 225, -1137.34, 355.299, 14.1484, 35, 6, 1, 1),
(3, 0, '', 45, -1611.92, -119.281, 14.1484, 35, 6, 1, 1),
(5, 0, '', 270, -1256.35, 507.647, 18.2344, 12, 6, 1, 1),
(5, 0, '', 90, -1445.44, 507.678, 18.2344, 12, 6, 1, 1),
(4, 2, 'H', 0, 1765.69, -2286.26, 26.796, 40, 0, 2, 3),
(3, 1, 'H', 0, -1224.36, -11.0377, 14.1484, 30, 0, 2, 3),
(3, 2, 'H', 0, -1186.09, 25.7664, 14.1484, 30, 0, 2, 3),
(6, 0, '', 350, -230.638, -2218.15, 29.2053, 18, 2, 1, 4),
(6, 0, '', 170, -248.472, -2116.85, 29.2741, 18, 2, 1, 4),
(7, 0, '', 90, 2186.74, 1597.6, 67.1996, 17, 2, 1, 3),
(7, 0, '', 270, 2242.2, 1597.53, 67.1996, 17, 0, 1, 3);

-- --------------------------------------------------------

--
-- Table structure for table `ses`
--

CREATE TABLE IF NOT EXISTS `ses` (
  `i` int(11) NOT NULL AUTO_INCREMENT,
  `u` int(11) NOT NULL,
  `s` int(11) NOT NULL,
  `e` int(11) NOT NULL,
  `ip` char(15) NOT NULL,
  PRIMARY KEY (`i`),
  KEY `idx_uid` (`u`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `ses`
--

-- --------------------------------------------------------

--
-- Table structure for table `spw`
--

CREATE TABLE IF NOT EXISTS `spw` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ap` int(11) NOT NULL,
  `class` int(11) NOT NULL,
  `sx` float NOT NULL,
  `sy` float NOT NULL,
  `sz` float NOT NULL,
  `sr` float NOT NULL,
  PRIMARY KEY (`id`),
  KEY `ap` (`ap`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `spw`
--

INSERT INTO `spw` (`id`, `ap`, `class`, `sx`, `sy`, `sz`, `sr`) VALUES
(1, 0, 12, 345.872, 2025.37, 22.6406, 145),
(2, 1, 1, 1320.41, 1268.25, 10.8203, 0),
(3, 3, 1, -1270.95, 7.0975, 14.1484, 140),
(4, 4, 1, 1964.61, -2543.31, 13.5469, 0),
(5, 5, 12, -1360.81, 498.624, 21.25, 0),
(6, 4, 2, 1956.45, -2240.55, 13.5469, 126.149);

-- --------------------------------------------------------

--
-- Table structure for table `svp`
--

CREATE TABLE IF NOT EXISTS `svp` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `apt` int(11) DEFAULT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  PRIMARY KEY (`id`),
  KEY `apt` (`apt`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `svp`
--

INSERT INTO `svp` (`id`, `apt`, `x`, `y`, `z`) VALUES
(1, 1, 1387.05, 1772.94, 10.8203),
(2, 1, 1335.95, 1488.05, 10.8203),
(3, 3, -1171.31, -206.957, 14.1484),
(4, 3, -1655.22, -365.198, 14.1484),
(5, 4, 1548.49, -2630.01, 13.5469),
(6, 4, 2104.57, -2451.49, 13.5469),
(7, 0, 289.898, 1925.75, 17.6406);

-- --------------------------------------------------------

--
-- Table structure for table `tracc`
--

CREATE TABLE IF NOT EXISTS `tracc` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `parent` int(11) NOT NULL,
  `usr` int(11) NOT NULL,
  `ip` varchar(45) NOT NULL,
  `stamp` int(10) unsigned NOT NULL,
  `type` int(11) NOT NULL,
  `comment` varchar(4096) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `usr` (`usr`),
  KEY `parent` (`parent`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `tracc`
--

-- --------------------------------------------------------

--
-- Table structure for table `tract`
--

CREATE TABLE IF NOT EXISTS `tract` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `op` int(11) NOT NULL,
  `stamp` int(11) NOT NULL,
  `updated` int(11) NOT NULL,
  `state` int(11) NOT NULL DEFAULT '0',
  `severity` int(11) NOT NULL DEFAULT '0',
  `visibility` int(11) NOT NULL DEFAULT '2147483647',
  `released` int(10) unsigned DEFAULT NULL,
  `summary` varchar(80) COLLATE utf8mb4_unicode_ci NOT NULL,
  `description` varchar(4096) COLLATE utf8mb4_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  KEY `op` (`op`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci ;

--
-- Dumping data for table `tract`
--

-- --------------------------------------------------------

--
-- Table structure for table `usr`
--

CREATE TABLE IF NOT EXISTS `usr` (
  `i` int(11) NOT NULL AUTO_INCREMENT,
  `name` char(24) NOT NULL,
  `pw` char(60) NOT NULL,
  `registertime` int(10) unsigned NOT NULL,
  `lastseengame` int(11) NOT NULL,
  `lastseenweb` int(11) NOT NULL DEFAULT '0',
  `onlinetime` int(11) NOT NULL DEFAULT '0',
  `playtime` int(11) NOT NULL DEFAULT '0',
  `groups` int(11) NOT NULL,
  `flighttime` int(11) NOT NULL DEFAULT '0',
  `distance` float NOT NULL DEFAULT '0',
  `score` int(11) NOT NULL DEFAULT '0',
  `cash` int(11) NOT NULL DEFAULT '15000',
  `prefs` int(11) NOT NULL,
  `lastfal` int(11) NOT NULL DEFAULT '0',
  `falng` int(11) NOT NULL DEFAULT '0',
  `falnw` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`i`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `usr`
--

-- --------------------------------------------------------

--
-- Table structure for table `veh`
--

CREATE TABLE IF NOT EXISTS `veh` (
  `i` int(11) NOT NULL AUTO_INCREMENT,
  `m` int(11) NOT NULL,
  `ownerplayer` int(11) DEFAULT NULL,
  `e` tinyint(1) NOT NULL DEFAULT '1',
  `ap` int(11) DEFAULT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `r` float NOT NULL,
  `col1` int(11) NOT NULL,
  `col2` int(11) NOT NULL,
  `inusedate` int(11) NOT NULL,
  `odo` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`i`),
  KEY `unq_veh_o` (`ownerplayer`),
  KEY `relatedap` (`ap`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

--
-- Dumping data for table `veh`
--

INSERT INTO `veh` (`i`, `m`, `ownerplayer`, `e`, `ap`, `x`, `y`, `z`, `r`, `col1`, `col2`, `inusedate`, `odo`) VALUES
(4, 593, NULL, 1, 4, 1834.1, -2630.8, 14.1, 0, 42, 119, 1566672861, 0),
(5, 593, NULL, 1, 4, 1877.1, -2630.8, 14.1, 0, 42, 119, 1566672861, 0),
(6, 593, NULL, 1, 4, 1901.5, -2630.8, 14.1, 0, 42, 119, 1566672861, 0),
(7, 593, NULL, 1, 4, 1766.5, -2630.8, 14.1, 0, 42, 119, 1566672861, 0),
(8, 593, NULL, 1, 4, 1742.6, -2630.8, 14.1, 0, 42, 119, 1566672861, 0),
(9, 593, NULL, 1, 4, 1807.3, -2630.8, 14.1, 0, 42, 119, 1566672861, 0),
(10, 511, NULL, 1, 4, 1951.7, -2641.4, 15.1, 0, 34, 51, 1566672861, 0),
(11, 511, NULL, 1, 4, 1975.6, -2641.4, 15.1, 0, 4, 90, 1566672861, 0),
(12, 487, NULL, 1, 4, 1896.2, -2286.6, 13.8, 270, 74, 35, 1566672861, 0),
(13, 487, NULL, 1, 4, 1896.2, -2301.4, 13.8, 270, 3, 29, 1566672861, 0),
(14, 487, NULL, 1, 4, 1896.2, -2272.2, 13.8, 270, 26, 57, 1566672861, 0),
(15, 563, NULL, 1, 4, 1931.8, -2249.7, 14.4, 224, 1, 3, 1566672861, 0),
(16, 563, NULL, 1, 4, 1916.3, -2249.4, 14.4, 224, 1, 6, 1566672861, 0),
(17, 469, NULL, 1, 4, 1909.5, -2333.2, 13.6, 198, 42, 119, 1566672861, 0),
(18, 513, NULL, 1, 4, 2056.7, -2418.92, 14.092, 180, 2, 75, 1566672861, 0),
(19, 513, NULL, 1, 4, 2056.7, -2431.3, 14.0928, 180, 25, 53, 1566672861, 0),
(20, 553, NULL, 1, 4, 2005.4, -2643, 15.8, 0, 71, 87, 1566672861, 0),
(21, 553, NULL, 1, 4, 2036.55, -2643, 15.8, 0, 38, 9, 1566672861, 0),
(22, 553, NULL, 1, 4, 2067.7, -2643, 15.8, 0, 76, 252, 1566672861, 0),
(23, 519, NULL, 1, 4, 1985, -2398.9, 14.5, 90, 42, 119, 1566672861, 0),
(24, 519, NULL, 1, 4, 1985, -2372, 14.5, 90, 42, 119, 1566672861, 0),
(25, 519, NULL, 1, 4, 1985, -2424.61, 14.466, 90, 42, 119, 1566672861, 0),
(27, 592, NULL, 1, 4, 1474.2, -2433.8, 14.6, 180, 148, 36, 1566672861, 0),
(28, 592, NULL, 1, 4, 1427, -2433.8, 14.6, 180, 198, 242, 1566672861, 0),
(29, 577, NULL, 1, 4, 2020.66, -2435.61, 13.3, 180, 8, 7, 1566672861, 0),
(30, 538, NULL, 1, NULL, -1944.53, 143.962, 25.7109, 356.265, 1, 1, 1576368129, 0),
(31, 538, NULL, 1, NULL, 1450.29, 2632.28, 10.8203, 90.14, 1, 1, 1576368147, 0),
(32, 538, NULL, 1, NULL, 2866.42, 1273.08, 10.8203, 179.626, 1, 1, 1576368162, 0),
(33, 538, NULL, 1, NULL, 1735.01, -1953.88, 13.5469, 271, 1, 1, 1576368185, 0);

-- --------------------------------------------------------

--
-- Table structure for table `webses`
--

CREATE TABLE IF NOT EXISTS `webses` (
  `id` char(32) NOT NULL,
  `usr` int(11) NOT NULL,
  `stamp` int(11) NOT NULL,
  `lastupdate` int(11) NOT NULL,
  `stay` int(11) NOT NULL,
  `ip` char(45) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `user` (`usr`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `webses`
--

-- --------------------------------------------------------

--
-- Table structure for table `wth`
--

CREATE TABLE IF NOT EXISTS `wth` (
  `w` tinyint(4) NOT NULL,
  `l` tinyint(4) NOT NULL,
  `t` int(10) unsigned NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `wth`
--

--
-- Constraints for dumped tables
--

--
-- Constraints for table `acl`
--
ALTER TABLE `acl`
  ADD CONSTRAINT `fk_acl_u` FOREIGN KEY (`u`) REFERENCES `usr` (`i`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `art`
--
ALTER TABLE `art`
  ADD CONSTRAINT `fk_art_cat_artcat_id` FOREIGN KEY (`cat`) REFERENCES `artcat` (`id`);

--
-- Constraints for table `artalt`
--
ALTER TABLE `artalt`
  ADD CONSTRAINT `fk_artalt_art_art_id` FOREIGN KEY (`art`) REFERENCES `art` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `artcat`
--
ALTER TABLE `artcat`
  ADD CONSTRAINT `fk_artcat_parent_artcat_id` FOREIGN KEY (`parent`) REFERENCES `artcat` (`id`);

--
-- Constraints for table `cmdlog`
--
ALTER TABLE `cmdlog`
  ADD CONSTRAINT `fk_cmdlog_player_usr_i` FOREIGN KEY (`player`) REFERENCES `usr` (`i`);

--
-- Constraints for table `deathlog`
--
ALTER TABLE `deathlog`
  ADD CONSTRAINT `fk_deathlog_killee_usr_i` FOREIGN KEY (`killee`) REFERENCES `usr` (`i`),
  ADD CONSTRAINT `fk_deathlog_killer_usr_i` FOREIGN KEY (`killer`) REFERENCES `usr` (`i`);

--
-- Constraints for table `fal`
--
ALTER TABLE `fal`
  ADD CONSTRAINT `fk_fal_u_usr_i` FOREIGN KEY (`u`) REFERENCES `usr` (`i`) ON DELETE SET NULL ON UPDATE SET NULL;

--
-- Constraints for table `flg`
--
ALTER TABLE `flg`
  ADD CONSTRAINT `fk_flg_fapt` FOREIGN KEY (`fapt`) REFERENCES `apt` (`i`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_flg_fmsp` FOREIGN KEY (`fmsp`) REFERENCES `msp` (`i`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_flg_player` FOREIGN KEY (`player`) REFERENCES `usr` (`i`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_flg_tapt` FOREIGN KEY (`tapt`) REFERENCES `apt` (`i`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_flg_tmsp` FOREIGN KEY (`tmsp`) REFERENCES `msp` (`i`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_flg_vehicle` FOREIGN KEY (`vehicle`) REFERENCES `veh` (`i`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `gpci`
--
ALTER TABLE `gpci`
  ADD CONSTRAINT `fk_gpci_u_usr_i` FOREIGN KEY (`u`) REFERENCES `usr` (`i`);

--
-- Constraints for table `kck`
--
ALTER TABLE `kck`
  ADD CONSTRAINT `fk_kck_issuer_i` FOREIGN KEY (`issuer`) REFERENCES `usr` (`i`),
  ADD CONSTRAINT `fk_kck_usr_i` FOREIGN KEY (`usr`) REFERENCES `usr` (`i`);

--
-- Constraints for table `map`
--
ALTER TABLE `map`
  ADD CONSTRAINT `fk_maps_ap_apt_id` FOREIGN KEY (`ap`) REFERENCES `apt` (`i`);

--
-- Constraints for table `msp`
--
ALTER TABLE `msp`
  ADD CONSTRAINT `fk_msp_a` FOREIGN KEY (`a`) REFERENCES `apt` (`i`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `refuellog`
--
ALTER TABLE `refuellog`
  ADD CONSTRAINT `fk_refuellog_player` FOREIGN KEY (`player`) REFERENCES `usr` (`i`),
  ADD CONSTRAINT `fk_refuellog_svp` FOREIGN KEY (`svp`) REFERENCES `svp` (`id`),
  ADD CONSTRAINT `fk_refuellog_vehicle` FOREIGN KEY (`vehicle`) REFERENCES `veh` (`i`);

--
-- Constraints for table `repairlog`
--
ALTER TABLE `repairlog`
  ADD CONSTRAINT `fk_repairlog_player` FOREIGN KEY (`player`) REFERENCES `usr` (`i`),
  ADD CONSTRAINT `fk_repairlog_svp` FOREIGN KEY (`svp`) REFERENCES `svp` (`id`),
  ADD CONSTRAINT `fk_repairlog_vehicle` FOREIGN KEY (`vehicle`) REFERENCES `veh` (`i`);

--
-- Constraints for table `rnw`
--
ALTER TABLE `rnw`
  ADD CONSTRAINT `fk_rnw_a` FOREIGN KEY (`a`) REFERENCES `apt` (`i`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `ses`
--
ALTER TABLE `ses`
  ADD CONSTRAINT `fk_ses_u_i` FOREIGN KEY (`u`) REFERENCES `usr` (`i`) ON DELETE CASCADE;

--
-- Constraints for table `spw`
--
ALTER TABLE `spw`
  ADD CONSTRAINT `fk_spw_ap_apt_i` FOREIGN KEY (`ap`) REFERENCES `apt` (`i`);

--
-- Constraints for table `svp`
--
ALTER TABLE `svp`
  ADD CONSTRAINT `fk_svp_apt_apt_i` FOREIGN KEY (`apt`) REFERENCES `apt` (`i`);

--
-- Constraints for table `tracc`
--
ALTER TABLE `tracc`
  ADD CONSTRAINT `fk_tracc_parent_tract_id` FOREIGN KEY (`parent`) REFERENCES `tract` (`id`),
  ADD CONSTRAINT `fk_tracc_usr_usr_i` FOREIGN KEY (`usr`) REFERENCES `usr` (`i`);

--
-- Constraints for table `tract`
--
ALTER TABLE `tract`
  ADD CONSTRAINT `fk_tract_op_usr_i` FOREIGN KEY (`op`) REFERENCES `usr` (`i`);

--
-- Constraints for table `veh`
--
ALTER TABLE `veh`
  ADD CONSTRAINT `fk_veh_ap_apt_i` FOREIGN KEY (`ap`) REFERENCES `apt` (`i`),
  ADD CONSTRAINT `fk_veh_ownerplayer_usr_i` FOREIGN KEY (`ownerplayer`) REFERENCES `usr` (`i`) ON DELETE CASCADE ON UPDATE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
