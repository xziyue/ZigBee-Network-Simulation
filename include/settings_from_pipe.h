#ifndef FILE_SETTINGS_FROM_PIPE_H
#define FILE_SETTINGS_FROM_PIPE_H

#ifdef __cplusplus
#define CPPEXTERN extern "C"
#else
#define CPPEXTERN
#include <stdbool.h>
#endif


CPPEXTERN void* read_and_parse_stdin();
CPPEXTERN void free_parsed_stdin(void *res);

// check if key is in JSON
CPPEXTERN bool check_has_item(void *res, const char *item);

// query the key and store the value in a shared buffer
CPPEXTERN void load_item_into_buffer(void *res, const char *item);

// get the shared buffer
// the behavior is undefined before calling read_and_parse_stdin()
CPPEXTERN const char *get_item_buffer();


#ifdef __cplusplus
struct _eightbytes{
    unsigned char data[8];
};
#else
typedef struct _d_eightbytes{
    unsigned char data[8];
} _eightbytes;
#endif

// parse the buffer as ieee device address
CPPEXTERN _eightbytes get_ieee_addr_from_buffer();

#endif