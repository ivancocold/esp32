<?php
$servername = "localhost";
$username = "root";
$password = "tpuser";
$dbname = "chute";
$port = 3308;

// Fonction pour insérer un message dans la base de données
function insert_message($message)
{
    global $servername, $username, $password, $dbname, $port;
    $conn = new mysqli($servername, $username, $password, $dbname, $port);

    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    $stmt = $conn->prepare("INSERT INTO messages (message) VALUES (?)");
    $stmt->bind_param("s", $message);

    if ($stmt->execute()) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $stmt->error;
    }

    $stmt->close();
    $conn->close();
}

// Fonction pour récupérer et afficher les messages de la base de données
function get_messages() {
    global $servername, $username, $password, $dbname, $port;
    $conn = new mysqli($servername, $username, $password, $dbname, $port);

    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    $sql = "SELECT id, message FROM messages";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        // Output data of each row
        while($row = $result->fetch_assoc()) {
            echo "id: " . $row["id"]. " - Message: " . $row["message"]. "<br>";
        }
    } else {
        echo "0 results";
    }
    $conn->close();
}

// Gestion des requêtes POST pour insérer un message
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["message"]))
{
    insert_message($_POST["message"]);
}

// Toujours afficher les messages
get_messages();
?>

<!-- Formulaire pour insérer un nouveau message -->
<form method="post" action="">
    Message: <input type="text" name="message">
    <input type="submit" value="Submit">
</form>
