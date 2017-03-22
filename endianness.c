//
// Created by howorang on 21.03.17.
//

#include <stdint.h>
#include "endianness.h"

uint64_t htond(double hostdouble) {
    char* data = &hostdouble;
    uint64_t result;
    char *dest = (char *)&result;
    if(!is_big_endian()) {
        for (int i = 0; i < sizeof(double); i++)
            dest[i] = data[sizeof(double) - i - 1];
    } else {
        for (int i = 0; i < sizeof(double); i++)
            dest[i] = data[i];
    }
    return result;
}
double ntohd(uint64_t netdouble) {
    char* data = &netdouble;
    double result;
    char *dest = (char *)&result;
    if(!is_big_endian()) {
        for (int i = 0; i < sizeof(double); i++)
            dest[i] = data[sizeof(double) - i - 1];
    } else {
        for (int i = 0; i < sizeof(double); i++)
            dest[i] = data[i];
    }
    return result;
}

int is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } e = { 0x01000000 };

    return e.c[0];
}