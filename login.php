<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>login</title>
</head>
<body>
    <form action="/login.php" method="POST">
        <input name="user_name" placeholder="user name" />
        <input name="password" type="password" placeholder="password" />
        <input name="secret" placeholder="secret" />
        <input type="submit"/>
    </form>
    <?php
        session_start();
        $password = "123456";
        $user_name = "4yuub";
        print_r($_POST);
        if ($_SERVER["REQUEST_METHOD"] === "POST") {
            if ($_POST["user_name"] !== $user_name || $_POST["password"] !== $password) {
                echo "<h1>Invalid data!</h1>";
            }
            else {
                $_SESSION["user_name"] = $user_name;
                $_SESSION["secret"] = $_POST["secret"];
            }
        }
    ?>
</body>
</html>