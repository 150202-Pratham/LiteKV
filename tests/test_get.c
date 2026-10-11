#include "kv.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    const char *path = "tests/get_test.wal";

    // Start with a clean WAL.
    unlink(path);

    // Open the database.
    kv_db *db = kv_open(path);

    assert(db != NULL);

    const uint8_t key[] = "name";
    const uint8_t value[] = "Pratham";

    // Insert a key-value pair.
    assert(kv_put(db, key, 4, value, 7) == 0);

    // Retrieve the value.
    uint8_t *stored_value = NULL;
    uint32_t stored_len = 0;

    int result = kv_get(db,
                        key,
                        4,
                        &stored_value,
                        &stored_len);

    assert(result == 0);
    assert(stored_len == 7);
    assert(memcmp(stored_value, value, 7) == 0);

    printf("Retrieved value: %.*s\n",
           (int)stored_len,
           (char *)stored_value);

    free(stored_value);

    // Verify that a missing key returns an error.
    const uint8_t missing_key[] = "unknown";

    result = kv_get(db,
                    missing_key,
                    7,
                    &stored_value,
                    &stored_len);

    assert(result == -1);
    assert(stored_value == NULL);

    // Close the database.
    assert(kv_close(db) == 0);

    // Remove the test WAL.
    unlink(path);

    printf("kv_get tests PASSED\n");

    return 0;
}