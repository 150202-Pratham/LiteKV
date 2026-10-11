#include "kv.h"

#include<stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    const char *path = "tests/put_test.wal";

    // Start with a clean test WAL.
    unlink(path);

    // Open the database.
    kv_db *db = kv_open(path);

    assert(db != NULL);

    // Insert a key-value pair.
    const uint8_t key[] = "name";
    const uint8_t value[] = "Pratham";

    int result = kv_put(db,
                        key, 4,
                        value, 7);

    assert(result == 0);

    // Verify that the value exists in the hash table.
    uint8_t *stored_value = NULL;
    uint32_t stored_len = 0;

    result = kv_hash_get(&db->table,
                         key, 4,
                         &stored_value,
                         &stored_len);

    assert(result == 0);
    assert(stored_len == 7);
    assert(memcmp(stored_value, value, 7) == 0);

    free(stored_value);

    // Close the database.
    assert(kv_close(db) == 0);

    // Clean up the test file.
    unlink(path);

    printf("kv_put test PASSED\n");

    return 0;
}