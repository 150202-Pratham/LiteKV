#include "kv_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int main(void) {

    /*
     * ========================================
     * ORIGINAL RECORD
     * ========================================
     */

    kv_record original;

    original.key =
        (uint8_t *)"username";

    original.key_len = 8;

    original.value =
        (uint8_t *)"pratham";

    original.val_len = 7;


    /*
     * ========================================
     * TEST 1
     * Serialization
     * ========================================
     */

    size_t buffer_len = 0;

    uint8_t *buffer =
        kv_record_serialize(
            &original,
            &buffer_len
        );

    assert(buffer != NULL);

    printf(
        "Test 1 PASSED: Serialization successful\n"
    );


    /*
     * ========================================
     * TEST 2
     * Check serialized size
     *
     * 4 key_len
     * 4 val_len
     * 8 key
     * 7 value
     * 4 CRC
     *
     * Total = 27
     * ========================================
     */

    assert(buffer_len == 27);

    printf(
        "Test 2 PASSED: Correct serialized size\n"
    );


    /*
     * ========================================
     * TEST 3
     * Deserialization
     * ========================================
     */

    kv_record restored;

    int result =
        kv_record_deserialize(
            buffer,
            buffer_len,
            &restored
        );

    assert(result == 0);

    printf(
        "Test 3 PASSED: Deserialization successful\n"
    );


    /*
     * ========================================
     * TEST 4
     * Check lengths
     * ========================================
     */

    assert(
        restored.key_len ==
        original.key_len
    );

    assert(
        restored.val_len ==
        original.val_len
    );

    printf(
        "Test 4 PASSED: Lengths match\n"
    );


    /*
     * ========================================
     * TEST 5
     * Check key
     * ========================================
     */

    assert(
        memcmp(
            restored.key,
            original.key,
            original.key_len
        ) == 0
    );

    printf(
        "Test 5 PASSED: Key matches\n"
    );


    /*
     * ========================================
     * TEST 6
     * Check value
     * ========================================
     */

    assert(
        memcmp(
            restored.value,
            original.value,
            original.val_len
        ) == 0
    );

    printf(
        "Test 6 PASSED: Value matches\n"
    );


    /*
     * Free deserialized record.
     */
    free(restored.key);
    free(restored.value);


    /*
     * ========================================
     * TEST 7
     * Corruption Detection
     * ========================================
     *
     * Change one byte in the record.
     */

    buffer[10] ^= 0xFF;


    kv_record corrupted;

    int corruption_result =
        kv_record_deserialize(
            buffer,
            buffer_len,
            &corrupted
        );


    assert(corruption_result == -1);

    printf(
        "Test 7 PASSED: Corruption detected\n"
    );


    /*
     * ========================================
     * TEST 8
     * Truncated Record
     * ========================================
     *
     * Pretend the last 5 bytes
     * were never written.
     */

    int truncated_result =
        kv_record_deserialize(
            buffer,
            buffer_len - 5,
            &corrupted
        );


    assert(truncated_result == -1);

    printf(
        "Test 8 PASSED: Truncated record detected\n"
    );


    /*
     * Free serialized buffer.
     */
    free(buffer);


    printf(
        "\nAll tests passed!\n"
    );


    return 0;
}