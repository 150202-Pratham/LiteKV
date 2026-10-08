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
        "tests/replay_test.wal";


    /*
     * Open WAL for writing.
     */
    int fd =
        kv_wal_open(path);

    assert(fd >= 0);


    /*
     * Create first record.
     */
    kv_record record1;

    record1.key_len = 4;
    record1.val_len = 7;

    record1.key =
        (uint8_t *)"name";

    record1.value =
        (uint8_t *)"Pratham";


    /*
     * Serialize first record.
     */
    size_t len1;

    uint8_t *data1 =
        kv_record_serialize(
            &record1,
            &len1
        );

    assert(data1 != NULL);


    /*
     * Write first record.
     */
    assert(
        kv_wal_append(
            fd,
            data1,
            len1
        ) == 0
    );


    free(data1);


    /*
     * Create second record.
     */
    kv_record record2;

    record2.key_len = 3;
    record2.val_len = 2;

    record2.key =
        (uint8_t *)"age";

    record2.value =
        (uint8_t *)"21";


    /*
     * Serialize second record.
     */
    size_t len2;

    uint8_t *data2 =
        kv_record_serialize(
            &record2,
            &len2
        );

    assert(data2 != NULL);


    /*
     * Write second record.
     */
    assert(
        kv_wal_append(
            fd,
            data2,
            len2
        ) == 0
    );


    free(data2);


    /*
     * Close WAL.
     */
    kv_wal_close(fd);


    printf("Write phase PASSED\n");


    /*
     * Reopen WAL for reading.
     */
    fd =
        open(
            path,
            O_RDONLY
        );

    assert(fd >= 0);


    /*
     * Replay first record.
     */
    uint8_t *buffer = NULL;
    size_t len = 0;

    int result =
        kv_wal_read(
            fd,
            &buffer,
            &len
        );

    assert(result == 0);


    kv_record decoded;

    assert(
        kv_record_deserialize(
            buffer,
            len,
            &decoded
        ) == 0
    );


    printf(
        "Recovered: %.*s = %.*s\n",
        (int)decoded.key_len,
        decoded.key,
        (int)decoded.val_len,
        decoded.value
    );


    free(decoded.key);
    free(decoded.value);
    free(buffer);


    /*
     * Replay second record.
     */
    buffer = NULL;
    len = 0;

    result =
        kv_wal_read(
            fd,
            &buffer,
            &len
        );

    assert(result == 0);


    assert(
        kv_record_deserialize(
            buffer,
            len,
            &decoded
        ) == 0
    );


    printf(
        "Recovered: %.*s = %.*s\n",
        (int)decoded.key_len,
        decoded.key,
        (int)decoded.val_len,
        decoded.value
    );


    free(decoded.key);
    free(decoded.value);
    free(buffer);


    /*
     * There should be no more records.
     */
    result =
        kv_wal_read(
            fd,
            &buffer,
            &len
        );

    assert(result == 1);


    printf(
        "Replay completed successfully\n"
    );


    close(fd);

    return 0;
}