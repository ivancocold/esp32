<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Messages</title>
    <link rel="stylesheet" type="text/css" href="styles.css">
    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
    <script>
        $(document).ready(function(){
            function fetchMessages(){
                $.ajax({
                    url: 'php.php',
                    success: function(data){
                        $('#message_table').html(data);
                    }
                });
            }
            // Actualiser les messages toutes les 5 secondes
            setInterval(fetchMessages, 3000);
        });
    </script>
</head>
<body>
    <div id="message_table">
        <?php include('php.php'); ?>
    </div>
</body>
</html>
