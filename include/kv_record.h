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


#endif
