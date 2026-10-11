#ifndef KV_H
#define KV_H

#include "kv_hash.h"

typedef struct {

    int wal_fd;

    kv_hash_table table;

} kv_db;

kv_db *kv_open(const char *path);

int kv_close(kv_db *db);

#endif