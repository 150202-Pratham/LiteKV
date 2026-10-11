#include "kv.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    const char *path = "tests/delete_test.wal";

    unlink(path);

    kv_db *db = kv_open(path);
    assert(db != NULL);

    const uint8_t key[] = "name";
    const uint8_t value[] = "Pratham";

    // Insert the key.
    assert(kv_put(db, key, 4, value, 7) == 0);

    // Delete the key.
    assert(kv_delete(db, key, 4) == 0);

    // Verify that the key no longer exists.
    uint8_t *stored_value = NULL;
    uint32_t stored_len = 0;

    int result = kv_get(db, key, 4,
                        &stored_value, &stored_len);

    assert(result == -1);
    assert(stored_value == NULL);

    assert(kv_close(db) == 0);

    unlink(path);

    printf("kv_delete tests PASSED\n");

    return 0;
}