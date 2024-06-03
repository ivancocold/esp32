// Import de la bibliothèque painlessMesh
#include "painlessMesh.h"
#include <ArduinoBLE.h>

// Nom du réseau
#define MESH_PREFIX "CotonLePlusBeau"
// Mdp du réseau
#define MESH_PASSWORD "somethingSneaky"

// Port utilisé pour la communication sur le réseau mesh
#define MESH_PORT 5555

// Déclaration des variables globales
uint32_t nodeId; //ID du noeud dans le réseau.
bool lastButtonState = LOW;  // Pour suivre l'état précédent de la pin.
unsigned long lastHighTime = 0;  // Pour suivre le temps depuis lequel la pin est HIGH
int reinit = 5000; //Temps d'attente entre 2 messages d'un même noeud en millisecondes.
unsigned long activationCount  = 0; //Nombre de chutes
String NodeName="M. MAGAGI";

// Objet Scheduler pour contrôler les tâches personnalisées
Scheduler userScheduler;

// Création de l'objet mesh
painlessMesh mesh;

// Prototype de la fonction pour envoyer des messages
void sendMessage();

// Définition de la tâche d'envoi de message en boucle infinie.
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

// Déclaration des caractéristiques BLE
BLEService resetService("180F"); // UUID du service
BLEByteCharacteristic resetCharacteristic("2A19", BLEWrite| BLERead); // UUID de la caractéristique pour réinitialisation
BLEUnsignedLongCharacteristic countCharacteristic("2A1C", BLERead); // UUID de la caractéristique de compteur
BLEUnsignedLongCharacteristic nodeIdCharacteristic("2A1D", BLERead); // UUID de la caractéristique d'ID de nœud
BLEUnsignedLongCharacteristic connectionCountCharacteristic("2A1E", BLERead); // UUID de la caractéristique de nombre de connexions
BLEStringCharacteristic connectionStatusCharacteristic("2A1F", BLERead, 20); // UUID de la caractéristique de statut de connexion

void setupCharacteristics()
{
  // Déclaration des descripteurs
  BLEDescriptor resetDescriptor("2A19", "Reset Command");
  BLEDescriptor countDescriptor("2A1C", "Compteur de chutes");
  BLEDescriptor nodeIdDescriptor("2A1D", "Node ID");
  BLEDescriptor connectionCountDescriptor("2A1E", "Nombre de connexions");
  BLEDescriptor connectionStatusDescriptor("2A1F", "Etat de la connexion");

  // Ajouter des descripteurs pour les caractéristiques BLE
  resetCharacteristic.addDescriptor(resetDescriptor);
  countCharacteristic.addDescriptor(countDescriptor);
  nodeIdCharacteristic.addDescriptor(nodeIdDescriptor);
  connectionCountCharacteristic.addDescriptor(connectionCountDescriptor);
  connectionStatusCharacteristic.addDescriptor(connectionStatusDescriptor);
}

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
    String msg = "Chute; "+ NodeName +" est tombé pour la "+ String(activationCount)+"è fois de la journée. ID : " + String(nodeId);
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
    // Affiche le message reçu.
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
  connectionCountCharacteristic.writeValue(mesh.getNodeList().size());
  connectionStatusCharacteristic.writeValue("Connected");
}

// Callback pour les changements de connexion
void changedConnectionCallback()
{
  // Affiche un message lorsque les connexions changent
  Serial.printf("Changed connections\n");
  connectionCountCharacteristic.writeValue(mesh.getNodeList().size());
}

// Callback pour l'ajustement du temps des nœuds
void nodeTimeAdjustedCallback(int32_t offset) {
  // Affiche l'heure ajustée et l'offset
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

// Fonction de configuration
void setup()
{
  // Initialisation de la communication série à 115200 bauds
  Serial.begin(115200);
  Serial.println("Connecting to WiFiMesh...");

  // Configuration de la pin 1 comme entrée
  pinMode(1, INPUT);

  // Configuration BLE
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // Configurez le périphérique BLE

  // Conversion de NodeName en char array
  char localName[NodeName.length() + 1];
  NodeName.toCharArray(localName, NodeName.length() + 1);

  BLE.setLocalName(localName);
  BLE.setAdvertisedService(resetService);
  resetService.addCharacteristic(resetCharacteristic);
  resetService.addCharacteristic(countCharacteristic);
  resetService.addCharacteristic(nodeIdCharacteristic);
  resetService.addCharacteristic(connectionCountCharacteristic);
  resetService.addCharacteristic(connectionStatusCharacteristic);
  setupCharacteristics(); // Appeler la fonction pour ajouter les descripteurs
  BLE.addService(resetService);
  resetCharacteristic.writeValue(0);
  countCharacteristic.writeValue(activationCount);
  nodeIdCharacteristic.writeValue(mesh.getNodeId());
  connectionCountCharacteristic.writeValue(mesh.getNodeList().size());
  connectionStatusCharacteristic.writeValue("Disconnected");
  BLE.advertise();
  Serial.println("Bluetooth en attente de connexions...");

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
  // Mise à jour du réseau mesh
  mesh.update();
  // Mise à jour BLE
  BLE.poll();

  // Vérifiez si la caractéristique de réinitialisation a été écrite
  if (resetCharacteristic.written())
  {
    // Lire la valeur de la caractéristique en tant qu'octets
    uint8_t buf[20]; // 20 octets maximum
    int bytesRead = resetCharacteristic.readValue(buf, sizeof(buf));

    if (bytesRead > 0)
    {
      // Convertir les octets en une chaîne de caractères
      String value = "";
      for (int i = 0; i < bytesRead; i++)
      {
        value += (char)buf[i];
      }

      // Vérifier si la valeur est "f"
      if (value == "f")
      {
        activationCount = 0;
        Serial.println("Réinitialisation réussie.");  // Envoyer le message "Réinitialisation réussie." sur le port série
        // Réinitialiser la caractéristique à une chaîne vide après la réinitialisation
        resetCharacteristic.writeValue((uint8_t)0);
      }
      else if (value == "0")
      {
        Serial.println("Reset BDD. " + String(nodeId)); // Envoyer le message "Reset BDD." sur le port série
      }
      else if (value == "a")
      {
        Serial.println("Reset all."); // Envoyer le message "Reset all." sur le port série
      }
    }
  }

  // Lire l'état actuel du bouton
  int currentButtonState = digitalRead(1);

  // Vérifier si le bouton vient d'être pressé (transition de LOW à HIGH)
  if (currentButtonState == HIGH && lastButtonState == LOW)
  {
    activationCount++; // Incrémenter le compteur
    countCharacteristic.writeValue(activationCount); // Mettre à jour la caractéristique
    // Envoyer un message lorsque le bouton est pressé
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

  // Mettre à jour lastButtonState
  lastButtonState = currentButtonState;
}
