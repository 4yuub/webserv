<!DOCTYPE html>
<html>
    <head>
        <title>PHP-CGI</title>
    </head>
    <body>

        <?php
            echo "<h1>Hi, I'm a PHP script!</h1>";
            echo "<p> this is a random number from 1 to 100: " . random_int(1, 100) . "</p>";
            echo "<p> Biscuits: </p>";
            print_r($_COOKIE);
            // set cookies
            setcookie("name", "Ayoub");
            setcookie("my_cookie", "I am a Cookie");
            setcookie("token", "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c");
        ?>

    </body>
</html>