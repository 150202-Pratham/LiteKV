CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

BUILD_DIR = build

SRC = src/kv_record.c \
      src/kv_wal.c \
      src/kv_hash.c \
      src/kv_replay.c \
      src/kv.c

LIB = $(BUILD_DIR)/liblitekv.a

TESTS = \
    tests/test_record \
    tests/test_wal \
    tests/test_wal_read \
    tests/test_hash_put \
    tests/test_hash_get \
    tests/test_hash_delete \
    tests/test_hash_free \
    tests/test_open_close \
    tests/test_put \
    tests/test_get \
    tests/test_delete \
    tests/test_replay \
    tests/test_replay_apply \
    tests/integration/test_persistence \
    tests/integration/test_delete_persistence \
    tests/integration/test_update_persistence \
    tests/integration/test_recovery \
    tests/integration/test_crc_corruption

.PHONY: all test clean

all: $(LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/kv_record.c -o $(BUILD_DIR)/kv_record.o
	$(CC) $(CFLAGS) -c src/kv_wal.c -o $(BUILD_DIR)/kv_wal.o
	$(CC) $(CFLAGS) -c src/kv_hash.c -o $(BUILD_DIR)/kv_hash.o
	$(CC) $(CFLAGS) -c src/kv_replay.c -o $(BUILD_DIR)/kv_replay.o
	$(CC) $(CFLAGS) -c src/kv.c -o $(BUILD_DIR)/kv.o
	$(AR) rcs $(LIB) $(BUILD_DIR)/*.o

tests/test_%: tests/test_%.c $(SRC)
	$(CC) $(CFLAGS) $(SRC) $< -o $@

tests/integration/test_%: tests/integration/test_%.c $(SRC)
	$(CC) $(CFLAGS) $(SRC) $< -o $@

test: $(TESTS)
	@set -e; \
	for t in $(TESTS); do \
	    echo ""; \
	    echo "========== Running $$t =========="; \
	    ./$$t; \
	done

clean:
	rm -rf $(BUILD_DIR) $(TESTS)
