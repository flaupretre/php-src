--TEST--
str_replace() tests - Replace search string with array
--FILE--
<?php

function check_replace($search,$replace,&$subject)
{
$c=0;
echo "Input : $subject\n";
$subject=str_replace($search,$replace,$subject,$c);
echo "Output: $subject\n";
echo "Count: $c\n";
echo "\n";
}

//------

function info($msg)
{
echo "------ $msg ------\n\n";
}

//------
/* Check 'needle=string/replace=array' case */

info("Basic tests"); //------------

$search='a';
$repl=array('B','C','A',10,'Aab', false, true, null);
$subject='FgAaaanBraVaaAaAaNBaFjAaaAaJzK?';
check_replace($search,$repl,$subject);
check_replace($search,$repl,$subject);
check_replace($search,$repl,$subject);

info("Empty replace array"); //------------

$repl=array();
$subject='FgAnBraVAAaNBaFjAaaAaJzK?';
check_replace($search,$repl,$subject);

info("length(subject) == length(search)"); //------------

$repl=array("a","ab","bc","cd");
$subject="abc";
$search="def";
check_replace($search,$repl,$subject);

info("subject == strtolower(search)"); //------------

$subject="abc";
$search="ABC";
check_replace($search,$repl,$subject);

info("subject == search"); //------------

$subject="abc";
$search="abc";
check_replace($search,$repl,$subject);

info("length(subject) < length(search)"); //------------

$subject="abc";
$search="abcd";
check_replace($search,$repl,$subject);
?>
==Done==
--EXPECTF--	

------ Basic tests ------

Input : FgAaaanBraVaaAaAaNBaFjAaaAaJzK?
Output: FgABCAnBr10VAabA1ANBaFjAaaAaJzK?
Count: 8

Input : FgABCAnBr10VAabA1ANBaFjAaaAaJzK?
Output: FgABCAnBr10VABbA1ANBCFjAA10AAabJzK?
Count: 5

Input : FgABCAnBr10VABbA1ANBCFjAA10AAabJzK?
Output: FgABCAnBr10VABbA1ANBCFjAA10AABbJzK?
Count: 1

------ Empty replace array ------

Input : FgAnBraVAAaNBaFjAaaAaJzK?
Output: FgAnBraVAAaNBaFjAaaAaJzK?
Count: 0

------ length(subject) == length(search) ------

Input : abc
Output: abc
Count: 0

------ subject == strtolower(search) ------

Input : abc
Output: abc
Count: 0

------ subject == search ------

Input : abc
Output: a
Count: 1

------ length(subject) < length(search) ------

Input : abc
Output: abc
Count: 0

==Done==
