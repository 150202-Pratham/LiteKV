#include "kv.h"
#include "kv_wal.h"
#include "kv_replay.h"
#include "kv_record.h"

#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

kv_db *kv_open(const char *path)
{
    // Validate the database path.
    if (path == NULL) {
        return NULL;
    }

    // Allocate memory for the database object.
    kv_db *db = calloc(1, sizeof(kv_db));

    if (db == NULL) {
        return NULL;
    }

    // Initialize the WAL file for writing.
    db->wal_fd = kv_wal_open(path);

    if (db->wal_fd == -1) {
        free(db);
        return NULL;
    }

    // Open the WAL separately for reading.
    int read_fd = open(path, O_RDONLY);

    if (read_fd == -1) {
        kv_wal_close(db->wal_fd);
        free(db);
        return NULL;
    }

    // Replay WAL records into the in-memory hash table.
    int result = kv_replay(read_fd, &db->table);

    // Close the temporary read-only file descriptor.
    close(read_fd);

    // Handle replay failure.
    if (result != 0) {
        kv_hash_free(&db->table);
        kv_wal_close(db->wal_fd);
        free(db);
        return NULL;
    }

    // Return the successfully opened database.
    return db;
}

int kv_close(kv_db *db)
{
    // Step 1: Validate the database pointer.
    if (db == NULL) {
        return -1;
    }

    // Step 2: Free all entries in the in-memory hash table.
    kv_hash_free(&db->table);

    // Step 3: Close the WAL file.
    kv_wal_close(db->wal_fd);

    // Step 4: Release the database object itself.
    free(db);

    return 0;
}

int kv_put(kv_db *db,
           const uint8_t *key,
           uint32_t key_len,
           const uint8_t *value,
           uint32_t value_len)
{
    // Validate the inputs.
    if (db == NULL || key == NULL || value == NULL) {
        return -1;
    }

    // Create a PUT record.
    kv_record record = {0};

    record.operation = KV_OP_PUT;
    record.key_len = key_len;
    record.val_len = value_len;
    record.key = (uint8_t *)key;
    record.value = (uint8_t *)value;

    // Serialize the record into bytes.
    size_t record_len = 0;

    uint8_t *buffer = kv_record_serialize(&record, &record_len);

    if (buffer == NULL) {
        return -1;
    }

    // Append the serialized record to the WAL.
    int result = kv_wal_append(db->wal_fd, buffer, record_len);

    // The serialized buffer is no longer needed.
    free(buffer);

    if (result != 0) {
        return -1;
    }

    // Update the in-memory hash table.
    result = kv_hash_put(&db->table,
                         key,
                         key_len,
                         value,
                         value_len);

    if (result != 0) {
        return -1;
    }

    // Report success.
    return 0;
}


int kv_get(kv_db *db,
           const uint8_t *key,
           uint32_t key_len,
           uint8_t **value,
           uint32_t *value_len)
{
    // Validate the inputs.
    if (db == NULL ||
        key == NULL ||
        value == NULL ||
        value_len == NULL) {
        return -1;
    }

    // Initialize the output parameters.
    *value = NULL;
    *value_len = 0;

    // Search the in-memory hash table.
    return kv_hash_get(&db->table,
                       key,
                       key_len,
                       value,
                       value_len);
}


int kv_delete(kv_db *db,
              const uint8_t *key,
              uint32_t key_len)
{
    // Validate the inputs.
    if (db == NULL || key == NULL) {
        return -1;
    }

    // Create a DELETE record.
    kv_record record = {0};

    record.operation = KV_OP_DELETE;
    record.key_len = key_len;
    record.val_len = 0;
    record.key = (uint8_t *)key;
    record.value = NULL;

    // Serialize the record.
    size_t record_len = 0;

    uint8_t *buffer = kv_record_serialize(&record, &record_len);

    if (buffer == NULL) {
        return -1;
    }

    // Persist the DELETE operation in the WAL.
    int result = kv_wal_append(db->wal_fd, buffer, record_len);

    free(buffer);

    if (result != 0) {
        return -1;
    }

    // Remove the key from the in-memory hash table.
    result = kv_hash_delete(&db->table, key, key_len);

    if (result != 0) {
        return -1;
    }

    return 0;
}