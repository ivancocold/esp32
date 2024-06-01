<?php
// Connexion à la base de données
$server = "localhost";
$db = "chute";
$user = "root";
$psw = "tpuser";
$port = 3308;

$conn = mysqli_connect($server, $user, $psw, $db, $port);

if (!$conn) {
    die("Erreur de connexion : " . mysqli_connect_error());
}

$query = "SELECT id, timestamp, message, patient FROM messages ORDER BY id DESC";
$result = mysqli_query($conn, $query);

if ($result) {
    echo "<table>";
    echo "<caption>Messages reçus</caption>";
    echo "<tr><th>ID</th><th>Timestamp</th><th>Message</th><th>Patient</th></tr>";
    while ($row = mysqli_fetch_assoc($result)) {
        echo "<tr>";
        echo "<td>" . $row['id'] . "</td>";
        echo "<td>" . $row['timestamp'] . "</td>";
        echo "<td>" . $row['message'] . "</td>";
        echo "<td>" . $row['patient'] . "</td>";
        echo "</tr>";
    }
    echo "</table>";
} else {
    echo "Erreur dans la requête : " . mysqli_error($conn);
}

mysqli_close($conn);
?>
