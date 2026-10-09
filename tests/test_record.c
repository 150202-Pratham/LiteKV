// #include "kv_record.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <assert.h>


// int main(void) {

//     /*
//      * ========================================
//      * ORIGINAL RECORD
//      * ========================================
//      */

//     kv_record original;

//     original.key =
//         (uint8_t *)"username";

//     original.key_len = 8;

//     original.value =
//         (uint8_t *)"pratham";

//     original.val_len = 7;


//     /*
//      * ========================================
//      * TEST 1
//      * Serialization
//      * ========================================
//      */

//     size_t buffer_len = 0;

//     uint8_t *buffer =
//         kv_record_serialize(
//             &original,
//             &buffer_len
//         );

//     assert(buffer != NULL);

//     printf(
//         "Test 1 PASSED: Serialization successful\n"
//     );


//     /*
//      * ========================================
//      * TEST 2
//      * Check serialized size
//      *
//      * 4 key_len
//      * 4 val_len
//      * 8 key
//      * 7 value
//      * 4 CRC
//      *
//      * Total = 27
//      * ========================================
//      */

//     assert(buffer_len == 27);

//     printf(
//         "Test 2 PASSED: Correct serialized size\n"
//     );


//     /*
//      * ========================================
//      * TEST 3
//      * Deserialization
//      * ========================================
//      */

//     kv_record restored;

//     int result =
//         kv_record_deserialize(
//             buffer,
//             buffer_len,
//             &restored
//         );

//     assert(result == 0);

//     printf(
//         "Test 3 PASSED: Deserialization successful\n"
//     );


//     /*
//      * ========================================
//      * TEST 4
//      * Check lengths
//      * ========================================
//      */

//     assert(
//         restored.key_len ==
//         original.key_len
//     );

//     assert(
//         restored.val_len ==
//         original.val_len
//     );

//     printf(
//         "Test 4 PASSED: Lengths match\n"
//     );


//     /*
//      * ========================================
//      * TEST 5
//      * Check key
//      * ========================================
//      */

//     assert(
//         memcmp(
//             restored.key,
//             original.key,
//             original.key_len
//         ) == 0
//     );

//     printf(
//         "Test 5 PASSED: Key matches\n"
//     );


//     /*
//      * ========================================
//      * TEST 6
//      * Check value
//      * ========================================
//      */

//     assert(
//         memcmp(
//             restored.value,
//             original.value,
//             original.val_len
//         ) == 0
//     );

//     printf(
//         "Test 6 PASSED: Value matches\n"
//     );


//     /*
//      * Free deserialized record.
//      */
//     free(restored.key);
//     free(restored.value);


//     /*
//      * ========================================
//      * TEST 7
//      * Corruption Detection
//      * ========================================
//      *
//      * Change one byte in the record.
//      */

//     buffer[10] ^= 0xFF;


//     kv_record corrupted;

//     int corruption_result =
//         kv_record_deserialize(
//             buffer,
//             buffer_len,
//             &corrupted
//         );


//     assert(corruption_result == -1);

//     printf(
//         "Test 7 PASSED: Corruption detected\n"
//     );


//     /*
//      * ========================================
//      * TEST 8
//      * Truncated Record
//      * ========================================
//      *
//      * Pretend the last 5 bytes
//      * were never written.
//      */

//     int truncated_result =
//         kv_record_deserialize(
//             buffer,
//             buffer_len - 5,
//             &corrupted
//         );


//     assert(truncated_result == -1);

//     printf(
//         "Test 8 PASSED: Truncated record detected\n"
//     );


//     /*
//      * Free serialized buffer.
//      */
//     free(buffer);


//     printf(
//         "\nAll tests passed!\n"
//     );


//     return 0;
// }


#include "kv_record.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Test 1:
 * PUT record.
 */
void test_put_record(void) {

    kv_record record;

    record.operation = KV_OP_PUT;

    record.key_len = 4;
    record.val_len = 7;

    record.key =
        (uint8_t *)"name";

    record.value =
        (uint8_t *)"Pratham";


    /*
     * Serialize.
     */
    size_t len;

    uint8_t *buffer =
        kv_record_serialize(
            &record,
            &len
        );

    assert(buffer != NULL);


    /*
     * Expected size:
     *
     * operation = 4
     * key_len   = 4
     * val_len   = 4
     * key       = 4
     * value     = 7
     * CRC       = 4
     *
     * Total = 27
     */
    assert(len == 27);


    /*
     * Deserialize.
     */
    kv_record decoded;

    int result =
        kv_record_deserialize(
            buffer,
            len,
            &decoded
        );

    assert(result == 0);


    /*
     * Check operation.
     */
    assert(
        decoded.operation == KV_OP_PUT
    );


    /*
     * Check key length.
     */
    assert(
        decoded.key_len == 4
    );


    /*
     * Check value length.
     */
    assert(
        decoded.val_len == 7
    );


    /*
     * Check key.
     */
    assert(
        memcmp(
            decoded.key,
            "name",
            4
        ) == 0
    );


    /*
     * Check value.
     */
    assert(
        memcmp(
            decoded.value,
            "Pratham",
            7
        ) == 0
    );


    printf(
        "Test 1 PASSED: PUT record\n"
    );


    free(decoded.key);
    free(decoded.value);
    free(buffer);
}


