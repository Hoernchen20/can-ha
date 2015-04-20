<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
    <head>
	    <title>Meldebuch</title>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <meta name="author" content="Felix Horn">
        <link rel="stylesheet" type="text/css" href="../styles.css">
    </head>

    <body>
        <div class="rahmen">
        
            <div class="inhalt">
                <h1>Meldebuch</h1>
                <table>
                    <thead>
                        <tr>
                            <th id="id">ID</th>
                            <th id="datum">Datum</th>
                            <th id="zeit">Zeit</th>
                            <th id="name">Name</th>
                            <th id="meldung_id">Meldung ID</th>
                        </tr>
                    </thead>
                    <tbody>
                <?php
                error_reporting (E_ALL);
                ini_set ('display_errors', 'Off');
                ini_set ('log_errors', 'On');
                ini_set ('error_log','../datei.log');

                include '../dbconnect.php';
                
                $get_interval = $_GET['interval'];
                
                /*
                 * Daten von get überprüfen */
                if ($get_interval < 0) {
                    $get_interval = 0;
                }
                
                $query = 'SELECT
                            meldung_archiv.id,
                            DATE_FORMAT( meldung_archiv.zeitstempel, \'%d.%m.%y\' ) AS datum,
                            DATE_FORMAT( meldung_archiv.zeitstempel, \'%H:%i:%s\' ) AS zeit,
                            CONCAT_WS( \' \', meldung.tw1, meldung.tw2, meldung.tw3 ) AS infopunkt,
                            meldung.status_00,
                            meldung.status_01,
                            meldung.status_10,
                            meldung.status_11,
                            meldung_archiv.status,
                            meldung_archiv.info,
                            meldung.id AS meldung_id
                          FROM
                            meldung_archiv
                          INNER JOIN
                            meldung ON meldung_archiv.meldung_id = meldung.id
                          ORDER BY meldung_archiv.id DESC LIMIT ' . $get_interval . ',30';

                $result = mysqli_query($db, $query);
                mysqli_close($db);
                
                while ($row = mysqli_fetch_object($result)) {
                    /**
                     *Textwort zum aktuellen Status sortieren*/
                    if ($row->status == 0) {
                        $textwort_status = $row->status_00;
                    }
                    else if ($row->status == 1) {
                        $textwort_status = $row->status_01;
                    }
                    else if ($row->status == 2) {
                        $textwort_status = $row->status_10;
                    }
                    else if ($row->status == 3) {
                        $textwort_status = $row->status_11;
                    }
                    else {
                        $textwort_status = 'ERROR textwort_status';
                    }

                    /**
                     *Tabellenreihen ausgeben*/
                    echo '                  <tr>' . "\n";
                    echo '                      <td headers="id">' . $row->id . '</td>' . "\n";
                    echo '                      <td headers="datum">' . $row->datum . '</td>' . "\n";
                    echo '                      <td headers="zeit">' . $row->zeit . '</td>' . "\n";
                    echo '                      <td headers="name">' . $row->infopunkt . ' ' . $textwort_status . ' ' . $row->info . '</td>' . "\n";
                    echo '                      <td headers="meldung_id">' . $row->meldung_id . '</td>' . "\n";
                    echo '               </tr>' . "\n";
                }
                ?>
                    </tbody>
                </table>
                
                <p>
                    <a href="meldung_archiv.php?interval=<?php echo $get_interval-30; ?>">&lt;&lt;&lt;</a>
                    <a href="meldung_archiv.php?interval=<?php echo $get_interval+30; ?>">&gt;&gt;&gt;</a>
                </p>
            </div>
        </div>
    </body>
</html>
