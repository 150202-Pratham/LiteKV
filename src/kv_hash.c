
#include "kv_hash.h"

#include <stdlib.h>
#include <string.h>
#define FNV_OFFSET 2166136261u
#define FNV_PRIME 16777619u


uint32_t kv_hash(const uint8_t *key , size_t key_len){
    // Initial Algorithm Start state
    uint32_t hash = FNV_OFFSET ;
    
    // Loop through the Key
    for(size_t i = 0 ; i< key_len ; i++){

    //    XOR mixes the current byte into the hash
        hash ^= key[i] ;

        // this spreads the effect of the bytes through the hash value ;
        hash *= FNV_PRIME ;
    }

    return hash ;

}

// int kv_hash_put(kv_hash_table *table , const uint8_t *key , uint32_t key_len , const uint8_t *value , uint32_t value_len){

//     /*
//      Validate Input
//     */

//     if(table == NULL || key==NULL || key_len == 0){
//         return -1 ;

//     }

//     /* Caluclate Bucket Index*/

//     size_t index = kv_hash(key , key_len) % KV_TABLE_SIZE ;

//     kv_entry *current = table->buckets[index] ;

//     /*Search for Existing key */

//     while(current != NULL ){

//         if(current->key_len == key_len && memcmp(current->key, key , key_len)==0){

//             /*
//             Key already exists , update its value
//             */

//             uint8_t *new_value = malloc(value_len >0 ? value_len : 1) ;

//             if(new_value == NULL){
//                 return -1 ;

//             }

//             if(value_len > 0){

//                 memcpy(new_value , value , value_len) ;

//             }

//             free(current->value) ;

//             current->value = new_value ;
//             current->value_len = value_len ;

//             return 0 ;
//         }

//         current = current->next ;

//         /*
//      * Key doesn't exist.
//      *
//      * Create a new entry.
//      */
//     kv_entry *entry =
//         malloc(
//             sizeof(kv_entry)
//         );

//     if (entry == NULL) {
//         return -1;
//     }


//     /*
//      * Allocate memory for key.
//      */
//     entry->key =
//         malloc(
//             key_len
//         );


//     /*
//      * Allocate memory for value.
//      */
//     entry->value =
//         malloc(
//             value_len > 0
//             ? value_len
//             : 1
//         );


//     if (entry->key == NULL ||
//         entry->value == NULL) {

//         free(entry->key);
//         free(entry->value);
//         free(entry);

//         return -1;
//     }


//     /*
//      * Copy key.
//      */
//     memcpy(
//         entry->key,
//         key,
//         key_len
//     );


//     /*
//      * Copy value.
//      */
//     if (value_len > 0) {

//         memcpy(
//             entry->value,
//             value,
//             value_len
//         );
//     }


//     entry->key_len =
//         key_len;

//     entry->value_len =
//         value_len;


//     /*
//      * Insert at the beginning
//      * of the bucket's linked list.
//      */
//     entry->next =
//         table->buckets[index];

//     table->buckets[index] =
//         entry;


//     return 0;

//     }
// } 


int kv_hash_put(
    kv_hash_table *table,
    const uint8_t *key,
    uint32_t key_len,
    const uint8_t *value,
    uint32_t value_len
) {

    /*
     * Validate input.
     */
    if (table == NULL ||
        key == NULL ||
        key_len == 0) {

        return -1;
    }


    /*
     * Calculate bucket index.
     */
    size_t index =
        kv_hash(key, key_len) % KV_TABLE_SIZE;


    /*
     * Start searching this bucket.
     */
    kv_entry *current =
        table->buckets[index];


    /*
     * Search for an existing key.
     */
    while (current != NULL) {

        if (current->key_len == key_len &&
            memcmp(current->key, key, key_len) == 0) {

            /*
             * Key already exists.
             * Update its value.
             */

            uint8_t *new_value =
                malloc(value_len > 0 ? value_len : 1);

            if (new_value == NULL) {
                return -1;
            }

            if (value_len > 0) {
                memcpy(new_value, value, value_len);
            }

            free(current->value);

            current->value = new_value;
            current->value_len = value_len;

            return 0;
        }

        current = current->next;
    }


    /*
     * Key does not exist.
     *
     * Create a new entry.
     */
    kv_entry *entry =
        malloc(sizeof(kv_entry));

    if (entry == NULL) {
        return -1;
    }


    /*
     * Allocate memory for key.
     */
    entry->key =
        malloc(key_len);


    /*
     * Allocate memory for value.
     */
    entry->value =
        malloc(value_len > 0 ? value_len : 1);


    /*
     * Check allocation.
     */
    if (entry->key == NULL ||
        entry->value == NULL) {

        free(entry->key);
        free(entry->value);
        free(entry);

        return -1;
    }


    /*
     * Copy key.
     */
    memcpy(
        entry->key,
        key,
        key_len
    );


    /*
     * Copy value.
     */
    if (value_len > 0) {

        memcpy(
            entry->value,
            value,
            value_len
        );
    }


    /*
     * Store lengths.
     */
    entry->key_len =
        key_len;

    entry->value_len =
        value_len;


    /*
     * Insert the new entry
     * at the beginning of
     * the bucket's linked list.
     */
    entry->next =
        table->buckets[index];

    table->buckets[index] =
        entry;


    return 0;
}

