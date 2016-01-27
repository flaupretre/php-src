--TEST--
str_ireplace() tests - Replace search string with array
--FILE--
<?php

function check_replace($search,$replace,&$subject)
{
$c=0;
echo "Input : $subject\n";
$subject=str_ireplace($search,$replace,$subject,$c);
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
Output: FgBCA10nBrAabV1aAaNBaFjAaaAaJzK?
Count: 8

Input : FgBCA10nBrAabV1aAaNBaFjAaaAaJzK?
Output: FgBCB10nBrCAbV110AabNB1FjaaAaJzK?
Count: 8

Input : FgBCB10nBrCAbV110AabNB1FjaaAaJzK?
Output: FgBCB10nBrCBbV110CAbNB1Fj10Aab1JzK?
Count: 7

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
Output: a
Count: 1

------ subject == search ------

Input : abc
Output: a
Count: 1

------ length(subject) < length(search) ------

Input : abc
Output: abc
Count: 0

==Done==

