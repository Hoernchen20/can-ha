<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<?php
    include '../dbconnect.php';
    
    $get_messwert_id = $_GET['messwert_id'];
    $get_interval = $_GET['interval'];

    $query = 'SELECT
                CONCAT_WS( \' \', messwert.tw1, messwert.tw2, messwert.tw3) AS infopunkt,
                messwert_archiv.wert,
                messwert.einheit,
                DATE_FORMAT(messwert_archiv.zeitstempel, \'%Y, %m-1, %e, %k, %i\') AS zeit
              FROM
                messwert_archiv
              INNER JOIN
                messwert ON messwert_archiv.messwert_id = messwert.id
              WHERE messwert_archiv.zeitstempel >= DATE_SUB(NOW(), INTERVAL ' . $get_interval . ' HOUR)
                AND messwert_archiv.messwert_id = ' . $get_messwert_id . ' ORDER BY messwert_archiv.id';

    $result = mysqli_query($db, $query);
    mysqli_close($db);
    
    //Variable §wert wegen dem ".=" in der nachfolgenden while-Schleife
    $wert = NULL;
    
    while($row = mysqli_fetch_object($result)) {
		$infopunkt = $row->infopunkt;
        $einheit = $row->einheit;
        $wert .= '[Date.UTC(' . $row->zeit . '), ' . $row->wert . '], ' . "\n";
        } 
?>
<html>
    <head>
	    <title><?php echo $infopunkt . ' ' . $get_interval . 'h'; ?></title>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <meta name="author" content="Felix Horn">
        <link rel="stylesheet" type="text/css" href="../styles.css">
	    <script type="text/javascript" src="/js/jquery.min.js"></script>
		<script type="text/javascript">
		    $(function () { 
			    Highcharts.setOptions({
            	    lang: { 		
                        months: ['Januar', 'Februar', 'März', 'April', 'Mai', 'Juni', 'Juli', 'August', 'September', 'Oktober', 'November', 'Dezember'],
                        weekdays: ['Sonntag', 'Montag', 'Dienstag', 'Mittwoch', 'Donnerstag', 'Freitag', 'Samstag']
                }
            });
            $('#container').highcharts({
				    chart: {
					    type: 'spline'
				    },
				    title: {
					    text: '<?php echo $infopunkt . ' ' . $get_interval . 'h'; ?>'
				    },
				    xAxis: {
					    type: 'datetime'
				    },
				    yAxis: {
					    title: {
						    text: '<?php echo $einheit; ?>'
					    },
					    plotLines: [{
                            value: 0,
                            width: 2,
                            color: '#808080'
                        }]
				    },
				    series: [{
					    name: 'Wert',
					    data: [<?php echo $wert; ?>],
					    marker:
						    {
						    symbol: 'circle',
						    enabled: false,
						    states:
							    {
							    hover:
								    {
								    symbol: 'circle',
								    enabled: true,
								    radius: 5
								    }
							    }
						    }
						    }]
			    });
		    });
	    </script>
    </head>

    <body>
        <script type="text/javascript" src="/js/highcharts.js"></script>
	    <script type="text/javascript" src="/js/themes/grid.js"></script>
	    <div id="container" style="width:1200px; height:600px; margin:10px auto 10px"></div>
    </body>
</html>
