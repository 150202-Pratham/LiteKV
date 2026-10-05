#include "kv_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void) {

    kv_record rec;

    rec.key = (uint8_t *)"username";
    rec.key_len = 8;

    rec.value = (uint8_t *)"pratham";
    rec.val_len = 7;

    size_t buffer_len = 0;

    uint8_t *buffer =
        kv_record_serialize(&rec, &buffer_len);

    // Test 1: Check memory allocation

    assert(buffer != NULL);

    printf("Test 1 PASSED: Buffer allocated\n");

    // Test 2: Check serialized size

    assert(buffer_len == 23);

    printf("Test 2 PASSED: Correct buffer size\n");

    // Test 3: Check stored key length

    uint32_t stored_key_len;

    memcpy(&stored_key_len, buffer, 4);

    assert(stored_key_len == 8);

    printf("Test 3 PASSED: Correct key length\n");

    // Test 4: Check stored value length

    uint32_t stored_val_len;

    memcpy(&stored_val_len, buffer + 4, 4);

    assert(stored_val_len == 7);

    printf("Test 4 PASSED: Correct value length\n");

    // Test 5: Check actual key

    assert(memcmp(buffer + 8, "username", 8) == 0);

    printf("Test 5 PASSED: Correct key\n");

    // Test 6: Check actual value

    assert(memcmp(buffer + 16, "pratham", 7) == 0);

    printf("Test 6 PASSED: Correct value\n");

    free(buffer);

    printf("\nAll serialization tests passed!\n");

    return 0;
}

