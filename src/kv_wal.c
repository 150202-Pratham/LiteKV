#include "kv_wal.h"
/*
  This Gives us functions related to opening Files
  like open() and also provide flags such as O_WRONLY ,O_CREAT, O_APPEND
*/
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int kv_wal_open(const char *path){

    if(path == NULL ){

        return -1 ;

    }
    /*
      O_WRONLY -> open for Writing
      O_CREAT -> Create the file if doesn't exist ;
      o_APPEND -> every write goes to the end of the file
    */
    int fd = open(
        path,
        O_WRONLY | O_CREAT | O_APPEND ,
        // This is File Permission like rw-r--r--
        0644 
    );

    /*
      open -> on Success returns -> file descriptor
      failure -> -1 ;

    */

    if( fd == -1){
        return -1 ;

    }

    return fd ;

}

int kv_wal_append(int fd , const uint8_t *data , size_t len){

    if(fd<0 || data==NULL){

        return -1 ;

    }
    
    /*
     write() -> returns the actually the number of bytes written 
    it can  also return -1 if an error occurs therefore return type needs to support negative
    values
    */
    ssize_t written = write(fd , data , len ) ;

    if(written != (ssize_t)len){
        return -1 ;

    }

    /*
      Makes Sure the Written data is 
      flushed to presisitent storage
      Important
      -> fsync() asks the operating system to flush the file's modified
      data and associated filesystem metadata as needed so that the
      changes are committed to persistent storage according to the 
      system's durability semantics.
    */
     
    if(fsync(fd)== -1){
        return -1 ;

    }
    return 0 ;

}

int kv_wal_read(int fd,
                uint8_t **data,
                size_t *len) {

    if (fd < 0 || data == NULL || len == NULL) {
        return -1;
    }

    uint8_t header[8];

    ssize_t n =
        read(fd, header, 8);

    if (n == 0) {
        return 1;
    }

    if (n != 8) {
        return -1;
    }

    uint32_t key_len;
    uint32_t val_len;

    memcpy(
        &key_len,
        header,
        4
    );

    memcpy(
        &val_len,
        header + 4,
        4
    );

    size_t body_len = 4 + 4 + key_len + val_len;

    size_t total_len = body_len + 4;

    uint8_t *buffer = malloc(total_len);

    if (buffer == NULL) {
        return -1;
    }

    memcpy(
        buffer,
        header,
        8
    );

    size_t remaining =
        total_len - 8;

    uint8_t *p =
        buffer + 8;

    ssize_t bytes_read =
        read(fd, p, remaining);

    if (bytes_read != (ssize_t)remaining) {

        free(buffer);

        return -1;
    }

    *data = buffer;
    *len = total_len;

    return 0;
}


void kv_wal_close(int fd){

    if(fd >=0 ){

        close(fd) ;

    }
}