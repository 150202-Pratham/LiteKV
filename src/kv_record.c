#include "kv_record.h"

#include <stdlib.h>
#include <string.h>

uint8_t *kv_record_serialize(const kv_record *rec , size_t *out_len){
	
	if (rec == NULL || out_len == NULL) {
        return NULL;
    }


	size_t body_len = 4 + 4 + rec->key_len + rec->val_len;

    size_t total_len = body_len + 4 ;

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


	uint32_t crc = kv_crc32(buffer, body_len);

    memcpy(buffer + body_len, &crc, 4);

    *out_len = total_len;

	return buffer ;
}


// Deserialization function 

int kv_record_deserialize(const uint8_t *buf , size_t buf_len , kv_record *out){

	if(buf == NULL || out == NULL  ){

		return -1 ;

	}

	/*
	we need at least:

	4 bytes key_len 
	4 bytes val_len 
	4 bytes crc
	however, we first need 8 bytes to even know the record size
	*/

	if( buf_len < 8 ){

		return -1 ;
	}

	/*
		Read Key length and value length 
	*/
	uint32_t key_len ;
	uint32_t val_len ;

	memcpy(&key_len , buf , 4) ;
	memcpy(&val_len , buf+4 , 4) ;

	/*
	  Calculate size of data
      before CRC.
	*/
	size_t body_len = 4+4+key_len+val_len ;


	/*
	Complete record also contains
       4 bytes of CRC.
	*/
    
	size_t total_len = body_len + 4 ;

	if(buf_len < total_len ){
		return -1 ;

	}

	 /*
      Read CRC stored in the record.
     */
    uint32_t stored_crc;

    memcpy(
        &stored_crc,
        buf + body_len,
        4
    );


    /*
      Calculate CRC again from the data.
     */
    uint32_t calculated_crc =
        kv_crc32(
            buf,
            body_len
        );


    /*
      If the checksums don't match,
      the record is corrupted.
     */
    if (stored_crc != calculated_crc) {
        return -1;
    }


    /*
       Store lengths in output structure.
     */

	out->key_len = key_len ;
	out->val_len = val_len ;

	out->key = malloc(key_len) ;
	out->value = malloc(val_len>0 ? val_len : 1 ) ;

	if(out->key == NULL || out->value == NULL ){

		free(out->key) ;
		free(out->value) ;

		out-> key = NULL ;
		out->value = NULL ;


		return -1 ;
	}


	/*
     * Copy key.
     *
     * Key starts after:
     *
     * 4 bytes key_len
     * 4 bytes val_len
     */
    if (key_len > 0) {
	 	memcpy(out->key , buf+8 , key_len) ;
	}

	/*
     * Copy value.
     *
     * Value starts after:
     *
     * 8 bytes header
     * + key_len
     */
    if (val_len > 0) {

		memcpy(out->value, buf+8+key_len , val_len) ;
	}
	return 0 ;
}


uint32_t kv_crc32(const uint8_t *data , size_t len){

	uint32_t crc = 0xFFFFFFFFu ;

	for(size_t i = 0 ; i < len ; i++){

		crc ^= data[i] ;

		for(int bit = 0 ; bit < 8 ; bit++){

			uint32_t mask = -(crc & 1u) ;

			crc = (crc >> 1) ^ (0xEDB88320u & mask) ;

		}
	}

	return ~crc ;
}


