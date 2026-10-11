#include "kv.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    const char *path =
        "tests/integration/update_persistence_test.wal";

    unlink(path);

    const uint8_t key[] = "name";
    const uint8_t old_value[] = "Pratham";
    const uint8_t new_value[] = "Developer";

    // Session 1: Open the database.
    kv_db *db = kv_open(path);
    assert(db != NULL);

    // Insert the initial value.
    assert(kv_put(db, key, 4, old_value, 7) == 0);

    // Update the existing key.
    assert(kv_put(db, key, 4, new_value, 9) == 0);

    // Close the database.
    assert(kv_close(db) == 0);

    // Session 2: Reopen the database.
    db = kv_open(path);
    assert(db != NULL);

    // Retrieve the updated value.
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
    assert(stored_len == 9);
    assert(memcmp(stored_value, new_value, 9) == 0);

    printf("Updated value recovered: %.*s\n",
           (int)stored_len,
           (char *)stored_value);

    free(stored_value);

    assert(kv_close(db) == 0);

    unlink(path);

    printf("INTEGRATION TEST 3 PASSED\n");

    return 0;
}