<?php
/*
  +----------------------------------------------------------------------+
  | PCS extension <http://pcs.tekwire.net>                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2015 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Francois Laupretre <francois@tekwire.net>                    |
  +----------------------------------------------------------------------+
*/
//=============================================================================

namespace PCS\Embedded {

function build($ipath, $opath, $prefix=null)
{
	include(dirname(__FILE__).'/FileArray.php');
	include(dirname(__FILE__).'/File.php');

	#---

	if (is_null($prefix)) {
		$a=explode('.',basename($opath));
		$prefix=$a[0];
	}

	$a=new \PCS\Embedded\FileArray();
	
	$a->register($ipath, '');

	$output = $a->dump($prefix);
	file_put_contents($opath, $output);
}

//=============================================================================
}
?>
