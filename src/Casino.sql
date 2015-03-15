-- MySQL dump 10.13  Distrib 5.5.40, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: Casino
-- ------------------------------------------------------
-- Server version	5.5.40-1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Giocatore`
--

DROP TABLE IF EXISTS `Giocatore`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Giocatore` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `password` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `fiches` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Giocatore`
--

LOCK TABLES `Giocatore` WRITE;
/*!40000 ALTER TABLE `Giocatore` DISABLE KEYS */;
INSERT INTO `Giocatore` VALUES (1,'michelangelo','cicciabaciccia',9999);
/*!40000 ALTER TABLE `Giocatore` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Gioco`
--

DROP TABLE IF EXISTS `Gioco`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Gioco` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `nome_gioco` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Gioco`
--

LOCK TABLES `Gioco` WRITE;
/*!40000 ALTER TABLE `Gioco` DISABLE KEYS */;
INSERT INTO `Gioco` VALUES (1,'BlackJack');
/*!40000 ALTER TABLE `Gioco` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Partita`
--

DROP TABLE IF EXISTS `Partita`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Partita` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `id_gioco` int(11) DEFAULT NULL,
  `giocatori_max` int(11) NOT NULL,
  `giocatori_min` int(11) NOT NULL,
  `data_inizio` datetime NOT NULL,
  `data_fine` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `IDX_2CBE73C195CD6D49` (`id_gioco`),
  CONSTRAINT `FK_2CBE73C195CD6D49` FOREIGN KEY (`id_gioco`) REFERENCES `Gioco` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Partita`
--

LOCK TABLES `Partita` WRITE;
/*!40000 ALTER TABLE `Partita` DISABLE KEYS */;
INSERT INTO `Partita` VALUES (1,1,7,1,'2015-02-04 16:07:00','2015-02-04 17:07:00');
/*!40000 ALTER TABLE `Partita` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Sessione`
--

DROP TABLE IF EXISTS `Sessione`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Sessione` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `id_giocatore` int(11) DEFAULT NULL,
  `chiave` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `ora_login` datetime NOT NULL,
  `ip_macchina` varchar(15) COLLATE utf8_unicode_ci NOT NULL,
  `hostname` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  KEY `IDX_97C24A144151D504` (`id_giocatore`),
  CONSTRAINT `FK_97C24A144151D504` FOREIGN KEY (`id_giocatore`) REFERENCES `Giocatore` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Sessione`
--

LOCK TABLES `Sessione` WRITE;
/*!40000 ALTER TABLE `Sessione` DISABLE KEYS */;
/*!40000 ALTER TABLE `Sessione` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `giocatori_partite`
--

DROP TABLE IF EXISTS `giocatori_partite`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `giocatori_partite` (
  `giocatore_id` int(11) NOT NULL,
  `partita_id` int(11) NOT NULL,
  PRIMARY KEY (`giocatore_id`,`partita_id`),
  KEY `IDX_58D57C89A2AA48E7` (`giocatore_id`),
  KEY `IDX_58D57C8991BA8E05` (`partita_id`),
  CONSTRAINT `FK_58D57C8991BA8E05` FOREIGN KEY (`partita_id`) REFERENCES `Partita` (`id`) ON DELETE CASCADE,
  CONSTRAINT `FK_58D57C89A2AA48E7` FOREIGN KEY (`giocatore_id`) REFERENCES `Giocatore` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `giocatori_partite`
--

LOCK TABLES `giocatori_partite` WRITE;
/*!40000 ALTER TABLE `giocatori_partite` DISABLE KEYS */;
/*!40000 ALTER TABLE `giocatori_partite` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-02-05 18:53:49
