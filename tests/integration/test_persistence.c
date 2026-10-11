#include "kv.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    const char *path = "tests/integration/persistence_test.wal";

    unlink(path);

    const uint8_t key[] = "name";
    const uint8_t value[] = "Pratham";

    // Session 1: Open the database.
    kv_db *db = kv_open(path);
    assert(db != NULL);

    // Store the key-value pair.
    assert(kv_put(db, key, 4, value, 7) == 0);

    // Close the database.
    assert(kv_close(db) == 0);

    // Session 2: Reopen the same database.
    db = kv_open(path);
    assert(db != NULL);

    // Retrieve the previously stored value.
    uint8_t *stored_value = NULL;
    uint32_t stored_len = 0;

    int result = kv_get(
        db,
        key,
        4,
        &stored_value,
        &stored_len
    );

    assert(result == 0);
    assert(stored_len == 7);
    assert(memcmp(stored_value, value, 7) == 0);

    printf("Recovered value: %.*s\n",
           (int)stored_len,
           (char *)stored_value);

    free(stored_value);

    // Close the database and clean up.
    assert(kv_close(db) == 0);

    unlink(path);

    printf("INTEGRATION TEST 1 PASSED\n");

    return 0;
}