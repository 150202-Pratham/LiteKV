#include "kv_wal.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

int main(void){

    const char *path = "tests/test.wal" ;

    int fd = kv_wal_open(path) ;
    
    assert(fd>=0) ;

    printf("Test 1 Passed: WAL opened\n"); 

    const uint8_t data[] = "Hello LiteKV" ;
    size_t len = sizeof(data) -1 ;

    int result = kv_wal_append( fd , data , len ) ;

    assert(result == 0);

    printf("Test 2 passed: Data appended \n") ;

    kv_wal_close(fd) ;

    printf("Test 3 PASSEDL WAL closed\n") ;

    return 0 ;



}