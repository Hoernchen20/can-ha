<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
    <head>
        <title>Aktuelle Sollwerte</title>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <meta name="author" content="Felix Horn">
        <link rel="stylesheet" type="text/css" href="../styles.css">
    </head>
    
    <body>
    <div class="rahmen">
    <div class="inhalt">
        <h1>Aktuelle Sollwerte</h1>    
        <?php
            include '../dbconnect.php';

            $query = 'SELECT
                        CONCAT_WS( \' \', sollwert.tw1, sollwert.tw2, sollwert.tw3 ) AS infopunkt,
                        CONCAT(sollwert.akt_sollwert, sollwert.einheit) AS sollwert,
                        DATE_FORMAT(sollwert.akt_sollwert_zeitstempel, \'%H:%i:%s %e.%m.%y\') AS update_time
                      FROM
                        sollwert';

                    $result = mysqli_query($db, $query) or die (mysqli_error());
                    mysqli_close($db);
                    
                    echo '    <table>' . "\n";
                    while($row = mysqli_fetch_object($result)) {
                        echo '        <tr>' . "\n";
                        echo '            <td>' . $row->infopunkt . '</td>' . "\n";
                        echo '            <td>' . $row->sollwert . '</td>' . "\n";
                        echo '            <td>' . $row->update_time . '</td>' . "\n";
                        echo '        </tr>' . "\n";
                    }
                    echo '    </table>' . "\n";
            ?>
    </div>
    </div>
    </body>
</html>
