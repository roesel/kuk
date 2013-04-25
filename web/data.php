<?php 
$db_server = "127.0.0.1";
$db_prihljmeno = "kuk";
$db_heslo = "malina";
$db_nazev = "kuk";

$spojeni = mysql_connect($db_server, $db_prihljmeno, $db_heslo);
if (!$spojeni) die("Spojení s databází se bohužel nezdařilo."); 
mysql_select_db("$db_nazev"); 
mysql_query("SET NAMES 'utf8';");

$table = "speed";
$sql = mysql_query("SELECT    *
                    FROM      `speed`
                    ORDER BY  `time` DESC
                    LIMIT     1;");
$result = mysql_result($sql,0,'speed');
if (is_string($result)) { 
    print $result;
} else {
    print "0";
}
?>