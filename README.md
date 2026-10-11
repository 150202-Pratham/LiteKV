# LiteKV

**LiteKV** is a lightweight, embeddable key-value storage library
written in **C**. It persists operations in an append-only Write-Ahead
Log (WAL), maintains an in-memory hash-table index, and reconstructs the
index by replaying the log when the database is reopened.

The project demonstrates core storage-engine concepts: binary record
encoding, CRC32 checksums, append-only persistence, in-memory indexing,
log replay, and recovery from an incomplete final write.

## Project Status

The core implementation and automated test suite are complete for the
current project scope.

Implemented: - Binary record serialization and deserialization - CRC32
validation to detect record corruption - Append-only WAL storage -
In-memory hash-table index - `PUT`, `GET`, and `DELETE` operations - WAL
replay when opening a database - Persistence across close/reopen
cycles - Recovery by truncating an incomplete trailing WAL record - Unit
and integration tests - Makefile targets for building, testing, and
cleaning

A complete record with an invalid checksum or invalid structure is
treated as corruption; LiteKV does not silently discard a corrupted
complete record.

## Problem Statement

Applications with small persistence requirements may not need a full
database system. LiteKV is a learning-oriented implementation of the
basic storage-engine building blocks behind key-value databases.

Its goal is to provide a small C library that stores byte-oriented keys
and values, persists mutations to a log, and reconstructs its in-memory
state from that log after reopening.

## Goals

-   Implement the project in C with a modular design.
-   Persist changes using an append-only log.
-   Detect corrupted records using CRC32.
-   Rebuild the in-memory index by replaying persisted records.
-   Support insertion, retrieval, update, and deletion.
-   Recover from an incomplete final WAL record without losing earlier
    valid records.
-   Verify behavior with component-level and integration tests.

## Specifications

### Functional requirements

  -----------------------------------------------------------------------
  Requirement                         Description
  ----------------------------------- -----------------------------------
  Open                                Open or create a database WAL and
                                      reconstruct the in-memory index.

  Put                                 Append a key-value update to the
                                      WAL and update the in-memory index.

  Get                                 Retrieve the current value
                                      associated with a key.

  Delete                              Persist a deletion operation and
                                      remove the key from the in-memory
                                      index.

  Replay                              Read WAL records in order and apply
                                      them to rebuild the current state.

  Integrity check                     Validate record structure and CRC32
                                      during deserialization.

  Tail recovery                       Detect an incomplete final record
                                      and truncate the WAL to the last
                                      valid record boundary.

  Close                               Release the in-memory index and
                                      close the WAL descriptor.
  -----------------------------------------------------------------------

### Record format

Each serialized record contains a fixed-size header, key and value
bytes, and a CRC32 checksum.

  ------------------------------------------------------------------------
  Field                                         Size Purpose
  --------------------- ---------------------------- ---------------------
  Operation                                  4 bytes Identifies an
                                                     operation such as
                                                     `PUT` or `DELETE`.

  Key length                                 4 bytes Number of bytes in
                                                     the key.

  Value length                               4 bytes Number of bytes in
                                                     the value.

  Key                                       Variable Raw key bytes.

  Value                                     Variable Raw value bytes;
                                                     empty for a delete
                                                     record.

  CRC32                                      4 bytes Detects accidental
                                                     record corruption.
  ------------------------------------------------------------------------

The fixed header occupies 12 bytes, followed by the key, value, and
4-byte checksum. Records are binary and length-delimited; keys and
values do not need to be null-terminated strings.

### Persistence and recovery behavior

1.  A mutation is serialized and appended to the WAL.
2.  The WAL append operation synchronizes the file using `fsync`.
3.  The in-memory hash table is updated during normal operation.
4.  On open, LiteKV reads the WAL from the beginning and replays
    complete records in order.
5.  If the final record is incomplete, LiteKV truncates the file to the
    end of the last valid record and opens the database using the
    recovered state.
6.  If a complete record is malformed or fails CRC validation, opening
    fails rather than silently accepting corrupted data.

LiteKV is a compact educational storage engine, not a replacement for a
production database. Transactions, concurrent access guarantees, WAL
compaction, snapshots, replication, and a query language are not
currently implemented.

## Design and Architecture

``` text
Application
    |
    v
Public API (kv.c / kv.h)
    |
    +----------------------+----------------------+
    |                                             |
    v                                             v
Write-Ahead Log                              In-memory Hash Table
(kv_wal.c)                                   (kv_hash.c)
    |
    v
Record Encoding / CRC32
(kv_record.c)

Database open:
WAL -> Record Reader -> Deserializer / CRC Validation
    -> Replay (kv_replay.c) -> In-memory Hash Table
    -> Recover incomplete trailing record if necessary
```

