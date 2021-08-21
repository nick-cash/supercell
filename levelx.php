<?php
/*Level X Parser*/
$fp = fopen("php://stdin", "r");

$name = trim(fgets($fp));
list($pstart_x, $pstart_y) = explode(',', trim(fgets($fp)));

$player = array();
$player['type'] = 'player';
$player['spawn interval']=0;
$player['number to spawn']=1;
$player['start spawn time']=0;
$player['start position']=array('x' => floatval($pstart_x), 'y' => floatval($pstart_y));
$player['velocity']=array('x' => 0, 'y' => 0);
$enemylist = array();
while($line = fgetcsv($fp,8000,';'))
{
   if(count($line)<=3) continue;
   $pattern = $centery = $height = $duration = $additional_def = '';
   list($pattern, $centery, $height, $duration, $additional_def) = $line;
   $definition_counter = 0;

   if($duration > 0)
   {
      /*Set defaults*/
      $additional = array(
         'XV' => -10,
         'YV' => 0,
         'XP' => 800,
         'COUNT'=> 9,
         'SX' => 20,
         'ENEMY'=>'enemy'
         );
      
      $additional_definitions = explode(',', $additional_def);
      
      foreach($additional_definitions as $def)
      {
         list($key, $value) = explode(':', $def);
         $additional[$key] = $value;
      }
   
      if($additional['COUNT'] <= 0)
      {
         $additional = 9;
      }
   
      $timeslice = floor($duration/strlen($pattern));
      $j = strlen($pattern);
      
      for($i = 0; $i < $j; $i++)
      {
      /*
              "enemy1": {
            "type": "enemy",
            "spawn interval": 500,
            "number to spawn": 8000,
            "start spawn time": 200,
            "spawn vector": {
                "x":    0,
                "y":    0
            },
            "start position": {
                "x":    340,
                "y":    200
            },
            "velocity": {
                "x":    10.5,
                "y":    2.5
            }
        }
      */
         $enemy = array();
         $enemy['type']=$additional['ENEMY'];
         $enemy['spawn interval']=floor($timeslice/$additional['COUNT']);
         $enemy['number to spawn']=floor($additional['COUNT']);
         $enemy['start spawn time']=$i*$timeslice;
         $enemy['velocity'] = array('x' => floatval($additional['XV']), 'y' => floatval($additional['YV']));
         
         switch($pattern[$i])
         {
            case 'X':
            case 'x':
               
               $enemy1 = $enemy;
               $enemy2 = $enemy;
               
               $enemy1['start position'] = 
               array(
                  'x' => floatval($additional['XP']),
                  'y' => $centery - $height/2
               );
               
               $enemy2['start position'] = 
               array(
                  'x' => floatval($additional['XP']),
                  'y' => $centery + $height/2
               );
               
               $enemy1['spawn vector'] = 
               array(
                  'x' => floatval($additional['SX']),
                  'y' => $height/$additional['COUNT']
               );

               $enemy2['spawn vector'] = 
               array(
                  'x' => floatval($additional['SX']),
                  'y' => ($height/$additional['COUNT']) * -1
               );               
               
               $enemylist[]=$enemy1;
               $enemylist[]=$enemy2;
               
               break;
            case '-':
               $enemy['start position']=array('x' => floatval($additional['XP']), floatval($centery));
               $enemy['spawn vector']=array('x' => floatval($additional['SX']), 'y' => 0);
               $enemylist[] = $enemy;
            case '|':
               $enemy['start position']=array('x' => floatval($additional['XP']), floatval($centery));
               $enemy['spawn vector']=array('x' => 0, 'y' => floatval($additional['SX']));
               $enemylist[] = $enemy;               
            case '>':
               //Not implemented yet.
            case '<':
               //Not implemented yet.
            default:
               break;
         }
      }
   }
}

$output = array('name' => $name);
$output['proto sprites']['player'] = $player;
foreach($enemylist as $index=>$value)
{
   $c = $index+1;
   $output['proto sprites']["enemy{$c}"] = $value;
}

echo trim(str_replace("{", "\n{", json_encode($output)));

