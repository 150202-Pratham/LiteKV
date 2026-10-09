#ifndef KV_HASH_H
#define KV_HASH_H

#include <stdint.h>
#include <stdfef.h> 

#define KV_TABLE_SIZE 16

/*This is the Structure of Node that we are storing inside each bucket
   bucket[0] -> Node(key -> vlaue -> next) , Node ()
*/
typedef struct kv_entry{

    uint8_t *key ;
    uint32_t key_len ;

    uint8_t *value;
    uint32_t value_len ;

    struct kv_entry *next ;
}kv_entry ;

/*
 this Structure is basically used for now for implementing the bucket behaviour
 
*/
typedef struct{
     
    kv_entry *buckets[KV_TABLE_SIZE] ;

}kv_hash_table ;

#endif 