### Modules

-   **`kv.c` / `kv.h`** --- Public database lifecycle and key-value
    operations.
-   **`kv_record.c` / `kv_record.h`** --- Record
    serialization/deserialization, operation validation, and CRC32
    integrity checks.
-   **`kv_wal.c` / `kv_wal.h`** --- WAL open, append, read, close, and
    incomplete-tail truncation.
-   **`kv_hash.c` / `kv_hash.h`** --- In-memory key-value hash table.
-   **`kv_replay.c` / `kv_replay.h`** --- Replays WAL records into the
    hash table and tracks the last valid record boundary.

### Data flow

-   **Write path:** API call → serialize record → append and synchronize
    WAL → update in-memory index.
-   **Read path:** API call → look up the key in the in-memory hash
    table.
-   **Startup path:** open WAL → replay complete records → validate
    checksums → recover an incomplete trailing record when present →
    expose the reconstructed database.

## Repository Structure

``` text
LiteKV/
├── include/
│   ├── kv.h
│   ├── kv_hash.h
│   ├── kv_record.h
│   ├── kv_replay.h
│   └── kv_wal.h
├── src/
│   ├── kv.c
│   ├── kv_hash.c
│   ├── kv_record.c
│   ├── kv_replay.c
│   └── kv_wal.c
├── tests/
│   ├── test_record.c
│   ├── test_wal.c
│   ├── test_wal_read.c
│   ├── test_hash_put.c
│   ├── test_hash_get.c
│   ├── test_hash_delete.c
│   ├── test_hash_free.c
│   ├── test_open_close.c
│   ├── test_put.c
│   ├── test_get.c
│   ├── test_delete.c
│   ├── test_replay.c
│   ├── test_replay_apply.c
│   └── integration/
│       ├── test_persistence.c
│       ├── test_delete_persistence.c
│       ├── test_update_persistence.c
│       ├── test_recovery.c
│       └── test_crc_corruption.c
├── Makefile
└── README.md
```

## Requirements

-   GCC or another compatible C compiler
-   GNU Make
-   A POSIX-compatible environment for `read`, `write`, `lseek`,
    `ftruncate`, and `fsync`

The project has been developed and tested in Linux.

## Build and Test

Run these commands from the LiteKV project directory.

### Build the static library

``` bash
make
```

The build target creates `build/liblitekv.a`.

### Compile and run the test suite

``` bash
make test
```

The suite covers record integrity, WAL append/read behavior, hash-table
operations, database API operations, replay, persistence across
restarts, updates, deletion persistence, incomplete-tail recovery, and
CRC corruption detection.

### Remove generated build files and test executables

``` bash
make clean
```

## Testing Coverage

Tests cover: - `PUT` and `DELETE` record serialization/deserialization -
CRC mismatch, truncated-record, and invalid-operation detection - WAL
append and read behavior - Hash-table insertion, lookup, update,
deletion, and cleanup - Database open and close - Public `PUT`, `GET`,
and `DELETE` behavior - WAL replay and replay application - Persistence
after closing and reopening the database - Update and deletion
persistence - Recovery from an incomplete trailing WAL record -
Rejection of a WAL containing a corrupted complete record

Run `make test` to execute the suite against the current checkout.

## Scope and Limitations

LiteKV is designed to demonstrate storage-engine fundamentals with a
small codebase. Current limitations include: - The hash-table index is
in memory and is reconstructed from the WAL on open. - There is no WAL
compaction or snapshot mechanism, so the log can grow over time. -
Concurrent access and multi-process coordination are not implemented. -
Transactions and multi-record atomic operations are not implemented. -
Recovery handles an incomplete trailing record; it does not
automatically repair arbitrary corruption in a complete record. - The
project is not claimed to be production-hardened database software.

## Possible Future Improvements

-   WAL compaction and snapshot support
-   More extensive crash-injection and fault-injection tests
-   Stronger validation for unusually large record lengths
-   Improved handling of interrupted system calls and partial writes
-   Concurrency support with explicit locking
-   Benchmarks for throughput, latency, and memory usage

These are possible extensions, not features claimed by the current
implementation.

## Learning Outcomes

LiteKV provides practical experience with: - Binary serialization and
deserialization - Checksums and data-integrity validation - Append-only
logging and durable writes - Hash-table indexing - Log replay and
startup recovery - Memory ownership and resource cleanup in C - Unit and
integration testing - Modular C project organization and Makefile-based
builds

## License

No license has been specified yet. Add a `LICENSE` file before
presenting the project as open source for reuse by others.
