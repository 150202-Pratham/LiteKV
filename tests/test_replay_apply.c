#include "kv_hash.h"
#include "kv_record.h"
#include "kv_replay.h"
#include "kv_wal.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void append_put(
    int fd,
    const char *key,
    const char *value
) {
    kv_record record = {0};

    record.operation = KV_OP_PUT;
    record.key_len = (uint32_t)strlen(key);
    record.val_len = (uint32_t)strlen(value);
    record.key = (uint8_t *)key;
    record.value = (uint8_t *)value;

    size_t len = 0;

    uint8_t *buffer = kv_record_serialize(
        &record,
        &len
    );

    assert(buffer != NULL);
    assert(kv_wal_append(fd, buffer, len) == 0);

    free(buffer);
}

int main(void) {

    const char *path = "tests/replay_apply.wal";

    unlink(path);

    /*
     * Write two PUT records.
     */
    int fd = kv_wal_open(path);
    assert(fd >= 0);

    append_put(fd, "name", "Pratham");
    append_put(fd, "age", "21");

    kv_wal_close(fd);

    /*
     * Start with an empty hash table.
     */
    kv_hash_table table = {0};

    /*
     * Open the WAL for reading.
     */
    fd = open(path, O_RDONLY);
    assert(fd >= 0);

    /*
     * Replay all records into the hash table.
     */
    assert(kv_replay(fd, &table) == 0);

    kv_wal_close(fd);

    /*
     * Verify that replay restored "name".
     */
    uint8_t *value = NULL;
    uint32_t value_len = 0;

    assert(
        kv_hash_get(
            &table,
            (const uint8_t *)"name",
            4,
            &value,
            &value_len
        ) == 0
    );

    assert(value_len == 7);
    assert(memcmp(value, "Pratham", 7) == 0);

    free(value);

    /*
     * Verify that replay restored "age".
     */
    value = NULL;
    value_len = 0;

    assert(
        kv_hash_get(
            &table,
            (const uint8_t *)"age",
            3,
            &value,
            &value_len
        ) == 0
    );

    assert(value_len == 2);
    assert(memcmp(value, "21", 2) == 0);

    free(value);

    kv_hash_free(&table);

    unlink(path);

    printf("Replay test PASSED\n");

    return 0;
}