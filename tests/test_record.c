#include "kv_record.h"

#include <stdio.h>

int main( void ){

	kv_record rec ;

	rec.key = (uint8_t *)"username" ;
	rec.key_len = 8 ;

	rec.value = (uint8_t *)"pratham" ;
	rec.val_len = 7 ;

	printf("Key: %.*s\n", rec.key_len , rec.key) ;
	printf("Value: %.*s\n" , rec.val_len , rec.value) ;

	printf("Key length: %u\n" , rec.key_len) ;
	printf("Value length: %u\n", rec.val_len) ;

	return 0 ;
}


