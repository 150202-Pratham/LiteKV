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


// Deserialization function 

int kv_record_deserialize(const uint8_t *buf , size_t buf_len , kv_record *out){

	if(buf_len < 8 ){

		return -1 ;

	}

	uint32_t key_len ;
	uint32_t val_len ;

	memcpy(&key_len , buf , 4) ;
	memcpy(&val_len , buf+4 , 4) ;

	size_t total_len = 4+4+key_len+val_len ;

	if(buf_len < total_len ){
		return -1 ;

	}

	out->key_len = key_len ;
	out->val_len = val_len ;

	out->key = malloc(key_len) ;
	out->value = malloc(val_len) ;

	if(out->key == NULL || out->value == NULL ){

		free(out->key) ;
		free(out->value) ;

		return -1 ;
	}

	memcpy(out->key , buf+8 , key_len) ;
	memcpy(out->value, buf+8+key_len , val_len) ;

	return 0 ;
}


