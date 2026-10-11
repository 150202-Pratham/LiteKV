#include "kv.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    const char *path =
        "tests/integration/crc_corruption_test.wal";

    unlink(path);

    // Create a database and write a valid record.
    kv_db *db = kv_open(path);
    assert(db != NULL);

    const uint8_t key[] = "name";
    const uint8_t value[] = "Pratham";

    assert(kv_put(db, key, 4, value, 7) == 0);
    assert(kv_close(db) == 0);

    // Modify one byte in the stored value.
    int fd = open(path, O_RDWR);
    assert(fd != -1);

    // Record layout:
    // operation(4) + key_len(4) + val_len(4) + key(4)
    // The value begins at byte offset 16.
    assert(lseek(fd, 16, SEEK_SET) == 16);

    unsigned char byte;
    assert(read(fd, &byte, 1) == 1);

    byte ^= 0x01;

    assert(lseek(fd, 16, SEEK_SET) == 16);
    assert(write(fd, &byte, 1) == 1);

    close(fd);

    // Reopening must detect the invalid CRC.
    db = kv_open(path);
    assert(db == NULL);

    printf("INTEGRATION TEST 5 PASSED: CRC corruption detected\n");

    unlink(path);

    return 0;
}