/*
 * Test 2:
 * DELETE record.
 */
void test_delete_record(void) {

    kv_record record;

    record.operation = KV_OP_DELETE;

    record.key_len = 4;
    record.val_len = 0;

    record.key =
        (uint8_t *)"name";

    record.value = NULL;


    /*
     * Serialize.
     */
    size_t len;

    uint8_t *buffer =
        kv_record_serialize(
            &record,
            &len
        );

    assert(buffer != NULL);


    /*
     * Deserialize.
     */
    kv_record decoded;

    int result =
        kv_record_deserialize(
            buffer,
            len,
            &decoded
        );

    assert(result == 0);


    /*
     * Check operation.
     */
    assert(
        decoded.operation == KV_OP_DELETE
    );


    /*
     * Check key.
     */
    assert(
        decoded.key_len == 4
    );


    assert(
        memcmp(
            decoded.key,
            "name",
            4
        ) == 0
    );


    /*
     * Delete has no value.
     */
    assert(
        decoded.val_len == 0
    );


    printf(
        "Test 2 PASSED: DELETE record\n"
    );


    free(decoded.key);
    free(decoded.value);
    free(buffer);
}


/*
 * Test 3:
 * Corrupted record.
 */
void test_corrupted_record(void) {

    kv_record record;

    record.operation = KV_OP_PUT;

    record.key_len = 3;
    record.val_len = 3;

    record.key =
        (uint8_t *)"abc";

    record.value =
        (uint8_t *)"xyz";


    size_t len;

    uint8_t *buffer =
        kv_record_serialize(
            &record,
            &len
        );

    assert(buffer != NULL);


    /*
     * Corrupt one byte.
     */
    buffer[12] ^= 0xFF;


    /*
     * Deserialization must fail.
     */
    kv_record decoded;

    int result =
        kv_record_deserialize(
            buffer,
            len,
            &decoded
        );

    assert(result == -1);


    printf(
        "Test 3 PASSED: Corruption detected\n"
    );


    free(buffer);
}


/*
 * Test 4:
 * Truncated record.
 */
void test_truncated_record(void) {

    kv_record record;

    record.operation = KV_OP_PUT;

    record.key_len = 4;
    record.val_len = 5;

    record.key =
        (uint8_t *)"name";

    record.value =
        (uint8_t *)"hello";


    size_t len;

    uint8_t *buffer =
        kv_record_serialize(
            &record,
            &len
        );

    assert(buffer != NULL);


    /*
     * Pretend the last byte
     * was never written.
     */
    size_t truncated_len =
        len - 1;


    kv_record decoded;

    int result =
        kv_record_deserialize(
            buffer,
            truncated_len,
            &decoded
        );


    assert(result == -1);


    printf(
        "Test 4 PASSED: Truncated record detected\n"
    );


    free(buffer);
}


/*
 * Test 5:
 * Invalid operation.
 */
void test_invalid_operation(void) {

    kv_record record;

    record.operation = KV_OP_PUT;

    record.key_len = 3;
    record.val_len = 3;

    record.key =
        (uint8_t *)"abc";

    record.value =
        (uint8_t *)"xyz";


    size_t len;

    uint8_t *buffer =
        kv_record_serialize(
            &record,
            &len
        );

    assert(buffer != NULL);


    /*
     * Change operation to
     * an invalid value.
     */
    uint32_t invalid_operation = 999;

    memcpy(
        buffer,
        &invalid_operation,
        4
    );


    /*
     * CRC is now also invalid,
     * so deserialization must fail.
     */
    kv_record decoded;

    int result =
        kv_record_deserialize(
            buffer,
            len,
            &decoded
        );


    assert(result == -1);


    printf(
        "Test 5 PASSED: Invalid operation detected\n"
    );


    free(buffer);
}


/*
 * Test 6:
 * PUT with empty value.
 */
void test_empty_value(void) {

    kv_record record;

    record.operation = KV_OP_PUT;

    record.key_len = 4;
    record.val_len = 0;

    record.key =
        (uint8_t *)"name";

    record.value = NULL;


    size_t len;

    uint8_t *buffer =
        kv_record_serialize(
            &record,
            &len
        );

    assert(buffer != NULL);


    kv_record decoded;

    int result =
        kv_record_deserialize(
            buffer,
            len,
            &decoded
        );


    assert(result == 0);


    assert(
        decoded.operation == KV_OP_PUT
    );

    assert(
        decoded.key_len == 4
    );

    assert(
        decoded.val_len == 0
    );


    assert(
        memcmp(
            decoded.key,
            "name",
            4
        ) == 0
    );


    printf(
        "Test 6 PASSED: Empty value\n"
    );


    free(decoded.key);
    free(decoded.value);
    free(buffer);
}


int main(void) {

    printf(
        "\n===== LiteKV Record Tests =====\n\n"
    );


    test_put_record();

    test_delete_record();

    test_corrupted_record();

    test_truncated_record();

    test_invalid_operation();

    test_empty_value();


    printf(
        "\n===== ALL TESTS PASSED =====\n\n"
    );


    return 0;
}