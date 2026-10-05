#include "kv_record.h"

#include <stdlib.h>
#include <string.h>

uint8_t *kv_record_serialize(const kv_record *rec , size_t *out_len){

	size_t total_len = 4 + 4 + rec->key_len + rec->val_len ;

	uint8_t *buffer = malloc(total_len) ;

	if(buffer == NULL){

		return NULL ;
	}

	uint8_t *p = buffer ;

	memcpy(p, &rec->key_len,4) ;
	p+=4 ;

	memcpy(p, &rec->val_len,4);
	p+=4 ;

        memcpy(p, rec->key, rec->key_len);
        p += rec->key_len;

    	memcpy(p, rec->value, rec->val_len);

    	*out_len = total_len;

	return buffer ;
}