int kv_hash_get(
    kv_hash_table *table,
    const uint8_t *key,
    uint32_t key_len,
    uint8_t **value,
    uint32_t *value_len
) {

    /*
     * Validate input.
     */
    if (table == NULL ||
        key == NULL ||
        key_len == 0 ||
        value == NULL ||
        value_len == NULL) {

        return -1;
    }


    /*
     * Calculate the bucket index.
     */
    size_t index =
        kv_hash(key, key_len) % KV_TABLE_SIZE;


    /*
     * Start from the first entry
     * in this bucket.
     */
    kv_entry *current =
        table->buckets[index];


    /*
     * Search through the bucket.
     */
    while (current != NULL) {

        /*
         * Check whether this is
         * the key we are looking for.
         */
        if (current->key_len == key_len &&
            memcmp(current->key, key, key_len) == 0) {

            /*
             * We found the key.
             */
            
            *value_len =
                current->value_len;


            /*
             * Allocate memory for
             * the returned value.
             */
            *value =
                malloc(
                    current->value_len > 0
                    ? current->value_len
                    : 1
                );


            /*
             * Check allocation.
             */
            if (*value == NULL) {
                return -1;
            }


            /*
             * Copy the stored value
             * into the newly allocated memory.
             */
            if (current->value_len > 0) {

                memcpy(
                    *value,
                    current->value,
                    current->value_len
                );
            }


            return 0;
        }


        /*
         * Key didn't match.
         *
         * Move to the next entry
         * in the linked list.
         */
        current =
            current->next;
    }


    /*
     * We reached the end of the
     * bucket without finding the key.
     */
    return -1;
}

int kv_hash_delete(
    kv_hash_table *table,
    const uint8_t *key,
    uint32_t key_len
){

    /*
     * Validate input.
     */
    if (table == NULL ||
        key == NULL ||
        key_len == 0) {

        return -1;
    }


    /*
     * Calculate the bucket.
     */
    size_t index =
        kv_hash(key, key_len) % KV_TABLE_SIZE;


    /*
     * Start at the first node.
     */
    kv_entry *current =
        table->buckets[index];


    /*
     * Keep track of the previous node.
     */
    kv_entry *previous = NULL;


    /*
     * Search the linked list.
     */
    while (current != NULL) {

        /*
         * Check whether this is
         * the key we want.
         */
        if (current->key_len == key_len &&
            memcmp(current->key, key, key_len) == 0) {

            /*
             * If previous is NULL,
             * current is the first node.
             */
            if (previous == NULL) {

                table->buckets[index] =
                    current->next;

            } else {

                /*
                 * Skip the current node.
                 */
                previous->next =
                    current->next;
            }


            /*
             * Free memory belonging
             * to the deleted entry.
             */
            free(current->key);
            free(current->value);
            free(current);


            return 0;
        }


        /*
         * Move both pointers forward.
         */
        previous = current;
        current = current->next;
    }


    /*
     * Key was not found.
     */
    return -1;
}

// this Function is Entirely used to release all its allocated memory

void kv_hash_free(kv_hash_table *table) {

    if (table == NULL) {
        return;
    }

    for (size_t i = 0; i < KV_TABLE_SIZE; i++) {

        kv_entry *current = table->buckets[i];

        while (current != NULL) {

            kv_entry *next = current->next;

            free(current->key);
            free(current->value);
            free(current);

            current = next;
        }

        table->buckets[i] = NULL;
    }
}



