<?php
    $db = mysqli_connect("localhost", "***", "***", "raspi_control");
     
    if(!$db) {
        exit("Verbindungsfehler: ".mysqli_connect_error());
    }

    //Zeichensatz der Verbindung zur Datenbank auf UTF-8 umstellen, um Zeichen wie ° korrekt zu lesen
    mysqli_set_charset($db, 'utf8');
?>
