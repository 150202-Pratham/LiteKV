# litekv

## Crash-Safe, Embeddable Key-Value Storage Library in C

---

## 1. Project Description

**litekv** is a lightweight, embeddable key-value storage library written entirely in **C**.

The project aims to provide a simple way for a C program to store and retrieve data using key-value pairs while exploring an important systems programming problem: **protecting stored data from corruption or loss when a program crashes during a write operation**.

A key-value pair consists of a key and its associated value:

```text
username → pratham
age      → 21
city     → Delhi
```

In a simple file-based storage system, modifying a file directly can become unsafe if the program crashes while a write is taking place. The file may be left with an incomplete or corrupted record.

litekv addresses this problem using an **append-only Write-Ahead Log (WAL)** and **CRC32 checksums**. The WAL allows new records to be appended rather than overwriting existing records, while the checksum allows the system to detect corrupted or incomplete records.

The project is intentionally kept small enough to understand and explain completely while still demonstrating practical concepts from C programming, data structures, file handling, and persistent storage.

---

## 2. Problem Statement

Simple file-based storage can lose or corrupt data when a program crashes during a write operation.

For example, suppose an application wants to update:

```text
age = 21
```

to:

```text
age = 22
```

If the application crashes while the file is being modified, the stored data may become incomplete or corrupted.

This creates an important question:

> **How can we build a small key-value storage system in C that stores data on disk while detecting incomplete or corrupted records and preserving committed data across crashes?**

litekv is an attempt to solve this problem while keeping the implementation small enough to understand from the ground up.

---

## 3. Goals

The main goals of the project are:

### 3.1 Persistent Storage

Store key-value pairs on disk so that data remains available after the application exits.

### 3.2 Crash Safety

Use an append-only **Write-Ahead Log (WAL)** instead of repeatedly overwriting existing data.

### 3.3 Data Integrity

Use a **CRC32 checksum** for each record to detect corrupted or incomplete records.

### 3.4 Simple API

Provide a small API for storing, retrieving, deleting, opening, and closing the database.

The planned operations are:

```c
kv_open();
kv_put();
kv_get();
kv_delete();
kv_close();
```

### 3.5 Efficient Lookup

Use an in-memory hash table so that keys can be located efficiently without scanning the complete storage file for every lookup.

### 3.6 Understandable Implementation

Keep the system small enough that its architecture, code, data structures, and important design decisions can be understood and explained during a project viva.

### 3.7 C and Systems Programming Practice

The project will provide practical experience with:

* C structures
* Pointers
* Dynamic memory allocation
* File handling
* Raw byte manipulation
* Serialization
* Hashing
* Checksums
* Persistent storage
* Error handling
* Crash recovery

---

## 4. Specifications

### 4.1 Programming Language

The project will be implemented entirely in:

```text
C
```

C is used because the project requires direct interaction with memory, byte-level data, and file operations.

---

### 4.2 Key-Value Representation

Each entry consists of:

```text
Key
Value
```

For example:

```text
key   = "username"
value = "pratham"
```

Keys and values will be represented as byte buffers with explicit lengths.

The basic record structure is:

```c
typedef struct {
    uint32_t key_len;
    uint32_t val_len;
    uint8_t *key;
    uint8_t *value;
} kv_record;
```

Explicit lengths are used instead of relying only on null-terminated strings. This allows the storage layer to work with raw byte data.

---

### 4.3 Record Format

A serialized record will use the following format:

```text
+----------+----------+----------+----------+----------+
| key_len  | val_len  |   key    |  value   |  CRC32   |
| 4 bytes  | 4 bytes  | variable | variable | 4 bytes  |
+----------+----------+----------+----------+----------+
```

Where:

* `key_len` is the length of the key.
* `val_len` is the length of the value.
* `key` contains the key bytes.
* `value` contains the value bytes.
* `CRC32` is the checksum used to verify the record.

The checksum will cover the record data before the checksum field itself.

