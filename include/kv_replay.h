#ifndef KV_REPLAY_H
#define KV_REPLAY_H

#include "kv_hash.h"
#include <sys/types.h>
/*
 * Read WAL records and rebuild the hash table.
 *
 * Returns:
 *   0  on success
 *  -1  on failure
 */
int kv_replay(int fd, kv_hash_table *table , off_t *valid_end);

#endif