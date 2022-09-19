<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>delete</title>
</head>
<body>
    <?php
        session_start();
        if ($_SERVER["REQUEST_METHOD"] === "DELETE") {
            echo "<h1> DELETE METHOD WAS USED </h1>";
        }
        else {
            echo "<h1> DELETE METHOD WAS NOT USED </h1>";
        }
    ?>
</body>
</html>