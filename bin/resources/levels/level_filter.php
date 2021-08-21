<?php
$fp = fopen("php://stdin", "r");

$maxspawntime = 1800000;

$jsonX = '';
while($r = fread($fp, 20000))
{
   $jsonX .= $r;
}

$json = json_decode($jsonX);

$v = get_object_vars($json->{"proto sprites"});
$keys = array_keys($v);

foreach($keys as $key)
{
   if($json->{"proto sprites"}->{$key}->{"start spawn time"} > $maxspawntime)
   {
      unset($json->{"proto sprites"}->{$key});
   }
}

echo trim(str_replace("{", "\n{", json_encode($json)));