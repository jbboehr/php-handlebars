<?php

function myprint($opcodes, $output = false) {
	/*
	$flags = defined('JSON_PRETTY_PRINT') ? constant('JSON_PRETTY_PRINT') : 0;
	if( $output ) {
		echo json_encode($opcodes, $flags);
	} else {
		return json_encode($opcodes, $flags);
	} */
	if( $output ) {
		print_r($opcodes);
	} else {
		return print_r($opcodes, true);
	}	
}
