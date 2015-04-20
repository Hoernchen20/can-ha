<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
    <head>
        <title>Meldungen</title>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <meta name="author" content="Felix Horn">
        <link rel="stylesheet" type="text/css" href="../styles.css">
    </head>
    
    <body>
    <div class="rahmen">
    <div class="inhalt">
        <h1>Meldungen</h1>    
        <?php
            include '../dbconnect.php';
            $query = 'SELECT
                        CONCAT_WS( \' \', meldung.tw1, meldung.tw2, meldung.tw3 ) AS infopunkt,
                        status_00,
                        status_01,
                        status_10,
                        status_11,
                        akt_status,
                        DATE_FORMAT( akt_status_zeitstempel, \'%H:%i:%s %d.%m.%Y\' ) AS update_time
                      FROM
                        meldung';

            $result = mysqli_query($db, $query);
            mysqli_close($db);
            
            echo '        <table>' . "\n";
            while($row = mysqli_fetch_object($result)) {
                /**
                 *Textwort zum aktuellen Status sortieren*/
                if ($row->akt_status == 0) {
                    $textwort_status = $row->status_00;
                }
                else if ($row->akt_status == 1) {
                    $textwort_status = $row->status_01;
                }
                else if ($row->akt_status == 2) {
                    $textwort_status = $row->status_10;
                }
                else {
                    $textwort_status = $row->status_11;
                }
                    
                /**
                 *Tabellenreihen ausgeben*/
                echo '            <tr>' . "\n";
                echo '                <td>' . $row->infopunkt . '</td>' . "\n";
                echo '                <td>' . $textwort_status . '</td>' . "\n";
                echo '                <td>' . $row->update_time . '</td>' . "\n";
                echo '            </tr>' . "\n";
            }
            echo '    </table>' . "\n";
        ?>
    </div>
    </div>
    </body>
</html>
