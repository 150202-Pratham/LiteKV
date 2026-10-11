#include "kv_replay.h"

#include "kv_wal.h"
#include "kv_record.h"

#include <stdlib.h>

int kv_replay(int fd, kv_hash_table *table) {

    if (fd < 0 || table == NULL) {
        return -1;
    }

    while (1) {

        /*
         * Read the next serialized record.
         */
        uint8_t *buffer = NULL;
        size_t len = 0;

        int result = kv_wal_read(
            fd,
            &buffer,
            &len
        );

        /*
         * End of WAL: replay completed.
         */
        if (result == 1) {
            return 0;
        }

        /*
         * Reading failed or record is incomplete.
         */
        if (result == -1) {
            return -1;
        }

        /*
         * Deserialize and verify CRC.
         */
        kv_record record = {0};

        result = kv_record_deserialize(
            buffer,
            len,
            &record
        );

        free(buffer);

        if (result != 0) {
            return -1;
        }

        /*
         * Apply the operation to the hash table.
         */
        if (record.operation == KV_OP_PUT) {

            result = kv_hash_put(
                table,
                record.key,
                record.key_len,
                record.value,
                record.val_len
            );

        } else if (record.operation == KV_OP_DELETE) {

            /*
             * A key might already be absent.
             * That does not prevent replay.
             */
            result = kv_hash_delete(
                table,
                record.key,
                record.key_len
            );

            if (result == -1) {
                result = 0;
            }

        } else {
            result = -1;
        }

        /*
         * Release memory allocated by deserialization.
         */
        free(record.key);
        free(record.value);

        if (result != 0) {
            return -1;
        }
    }
}