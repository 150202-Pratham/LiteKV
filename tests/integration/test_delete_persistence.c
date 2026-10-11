#include "kv.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    const char *path =
        "tests/integration/delete_persistence_test.wal";

    // Start with a clean WAL.
    unlink(path);

    const uint8_t key[] = "name";
    const uint8_t value[] = "Pratham";

    // Session 1: Open the database.
    kv_db *db = kv_open(path);
    assert(db != NULL);

    // Insert a key-value pair.
    assert(kv_put(db, key, 4, value, 7) == 0);

    // Delete the key.
    assert(kv_delete(db, key, 4) == 0);

    // Close the database.
    assert(kv_close(db) == 0);

    // Session 2: Reopen the database.
    db = kv_open(path);
    assert(db != NULL);

    // Verify that the deleted key remains absent.
    uint8_t *stored_value = NULL;
    uint32_t stored_len = 0;

    int result = kv_get(
        db,
        key,
        4,
        &stored_value,
        &stored_len
    );

    assert(result == -1);
    assert(stored_value == NULL);

    printf("Deleted key remained absent after reopening.\n");

    // Close and clean up.
    assert(kv_close(db) == 0);

    unlink(path);

    printf("INTEGRATION TEST 2 PASSED\n");

    return 0;
}