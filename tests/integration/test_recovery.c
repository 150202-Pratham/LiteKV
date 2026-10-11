#include "kv.h"
#include "kv_wal.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    const char *path = "tests/integration/recovery_test.wal";

    // Start with a clean WAL.
    unlink(path);

    // Create a valid database and persist a record.
    kv_db *db = kv_open(path);
    assert(db != NULL);

    const uint8_t key[] = "name";
    const uint8_t value[] = "Pratham";

    assert(kv_put(db, key, 4, value, 7) == 0);
    assert(kv_close(db) == 0);

    // Append an intentionally incomplete record header.
    int fd = open(path, O_WRONLY | O_APPEND);
    assert(fd != -1);

    const uint8_t incomplete_header[] = {1, 0, 0, 0};

    assert(write(fd,
                 incomplete_header,
                 sizeof(incomplete_header)) ==
           (ssize_t)sizeof(incomplete_header));

    close(fd);

    // Reopening should fail with the current recovery implementation.
    db = kv_open(path);

    assert(db == NULL);

    printf("INTEGRATION TEST 4 PASSED: incomplete WAL rejected\n");

    unlink(path);

    return 0;
}