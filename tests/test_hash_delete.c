#include "kv_hash.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {

    kv_hash_table table = {0};


    /*
     * Insert a key.
     */
    assert(
        kv_hash_put(
            &table,
            (uint8_t *)"name",
            4,
            (uint8_t *)"Pratham",
            7
        ) == 0
    );

    printf("Test 1 PASSED: Insert\n");


    /*
     * Delete the key.
     */
    assert(
        kv_hash_delete(
            &table,
            (uint8_t *)"name",
            4
        ) == 0
    );

    printf("Test 2 PASSED: Delete\n");


    /*
     * Make sure the key no longer exists.
     */
    uint8_t *value = NULL;
    uint32_t value_len = 0;

    assert(
        kv_hash_get(
            &table,
            (uint8_t *)"name",
            4,
            &value,
            &value_len
        ) == -1
    );

    printf("Test 3 PASSED: Key no longer exists\n");


    printf("\nHash DELETE tests PASSED\n");

    return 0;
}