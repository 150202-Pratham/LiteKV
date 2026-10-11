#include "kv.h"
#include "kv_wal.h"
#include "kv_replay.h"

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