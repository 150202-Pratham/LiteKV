#include "kv_wal.h"
#include <stdio.h>
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

    *data = NULL ;
    *len = 0 ;
    uint8_t header[12];
    size_t  header_bytes = 0 ;

    while(header_bytes < sizeof(header) ){
        ssize_t n = read(
            fd ,
            header + header_bytes,
            sizeof(header) - header_bytes
        );
        
        if(n==0){

            if(header_bytes == 0 ){
                return 1 ;

            }

            fprintf(
                stderr,
                "WAL ERROR: Incomplete header (%zu of 12 bytes)\n",
                header_bytes
            );

            return -1 ;

        }

        if( n<0 ){
            perror("WAL ERROR: Reading header");
            return -1; 
        }
        header_bytes += (size_t)n ;

    } 
   
    uint32_t operation ;
    uint32_t key_len;
    uint32_t val_len;

    memcpy(
        &operation,
        header,
        4
    );
    memcpy(
        &key_len,
        header+4,
        4
    );

    memcpy(
        &val_len,
        header + 8,
        4
    );
    
    // if(operation!=1 && operation!=2){
    //     return -1 ;
    // }
    
    fprintf(stderr, "DEBUG: operation=%u, key_len=%u, val_len=%u\n",
        operation, key_len, val_len);

    if (operation != 1 && operation != 2) {
        fprintf(stderr, "DEBUG: Invalid operation\n");
        return -1;
    }


    if(operation == 2 && val_len != 0){
         fprintf(
            stderr,
            "WAL ERROR: DELETE record has a nonzero value length\n"
        );
        return -1 ;

    }
    size_t body_len = 12+ (size_t)key_len + (size_t)val_len;

    size_t total_len = body_len + 4;

    uint8_t *buffer = malloc(total_len);

    if (buffer == NULL) {
        return -1;
    }

    memcpy(
        buffer,
        header,
        sizeof(header)
    );

    size_t remaining =
        total_len - sizeof(header);
    size_t bytes_read = 0 ;

    while(bytes_read < remaining ){
        ssize_t n = read(
            fd ,
            buffer + sizeof(header) + bytes_read,
            remaining - bytes_read
        );
        
        if (n == 0) {
            fprintf(
                stderr,
                "WAL ERROR: Incomplete record body "
                "(%zu of %zu bytes read)\n",
                bytes_read,
                remaining
            );

            free(buffer);
            return -1;
        }
        if(n<0){
            free(buffer) ;
            return -1 ;

        }

        bytes_read += (size_t)n ;

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