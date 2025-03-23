CREATE DATABASE  IF NOT EXISTS `test` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;
USE `test`;
-- MySQL dump 10.13  Distrib 8.0.38, for Win64 (x86_64)
--
-- Host: localhost    Database: test
-- ------------------------------------------------------
-- Server version	8.0.39

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `actors`
--

DROP TABLE IF EXISTS `actors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `actors` (
  `id` int NOT NULL AUTO_INCREMENT,
  `actor_name` varchar(100) DEFAULT NULL,
  `birth` varchar(10) DEFAULT NULL,
  `sex` varchar(5) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ix_actors_actor_name` (`actor_name`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `actors`
--

LOCK TABLES `actors` WRITE;
/*!40000 ALTER TABLE `actors` DISABLE KEYS */;
INSERT INTO `actors` VALUES (1,'徐均朔','1996-12-11','Male'),(2,'方书剑','1998-08-05','Male'),(3,'黄子弘凡','1999-04-21','Male'),(6,'李存贤','1999-07-21','Male'),(7,'李秋盟','1991-09-08','Male');
/*!40000 ALTER TABLE `actors` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `companies`
--

DROP TABLE IF EXISTS `companies`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `companies` (
  `id` int NOT NULL AUTO_INCREMENT,
  `company_name` varchar(100) DEFAULT NULL,
  `company_ip` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ix_companies_company_name` (`company_name`),
  KEY `ix_companies_company_ip` (`company_ip`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `companies`
--

LOCK TABLES `companies` WRITE;
/*!40000 ALTER TABLE `companies` DISABLE KEYS */;
INSERT INTO `companies` VALUES (1,'一台好戏','上海'),(2,'徐俊戏剧','北京'),(3,'魅鲸文化','上海'),(7,'好好有戏','上海');
/*!40000 ALTER TABLE `companies` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `drama_actor`
--

DROP TABLE IF EXISTS `drama_actor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `drama_actor` (
  `drama_id` int NOT NULL,
  `actor_id` int NOT NULL,
  `date` datetime DEFAULT NULL,
  PRIMARY KEY (`drama_id`,`actor_id`),
  KEY `actor_id` (`actor_id`),
  CONSTRAINT `drama_actor_ibfk_1` FOREIGN KEY (`drama_id`) REFERENCES `dramas` (`id`),
  CONSTRAINT `drama_actor_ibfk_2` FOREIGN KEY (`actor_id`) REFERENCES `actors` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `drama_actor`
--

LOCK TABLES `drama_actor` WRITE;
/*!40000 ALTER TABLE `drama_actor` DISABLE KEYS */;
INSERT INTO `drama_actor` VALUES (1,1,'2024-12-01 19:30:00'),(1,2,'2024-12-01 19:30:00'),(3,6,'2024-09-30 19:30:00'),(3,7,'2024-09-30 19:30:00'),(5,7,'2024-10-11 19:30:00');
/*!40000 ALTER TABLE `drama_actor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `dramas`
--

DROP TABLE IF EXISTS `dramas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `dramas` (
  `id` int NOT NULL AUTO_INCREMENT,
  `drama_name` varchar(100) DEFAULT NULL,
  `theatre_name` varchar(100) DEFAULT NULL,
  `company_id` int DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ix_dramas_drama_name` (`drama_name`),
  KEY `company_id` (`company_id`),
  KEY `ix_dramas_theatre_name` (`theatre_name`),
  CONSTRAINT `dramas_ibfk_1` FOREIGN KEY (`company_id`) REFERENCES `companies` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `dramas`
--

LOCK TABLES `dramas` WRITE;
/*!40000 ALTER TABLE `dramas` DISABLE KEYS */;
INSERT INTO `dramas` VALUES (1,'赵氏孤儿','上海文化广场',2),(2,'哈姆雷特','上海大剧院',2),(3,'桑塔露琪亚','星空间6号——一台好戏小剧场',1),(4,'阿波罗尼亚','星空间1号——一台好戏小剧场',1),(5,'太阳黑子','人民大舞台',3),(6,'梦微之','星空间33号——鲸剧场',3),(7,'献给阿尔吉侬的花束','上海文化广场',1);
/*!40000 ALTER TABLE `dramas` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `users` (
  `id` int NOT NULL AUTO_INCREMENT,
  `username` varchar(100) DEFAULT NULL,
  `first_name` varchar(50) DEFAULT NULL,
  `last_name` varchar(50) DEFAULT NULL,
  `email` varchar(50) DEFAULT NULL,
  `hashed_password` varchar(255) DEFAULT NULL,
  `is_active` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ix_users_username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (1,'bingyu','bingyu','li','0522@shazi.com','$2b$12$a5tOfYfQuQO96EWNLyQ2D.8lxbhbDpuKoCpkVAKZyPvT0sTK9p.C.',1),(3,'phoebe','yufei','liu','yufei@email.com','$2b$12$4hJQ0unddZ6hAdzyPxXB0.brVTj0Luap9zglwGE6.09l0IohbQxmG',1),(4,'string','string','string','string@email.com','$2b$12$PI4VlHneDw7Ehkeq6Do7m.iBK0p3YujAGwppcr5xZBygCVHxEKYCG',1),(5,'OK DU','kehao','du','0504@email.com','$2b$12$ccybJS/a6mCqlPl0C2HGIerhSnkSEYipYoe/ld1an.CrwnwC1xpTa',1);
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-11-01  2:02:24
