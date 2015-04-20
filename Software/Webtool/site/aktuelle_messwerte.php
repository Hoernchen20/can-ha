<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
    <head>
        <title>Aktuelle Messwerte</title>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <meta name="author" content="Felix Horn">
        <link rel="stylesheet" type="text/css" href="../styles.css">
    </head>
    
    <body>
    <div class="rahmen">
    <div class="inhalt">
        <h1>Aktuelle Messwerte</h1>    
        <?php
            include '../dbconnect.php';

            $query = 'SELECT
                        CONCAT_WS( \' \', messwert.tw1, messwert.tw2, messwert.tw3 ) AS infopunkt,
                        CONCAT(messwert.akt_messwert, messwert.einheit) AS messwert,
                        DATE_FORMAT(messwert.akt_messwert_zeitstempel, \'%H:%i:%s %e.%m.%y\') AS update_time
                      FROM
                        messwert';

                    $result = mysqli_query($db, $query) or die (mysqli_error());
                    mysqli_close($db);
                    
                    echo '    <table>' . "\n";
                    while($row = mysqli_fetch_object($result)) {
                        echo '        <tr>' . "\n";
                        echo '            <td>' . $row->infopunkt . '</td>' . "\n";
                        echo '            <td>' . $row->messwert . '</td>' . "\n";
                        echo '            <td>' . $row->update_time . '</td>' . "\n";
                        echo '        </tr>' . "\n";
                    }
                    echo '    </table>' . "\n";
            ?>
    </div>
    </div>
    </body>
</html>
