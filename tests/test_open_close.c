#include "kv.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    const char *path = "tests/open_close.wal";

    // Remove any old test WAL so this test starts clean.
    unlink(path);

    // Test opening the database.
    kv_db *db = kv_open(path);

    assert(db != NULL);

    // Test closing the database.
    int result = kv_close(db);

    assert(result == 0);

    // Remove the test WAL.
    unlink(path);

    printf("Open/close tests PASSED\n");

    return 0;
}