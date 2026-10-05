#include "kv_record.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {

    kv_record rec;

    rec.key = (uint8_t *)"username";
    rec.key_len = 8;

    rec.value = (uint8_t *)"pratham";
    rec.val_len = 7;

    size_t buffer_len;

    uint8_t *buffer =
        kv_record_serialize(&rec, &buffer_len);

    if (buffer == NULL) {
        printf("Serialization failed\n");
        return 1;
    }

    printf("Serialized size: %zu bytes\n", buffer_len);

    printf("Bytes:\n");

    for (size_t i = 0; i < buffer_len; i++) {
        printf("%02X ", buffer[i]);
    }

    printf("\n");

    free(buffer);

    return 0;
}

