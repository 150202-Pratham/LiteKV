#include "kv_hash.h"

#include <assert.h>
#include <stdio.h>

int main(void) {

    kv_hash_table table = {0};

    assert(
        kv_hash_put(
            &table,
            (const uint8_t *)"name",
            4,
            (const uint8_t *)"Pratham",
            7
        ) == 0
    );

    assert(
        kv_hash_put(
            &table,
            (const uint8_t *)"age",
            3,
            (const uint8_t *)"21",
            2
        ) == 0
    );

    kv_hash_free(&table);

    for (size_t i = 0; i < KV_TABLE_SIZE; i++) {
        assert(table.buckets[i] == NULL);
    }

    printf("Hash FREE tests PASSED\n");

    return 0;
}

