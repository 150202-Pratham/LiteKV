#include "kv_wal.h"
/*
  This Gives us functions related to opening Files
  like open() and also provide flags such as O_WRONLY ,O_CREAT, O_APPEND
*/
#include <fcntl.h>
#include <unistd.h>

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

    return 0 ;

}

void kv_wal_close(int fd){

    if(fd >=0 ){

        close(fd) ;

    }
}