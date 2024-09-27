# Déscription du projet

L'objectif est d’envoyer et recevoir des messages avec plusieurs cartes ESP32 connectés en wifi mesh. Chaque ESP32 connecté sur le réseau envoie et reçoit les messages du réseau.
 
# Cas d’usage

L'objectif global est de faire une maquette de suivi pour des personnes âgées par exemple. Si une personne tombe, le capteur envoie un message/alerte. Dans mon projet, la chute est simulée par l'activation de la pin 1 de la carte. La pin est activée par un bouton branché dessus. A défaut d'un bouton, la pin peut être activée en la touchant directement avec la main. Vous pouvez adapter le code et brancher un accéléromètre sur la pin.

Les messages reçus par une carte sont filtrés. Ceux qui correspondent à un certain format sont envoyés sur un port série (COM7). Un script python qui s'exécute en continu lit le port série et filtre les messages. Les messages amorcent des requêtes SQL d'insertion ou de suppression dans une base de données MySql. Une page web affiche les alertes en temps réel. Un fichier php nommé "insert" sert à insérer des lignes dans la base de données pour des fins de test.

Un administrateur peut se connecter sur une carte par BLE (Bluetooth Low Energy). Il pourra réaliser les opérations suivantes :
- réinitialiser le compteur de chutes de la carte
- supprimer les lignes de la carte insérées dans la base de données
- supprimer toutes les lignes de la base de données.
La connection se fait à partir d'un terminal mobile comme une application android, tablette ou Ios. Les tests on été réalisés sur nRF Connect pour Ios.

## Ressources utilisées
### Langages
Dans l'ordre d'importance:
- C++
- Python
- PHP/html
- SQL
- CSS


### Matériel
Pour réaliser ce projet, on a obligatoirement besoin de:
- une carte ESP32
- Un terminal mobile (android/ios) capable de scanner, lire et écrire avec BLE (exemple : nRF Connect).
- Un client web (navigateur)


### Logiciel
Obligatoirement :
- IDE type Arduino IDE
- Environnement de développement python (Pycharm)
- Editeur de code
- Moteur de base de données MySql
- Serveur de base de données web (wampp)

### Dépendances
Python :
- serial
- mysql.connector
- time
- logging

Adruino:
- Bibliothèque "painlessMesh.h"
- Bibliothèque <ArduinoBLE.h>


## Etapes
1. Installer les environnements nécessaires (IDE, base de données)!
2. Créez votre base de données avec le script sql fourni. Assurez-vous que l'utilisateur de votre base de données a les droits requis!
3. Téléversez le code c++ sur votre carte!
4. Lancer ficher get_messages.php
5. Après avoir fermé le l'IDE arduino pour libérer le port série, exécutez le scipt python dans votre environnement de développement ou en ligne de commande. Si vous utiliser wamp comme serveur web, vous pouvez placer tous les fichiers dans le dossier "C:\wamp64\www\chute".
6. Scannez les sources BLE avec votre application mobile et connextez-vous sur la carte!
7. Lisez les données disponibles sur la carte. Saisissez:
   - "f": pour réinitaliser le compteur de la carte.
   - "0" : pour supprimer les lignes liées à cette carte dans la base de données
   - "a" : pour effacer toutes les lignes de la base de données.

