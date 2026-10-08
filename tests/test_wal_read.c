#include "kv_wal.h"
#include "kv_record.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {

    const char *path =
        "tests/read_test.wal";


    /*
     * Create a test record.
     */
    kv_record record;

    record.key_len = 5;
    record.val_len = 5;

    record.key =
        (uint8_t *)"hello";

    record.value =
        (uint8_t *)"world";


    /*
     * Serialize the record.
     */
    size_t serialized_len;

    uint8_t *serialized =
        kv_record_serialize(
            &record,
            &serialized_len
        );

    assert(serialized != NULL);


    /*
     * Open WAL.
     */
    int fd =
        kv_wal_open(path);

    assert(fd >= 0);


    /*
     * Write record.
     */
    assert(
        kv_wal_append(
            fd,
            serialized,
            serialized_len
        ) == 0
    );


    kv_wal_close(fd);

    free(serialized);


    /*
     * Open the WAL again for reading.
     */
    fd =
        open(
            path,
            O_RDONLY
        );

    assert(fd >= 0);


    /*
     * Read one record.
     */
    uint8_t *data = NULL;
    size_t len = 0;

    int result =
        kv_wal_read(
            fd,
            &data,
            &len
        );

    assert(result == 0);

    printf(
        "Test 1 PASSED: Record read\n"
    );


    /*
     * Deserialize the record.
     */
    kv_record decoded;

    assert(
        kv_record_deserialize(
            data,
            len,
            &decoded
        ) == 0
    );

    printf(
        "Test 2 PASSED: Record deserialized\n"
    );


    /*
     * Verify key.
     */
    assert(decoded.key_len == 5);

    assert(
        memcmp(
            decoded.key,
            "hello",
            5
        ) == 0
    );


    /*
     * Verify value.
     */
    assert(decoded.val_len == 5);

    assert(
        memcmp(
            decoded.value,
            "world",
            5
        ) == 0
    );

    printf(
        "Test 3 PASSED: Key/value verified\n"
    );


    free(decoded.key);
    free(decoded.value);
    free(data);

    close(fd);

    return 0;
}