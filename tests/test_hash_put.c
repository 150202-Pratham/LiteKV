#include "kv_hash.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {

    /*
     * Create an empty hash table.
     */
    kv_hash_table table = {0};


    /*
     * Insert name.
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


    printf(
        "Test 1 PASSED: Insert\n"
    );


    /*
     * Find the bucket manually.
     */
    size_t index =
        kv_hash(
            (uint8_t *)"name",
            4
        ) % KV_TABLE_SIZE;


    /*
     * Entry should exist.
     */
    kv_entry *entry =
        table.buckets[index];


    assert(entry != NULL);


    /*
     * Verify key.
     */
    assert(
        entry->key_len == 4
    );

    assert(
        memcmp(
            entry->key,
            "name",
            4
        ) == 0
    );


    /*
     * Verify value.
     */
    assert(
        entry->value_len == 7
    );

    assert(
        memcmp(
            entry->value,
            "Pratham",
            7
        ) == 0
    );


    printf(
        "Test 2 PASSED: Key/value verified\n"
    );


    /*
     * Test update.
     */
    assert(
        kv_hash_put(
            &table,
            (uint8_t *)"name",
            4,
            (uint8_t *)"Rahul",
            5
        ) == 0
    );


    /*
     * Same bucket.
     */
    entry =
        table.buckets[index];


    /*
     * Make sure we didn't create
     * another node.
     */
    assert(entry != NULL);

    assert(entry->next == NULL);


    /*
     * Verify updated value.
     */
    assert(
        entry->value_len == 5
    );

    assert(
        memcmp(
            entry->value,
            "Rahul",
            5
        ) == 0
    );


    printf(
        "Test 3 PASSED: Update\n"
    );


    /*
     * Clean up.
     */
    free(entry->key);
    free(entry->value);
    free(entry);


    printf(
        "\nHash PUT tests PASSED\n"
    );


    return 0;
}