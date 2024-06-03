#Déscription du projet à venir

L'objectif est d’envoyer des messages avec plusieurs ESP32  connectés en mesh. Quand il y a plusieurs ESP32 connectés, chaque ESP32 reçoit des messages.
 
Cas d’usage : L'objectif global est de faire un maquette de suivi pour des personnes âgées. Par exemple, si une personne tombe, le capteur envoie un message/alerte. Ici dans le projet, la chute est simulé par l'activation de la pin 1 de la carte. Dans le cadre de mon projet, la pin est activée par un boutton branché dessus. Il peut être activé au toucher de la pin directement. Vous pouvez adapter le code et brancher un accéléromètre sur la pin.

Les messages reçus par une carte sont filtrés. Ceux qui correspondent à un certain format sont envoyés sur un port série. Un script python lit le port série et filtre les messages. Les messages amorcent des requêtes SQL d'insertion ou de suppression dans une base de données MySql. Une page web affiche les alertes en temps réel.

Un administrateur peut se connecter sur une carte par BLE (Bluetooth Low Energy). Il pourra réaliser les opération suivantes :
- réinitialiser le compteur de chutes de la carte
- supprimer les lignes de la carte insérées dans la base de données
- supprimer toutes les lignes de la base de données.
La connection se fait à partir d'un terminal mobile comme une application android, tablette ou Ios. Les tests on été réalisés sur nRF Connect pour Ios.

#Ressources utilisées
##Matériel
Pour réaliser ce projet, on a obligatoirement besoin de:
- une carte ESP32


#Logiciel
Oligatoirement :
- IDE type Arduino IDE
- Environnement de développement python (Pycharm)
- Editeur de code
- Moteur de base de données MySql
- Serveur de base de données web

#Dépenses
Python:
- 

Arduino :
- 

