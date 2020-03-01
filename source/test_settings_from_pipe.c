#include "settings_from_pipe.h"
#include <stdio.h>

int main(){

    void *params = read_and_parse_stdin();

    load_item_into_buffer(params, "pan-id");
    printf("%s\n", get_item_buffer());

    load_item_into_buffer(params, "device-mac");
    _eightbytes addr = get_ieee_addr_from_buffer();
    printf("%s\n", get_item_buffer());

    for(int i = 0; i < 8; ++i){
        printf("%02x\n", addr.data[i]);
    }

    free_parsed_stdin(params);

    return 0;
}