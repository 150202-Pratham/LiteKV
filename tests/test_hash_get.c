#include "kv_hash.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {

    kv_hash_table table = {0};


    /*
     * Insert a value first.
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
     * Variables that GET will fill.
     */
    uint8_t *value = NULL;
    uint32_t value_len = 0;


    /*
     * Retrieve the value.
     */
    assert(
        kv_hash_get(
            &table,
            (uint8_t *)"name",
            4,
            &value,
            &value_len
        ) == 0
    );

    printf("Test 2 PASSED: Get\n");


    /*
     * Verify the returned value.
     */
    assert(value_len == 7);

    assert(
        memcmp(
            value,
            "Pratham",
            7
        ) == 0
    );

    printf("Test 3 PASSED: Value verified\n");


    /*
     * Free memory allocated by GET.
     */
    free(value);


    printf("\nHash GET tests PASSED\n");

    return 0;
}