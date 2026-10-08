#ifndef KV_WAL_H

#define KV_WAL_H

// Gives us definations related to sizes -> size_t = it is used to store sizes of memory buffers and files
#include <stddef.h>
#include <stdint.h>

/*
  Open a WAL File
  Returns 
  file Descriptor >=0 on success 
  -1 on failure

*/

int kv_wal_open(const char *path) ;

/*
 Append raw bytes to the WAL

  Returns
  0 on success 
  and -1 on failure

*/

/*
  what is fd -> it is File Descriptor
  linux represents an opened file using a number
  for example fd = 3 ;
  means The Operating System Gave my programm identifier 3 for this open file
*/
int kv_wal_append(int fd , const uint8_t *data , size_t len) ;


/*
 Close the WAL File
*/

void kv_wal_close(int fd) ;

/*
 * Read one complete serialized record from the WAL.
 *
 * Returns:
 *   0  on success
 *   1  when end of WAL is reached
 *  -1  on error or incomplete record
 */
int kv_wal_read(int fd,
                uint8_t **data,
                size_t *len
            );

#endif
