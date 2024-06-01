
// Import de la bibliothèque painlessMesh
#include "painlessMesh.h"
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
//#include <MySQL_Encrypt_Sha1.h>
//#include <MySQL_Packet.h>

//************************************************************
// Exemple simple utilisant la bibliothèque painlessMesh
//
// 1. Envoie un message aléatoire à chaque nœud du réseau mesh entre 1 et 5 secondes
// 2. Affiche tout ce qu'il reçoit sur Serial.print
//
//
//************************************************************


// Nom du réseau
#define MESH_PREFIX "CotonLePlusBeau"

// Mdp du réseau
#define MESH_PASSWORD "somethingSneaky"

// Port utilisé pour la communication sur le réseau mesh
#define MESH_PORT 5555


// Déclaration des variables globales
uint32_t nodeId;
unsigned long timestamp;

// Objet Scheduler pour contrôler les tâches personnalisées
Scheduler userScheduler;

// Création de l'objet mesh
painlessMesh mesh;

// Prototype de la fonction pour envoyer des messages
void sendMessage();

// Prototype pour éviter les avertissements de PlatformIO
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

// Fonction pour envoyer des messages
void sendMessage()
{
  // Lire l'état de la pin 1
  int pinState = digitalRead(1);

  // Vérifier si la pin 1 est HIGH
  if (pinState == LOW)
  {
    nodeId = mesh.getNodeId();

    // Message à envoyer
    String msg = "Chute; Mr MAGAGI. ID : " + String(nodeId);

    // Envoi du message à tous les nœuds
    mesh.sendBroadcast(msg);
    Serial.println(msg);

    // Définit un intervalle aléatoire entre 1 et 5 secondes pour le prochain message
    taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
  }
}

// Callback pour les messages reçus
void receivedCallback(uint32_t from, String &msg)
{
  // Affiche le message reçu et l'ID de l'expéditeur
  if (msg.length() > 0)
  {
    Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  }
  // Affiche un message si le message reçu est vide
  else 
  {
    Serial.printf("startHere: Received an empty message from %u\n", from);
  }
}

// Callback pour les nouvelles connexions
void newConnectionCallback(uint32_t nodeId)
{
  // Affiche l'ID du nouveau nœud connecté
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

// Callback pour les changements de connexion
void changedConnectionCallback()
{
  // Affiche un message lorsque les connexions changent
  Serial.printf("Changed connections\n");
}

// Callback pour l'ajustement du temps des nœuds
void nodeTimeAdjustedCallback(int32_t offset)
{
  // Affiche l'heure ajustée et l'offset
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

// Fonction de configuration
void setup()
{
  // Initialisation de la communication série à 115200 bauds
  Serial.begin(115200);

  // Configuration de la pin 1 comme entrée
  pinMode(1, INPUT);

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  // Configure les types de messages de débogage à afficher avant l'initialisation
  mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages

  // Initialisation du réseau mesh avec les paramètres définis
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  // Définition du callback pour les messages reçus
  mesh.onReceive(&receivedCallback);

  // Définition du callback pour les nouvelles connexions
  mesh.onNewConnection(&newConnectionCallback);

  // Définition du callback pour les changements de connexion
  mesh.onChangedConnections(&changedConnectionCallback);

  // Définition du callback pour l'ajustement du temps des nœuds
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // Ajout de la tâche d'envoi de message au schedule
  userScheduler.addTask(taskSendMessage);

  // Activation de la tâche
  taskSendMessage.enable();
}

// Fonction de boucle principale
void loop()
{
  // it will run the user scheduler as well
  // Mise à jour du réseau mesh
  mesh.update();
}
