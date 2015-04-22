﻿<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<?php
    include '../dbconnect.php';
    
    $get_messwert_id = $_GET['messwert_id'];
    $get_interval = $_GET['interval'];

    $query = 'SELECT
                CONCAT_WS( \' \', messwert.tw1, messwert.tw2, messwert.tw3) AS infopunkt,
                messwert_archiv_1h.min,
                messwert_archiv_1h.avg,
                messwert_archiv_1h.max,
                messwert.einheit,
                DATE_FORMAT(messwert_archiv_1h.zeitstempel, \'%Y, %m-1, %e, %k\') AS zeit
              FROM
                messwert_archiv_1h
              INNER JOIN
                messwert ON messwert_archiv_1h.messwert_id = messwert.id
              WHERE messwert_archiv_1h.zeitstempel >= DATE_SUB(NOW(), INTERVAL ' . ($get_interval+1) . ' HOUR)
                AND messwert_archiv_1h.messwert_id = ' . $get_messwert_id . ' ORDER BY messwert_archiv_1h.id';

    $result = mysqli_query($db, $query);
    mysqli_close($db);
    
    //Variablen wegen dem ".=" in der nachfolgenden while-Schleife
    $wert_avg = NULL;
    $wert_min = NULL;
    $wert_max = NULL;
    
    while($row = mysqli_fetch_object($result)) {
		$infopunkt = $row->infopunkt;
        $einheit = $row->einheit;
        $wert_avg .= '[Date.UTC(' . $row->zeit . '), ' . $row->avg . '], ' . "\n";
        $wert_min .= '[Date.UTC(' . $row->zeit . '), ' . $row->min . '], ' . "\n";
        $wert_max .= '[Date.UTC(' . $row->zeit . '), ' . $row->max . '], ' . "\n";
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
					    name: 'Minimum',
					    data: [<?php echo $wert_min; ?>],
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
						    },{
					    name: 'Durchschnitt',
					    data: [<?php echo $wert_avg; ?>],
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
						    },{
					    name: 'Maximum',
					    data: [<?php echo $wert_max; ?>],
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
