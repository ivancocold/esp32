// Import de la bibliothèque painlessMesh
#include "painlessMesh.h"

//************************************************************
// Utilisation de la bibliothèque painlessMesh
//
// 1. Envoie un message précis à chaque nœud du réseau
// 2. Affiche tout ce qu'il reçoit sur Serial.print
// 
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
bool lastButtonState = LOW;  // Pour suivre l'état précédent du bouton
unsigned long lastHighTime = 0;  // Pour suivre le temps depuis que le bouton est HIGH
int reinit = 5000; //Temps d'attente entre 2 messages d'un même noeud en millisecondes.

// Objet Scheduler pour contrôler les tâches personnalisées
Scheduler userScheduler;

// Création de l'objet mesh
painlessMesh mesh;

// Prototype de la fonction pour envoyer des messages
void sendMessage();

// Définition de la tâche d'envoie de message en boucle infinie.
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

//Définition des fonctions

// Fonction pour envoyer des messages
void sendMessage()
{
  // Lire l'état de la pin 1
  int pinState = digitalRead(1);
  // Vérifier si la pin 1 est HIGH
  if (pinState == HIGH)
  {
    nodeId = mesh.getNodeId();
    // Message à envoyer
    String msg = "Chute; Mr MAGAGI. ID : " + String(nodeId);
    // Envoi du message à tous les nœuds du wifi-mesh
    mesh.sendBroadcast(msg);
    Serial.println(msg);  // Envoie le message via le port série
    delay(20000);  // Attendre 20 secondes avant d'envoyer le prochain message
  }
}

// Callback pour les messages reçus
void receivedCallback(uint32_t from, String &msg)
{
  // Affiche le message reçu et l'ID de l'expéditeur
  if (msg.length() > 0)
    {
      //Affiche le message reçu.
      Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
      // Envoie le message via le port série
      Serial.println(msg);
      delay(20000);  // Attendre 20 secondes avant d'envoyer le prochain message
    }
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
void nodeTimeAdjustedCallback(int32_t offset) {
  // Affiche l'heure ajustée et l'offset
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

// Fonction de configuration
void setup() {
  // Initialisation de la communication série à 115200 bauds
  Serial.begin(115200);
  Serial.println("Connecting to WiFiMesh...");

  // Configuration de la pin 1 comme entrée
  pinMode(1, INPUT);

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  // Configure les types de messages de débogage à afficher avant l'initialisation
  mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages

  // Initialisation du réseau mesh avec les paramètres définis
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  Serial.println("\nConnected to WiFi mesh");

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

  // Lire l'état actuel du bouton
  int currentButtonState = digitalRead(1);

  // Vérifier si le bouton vient d'être pressé (transition de LOW à HIGH)
  if (currentButtonState == HIGH && lastButtonState == LOW)
  {
    // Envoyer un message lorsque le bouton est pressés
    sendMessage();
    // Enregistrer le moment où le bouton a été pressé
    lastHighTime = millis();
  }

  // Si le bouton est maintenu enfoncé, vérifier le temps écoulé
  if (currentButtonState == HIGH && lastButtonState == HIGH)
  {
    if (millis() - lastHighTime >= reinit)  // 5 secondes
    {
      // Réinitialiser lastButtonState à LOW après 5 secondes
      lastButtonState = LOW;
    }
  }
}