---

### 4.4 Planned API

#### Open

```c
kv_open(path);
```

Opens an existing storage file or creates a new one.

#### Put

```c
kv_put(db, key, value);
```

Stores a key-value pair.

If the key already exists, the newer record will represent the latest value.

#### Get

```c
kv_get(db, key);
```

Retrieves the value associated with a key.

#### Delete

```c
kv_delete(db, key);
```

Marks a key as deleted using a tombstone record.

#### Close

```c
kv_close(db);
```

Closes the storage and releases allocated resources.

---

## 5. Design

### 5.1 High-Level Architecture

The planned architecture is:

```text
                    Application
                         |
                         |
                kv_put / kv_get
                  /    kv_delete
                         |
                         v
                 +---------------+
                 |   litekv API  |
                 +-------+-------+
                         |
                +--------+--------+
                |                 |
                v                 v
        +---------------+   +---------------+
        |  In-Memory    |   |      WAL      |
        |  Hash Table   |   | Append-Only   |
        |     Index     |   |      Log      |
        +---------------+   +-------+-------+
                                    |
                                    v
                                   Disk
```

The application interacts with litekv through its API.

The library will use:

1. An **in-memory hash table** for efficient lookup.
2. A **Write-Ahead Log** for persistent storage.

---

### 5.2 In-Memory Index

The project will maintain an in-memory hash table.

The basic idea is:

```text
Key
 |
 v
Hash Function
 |
 v
Bucket
 |
 v
WAL Record Offset
```

For example:

```text
"username"
     |
     v
   Hash
     |
     v
  Bucket 5
     |
     v
 Offset 128
```

The index will allow the library to locate information associated with a key without scanning the complete WAL for every lookup.

The hash table will be implemented in a later development stage.

---

### 5.3 Write-Ahead Log

The storage system will use an **append-only log**.

Instead of modifying existing records directly:

```text
Existing Record
      |
      v
   Overwrite
```

new records will be appended:

```text
Record 1
Record 2
Record 3
Record 4
```

The planned write flow is:

```text
Key + Value
     |
     v
Create Record
     |
     v
Serialize Record
     |
     v
Append to WAL
     |
     v
Synchronize to Disk
     |
     v
Update In-Memory Index
```

This design helps preserve previous records and makes recovery possible.

---

### 5.4 Crash Recovery

When litekv is opened, the WAL will be read from the beginning.

The planned recovery process is:

```text
WAL File
   |
   v
Read Record
   |
   v
Verify CRC32
   |
   v
Process Valid Record
   |
   v
Update In-Memory Index
   |
   v
Continue
```

If an invalid or incomplete record is detected, it should not be silently treated as valid data.

The recovery mechanism will be implemented after the record and WAL components are complete.

---

### 5.5 Delete Operation

The WAL will remain append-only.

Therefore, deleting a key will not immediately remove the old record from the file.

Instead, litekv will use a **tombstone record**.

For example:

```text
SET username → pratham
DELETE username
```

The delete operation will append information indicating that the key is no longer active.

During recovery, the later delete operation will take precedence over the earlier value.

---

### 5.6 Memory Management

The project will explicitly manage dynamically allocated memory using C memory-management functions such as:

```c
malloc()
free()
```

Memory ownership will be clearly defined.

For example:

```text
serialize()
     |
     v
  malloc()
     |
     v
Buffer returned
     |
     v
Caller owns buffer
     |
     v
   free()
```

This makes it clear which part of the program is responsible for releasing allocated memory.

---

### 5.7 Error Handling

The system will check for failures in important operations, including:

* Memory allocation
* File opening
* File reading
* File writing
* File synchronization
* Invalid records
* Corrupted records
* Incomplete records

Errors should be detected and handled explicitly rather than silently ignored.

---

## 6. Development Plan

The project will be developed incrementally.

### Milestone 1 — Problem and Basic Design

