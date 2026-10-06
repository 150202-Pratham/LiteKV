#ifndef KV_RECORD_H
#define KV_RECORD_H

#include <stdint.h>
#include <stddef.h>

typedef struct{

	uint32_t key_len ;
	uint32_t val_len ;
	uint8_t *key;
	uint8_t *value;

}kv_record ;


// Serialize Function Declaration 
uint8_t *kv_record_serialize(const kv_record *rec , size_t *out_len);

// Deserialization Function Declaration
int kv_record_deserialize(const uint8_t *buf , size_t buf_len , kv_record *out) ;

// CRC 32 CheckSum Function Declaration

uint32_t kv_crc32(const uint8_t *data , size_t len) ;


#endif
