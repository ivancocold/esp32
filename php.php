<?php
// Connexion à la base de données
$server = "localhost";
$db = "chute";
$user = "root";
$psw = "tpuser";
$port = 3308;

$conn = mysqli_connect($server, $user, $psw, $db, $port);

// Vérification de la connexion
if (!$conn) {
    die("Erreur de connexion : " . mysqli_connect_error());
}

// Requête SQL
$query = "SELECT id, timestamp, message, patient FROM messages ORDER BY id DESC"; // Remplacez "votre_table" par le nom de votre table
$result = mysqli_query($conn, $query);

// Vérification du résultat de la requête
if ($result) {
    // Construction du tableau HTML
    $html = "<table>";
    $html .= "<caption>Messages reçus</caption>"; // Titre du tableau
    $html .= "<tr><th>ID</th><th>Timestamp</th><th>Message</th><th>Patient</th></tr>";
    while ($row = mysqli_fetch_assoc($result)) {
        $html .= "<tr>";
        $html .= "<td>" . $row['id'] . "</td>";
        $html .= "<td>" . $row['timestamp'] . "</td>";
        $html .= "<td>" . $row['message'] . "</td>";
        $html .= "<td>" . $row['patient'] . "</td>";
        $html .= "</tr>";
    }
    $html .= "</table>";

    // Afficher le contenu du tableau
    echo $html;
} else {
    echo "Erreur dans la requête : " . mysqli_error($conn);
}

// Fermeture de la connexion
mysqli_close($conn);
?>