* Define the problem.
* Define the key-value record.
* Define the record format.
* Define the project architecture.
* Define the basic API.

### Milestone 2 — Records and WAL

* Implement record serialization.
* Implement record deserialization.
* Implement CRC32.
* Implement WAL file handling.
* Append records to the WAL.

### Milestone 3 — In-Memory Index

* Implement the hash table.
* Store key-to-record information.
* Implement efficient lookup.

### Milestone 4 — Recovery

* Read the WAL during startup.
* Validate records.
* Rebuild the in-memory index.

### Milestone 5 — Delete

* Implement tombstone records.
* Handle deleted keys during recovery.

### Milestone 6 — Crash-Safety Testing

* Test incomplete records.
* Test corrupted records.
* Test recovery after failures.
* Add crash-injection testing.

### Milestone 7 — Log Compaction

* Remove obsolete records.
* Rewrite the WAL.
* Reduce unnecessary storage usage.

### Milestone 8 — Demonstration and Benchmarking

* Create a simple CLI demonstration.
* Test common operations.
* Add optional performance benchmarks.

### Milestone 9 — Documentation and Viva

* Complete the project documentation.
* Review the architecture.
* Review the C concepts used.
* Prepare viva questions.
* Perform mock viva practice.

---

## 7. Project Scope

To keep the project manageable and understandable, the following are explicitly outside the main scope:

* Network/server mode
* SQL query language
* Multi-process concurrency
* Distributed replication
* Distributed databases
* Full database-engine functionality

The project focuses specifically on:

> **Building a small persistent key-value storage system with crash-safety mechanisms in C.**

---

## 8. Current Development Status

The project is being developed incrementally.

### Completed

* Project directory structure
* Basic `kv_record` structure
* Key and value length representation
* Basic record test
* Initial serialization implementation

### In Progress

* Complete record serialization/deserialization
* CRC32 checksum
* WAL implementation

### Planned

* In-memory hash table
* `kv_put()`
* `kv_get()`
* `kv_delete()`
* WAL replay
* Crash recovery
* Crash-injection testing
* Log compaction
* CLI demonstration
* Benchmarks
* Final documentation
* Viva preparation

---

## 9. Expected Outcome

The final version of litekv is expected to provide a small C library capable of:

```text
Store key-value data
        |
        v
Persist data to disk
        |
        v
Detect corrupted records
        |
        v
Recover valid state after restart
        |
        v
Provide efficient key lookup
```

The project will demonstrate how data structures, file I/O, serialization, checksums, memory management, and crash recovery can be combined to build a small persistent storage system.

---

## 10. Learning Outcomes

After completing the project, the expected learning outcomes are:

* Understand how persistent storage works at a basic level.
* Understand how data can be represented as raw bytes.
* Work with structures and pointers in C.
* Practice dynamic memory management.
* Understand file I/O and low-level file operations.
* Implement and use a hash table.
* Understand serialization and deserialization.
* Understand checksums and data integrity.
* Understand append-only logs.
* Understand the basic idea of write-ahead logging.
* Understand crash recovery.
* Practice testing failure cases.
* Make and explain engineering trade-offs.

---

## 11. Future Improvements

After the core system is complete, the following features may be considered:

* WAL log compaction
* Crash-injection testing
* Performance benchmarking
* Versioned reads
* Snapshots
* Range queries using a suitable sorted data structure

These features are optional and will only be implemented after the core system is working correctly.

---

## 12. Conclusion

litekv is an educational systems-programming project focused on building a small and understandable crash-safe key-value storage library in C.

The overall development path is:

```text
Data Representation
        |
        v
Serialization
        |
        v
Checksum
        |
        v
Write-Ahead Log
        |
        v
Hash Index
        |
        v
Recovery
```

The goal is not to build a replacement for a complete database system.

Instead, litekv focuses on understanding the fundamental techniques behind persistent and reliable storage by implementing a small system from the ground up in C.

