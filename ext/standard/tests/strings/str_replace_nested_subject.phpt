--TEST--
str_replace() tests - Multi-level nested array as subject
--FILE--
<?php

function check_replace($search,$replace,&$subject)
{
$c=0;
echo "Input :\n";
var_dump($subject);
$subject=str_replace($search,$replace,$subject,$c);
echo "Output:\n";
var_dump($subject);
echo "Count: $c\n";
echo "\n";
}

//------

function info($msg)
{
echo "------ $msg ------\n\n";
}

//------

info("Basic tests"); //------------

$search='a';
$repl='<ara>';
$subject = $sbase = array('alpha',
			array('beta',
				array('gamma', 10, false, null, 'mamamamamama')
				),
			true);

check_replace($search,$repl,$subject);
check_replace($search,$repl,$subject);

info("Using search string with replace array"); //------------

$subject = $sbase;
$repl = array('a', 'b', 'ca', 'za');

check_replace($search,$repl,$subject);

info("Empty replace array"); //------------

$repl=array();
$subject=$sbase;

check_replace($search,$repl,$subject);

?>
==Done==
--EXPECTF--	
------ Basic tests ------

Input :
array(3) {
  [0]=>
  string(5) "alpha"
  [1]=>
  array(2) {
    [0]=>
    string(4) "beta"
    [1]=>
    array(5) {
      [0]=>
      string(5) "gamma"
      [1]=>
      int(10)
      [2]=>
      bool(false)
      [3]=>
      NULL
      [4]=>
      string(12) "mamamamamama"
    }
  }
  [2]=>
  bool(true)
}
Output:
array(3) {
  [0]=>
  string(13) "<ara>lph<ara>"
  [1]=>
  array(2) {
    [0]=>
    string(8) "bet<ara>"
    [1]=>
    array(5) {
      [0]=>
      string(13) "g<ara>mm<ara>"
      [1]=>
      string(2) "10"
      [2]=>
      string(0) ""
      [3]=>
      string(0) ""
      [4]=>
      string(36) "m<ara>m<ara>m<ara>m<ara>m<ara>m<ara>"
    }
  }
  [2]=>
  string(1) "1"
}
Count: 11

Input :
array(3) {
  [0]=>
  string(13) "<ara>lph<ara>"
  [1]=>
  array(2) {
    [0]=>
    string(8) "bet<ara>"
    [1]=>
    array(5) {
      [0]=>
      string(13) "g<ara>mm<ara>"
      [1]=>
      string(2) "10"
      [2]=>
      string(0) ""
      [3]=>
      string(0) ""
      [4]=>
      string(36) "m<ara>m<ara>m<ara>m<ara>m<ara>m<ara>"
    }
  }
  [2]=>
  string(1) "1"
}
Output:
array(3) {
  [0]=>
  string(29) "<<ara>r<ara>>lph<<ara>r<ara>>"
  [1]=>
  array(2) {
    [0]=>
    string(16) "bet<<ara>r<ara>>"
    [1]=>
    array(5) {
      [0]=>
      string(29) "g<<ara>r<ara>>mm<<ara>r<ara>>"
      [1]=>
      string(2) "10"
      [2]=>
      string(0) ""
      [3]=>
      string(0) ""
      [4]=>
      string(84) "m<<ara>r<ara>>m<<ara>r<ara>>m<<ara>r<ara>>m<<ara>r<ara>>m<<ara>r<ara>>m<<ara>r<ara>>"
    }
  }
  [2]=>
  string(1) "1"
}
Count: 22

------ Using search string with replace array ------

Input :
array(3) {
  [0]=>
  string(5) "alpha"
  [1]=>
  array(2) {
    [0]=>
    string(4) "beta"
    [1]=>
    array(5) {
      [0]=>
      string(5) "gamma"
      [1]=>
      int(10)
      [2]=>
      bool(false)
      [3]=>
      NULL
      [4]=>
      string(12) "mamamamamama"
    }
  }
  [2]=>
  bool(true)
}
Output:
array(3) {
  [0]=>
  string(5) "alphb"
  [1]=>
  array(2) {
    [0]=>
    string(4) "beta"
    [1]=>
    array(5) {
      [0]=>
      string(5) "gammb"
      [1]=>
      string(2) "10"
      [2]=>
      string(0) ""
      [3]=>
      string(0) ""
      [4]=>
      string(14) "mambmcamzamama"
    }
  }
  [2]=>
  string(1) "1"
}
Count: 9

------ Empty replace array ------

Input :
array(3) {
  [0]=>
  string(5) "alpha"
  [1]=>
  array(2) {
    [0]=>
    string(4) "beta"
    [1]=>
    array(5) {
      [0]=>
      string(5) "gamma"
      [1]=>
      int(10)
      [2]=>
      bool(false)
      [3]=>
      NULL
      [4]=>
      string(12) "mamamamamama"
    }
  }
  [2]=>
  bool(true)
}
Output:
array(3) {
  [0]=>
  string(5) "alpha"
  [1]=>
  array(2) {
    [0]=>
    string(4) "beta"
    [1]=>
    array(5) {
      [0]=>
      string(5) "gamma"
      [1]=>
      string(2) "10"
      [2]=>
      string(0) ""
      [3]=>
      string(0) ""
      [4]=>
      string(12) "mamamamamama"
    }
  }
  [2]=>
  string(1) "1"
}
Count: 0

==Done==
