//
// Created by howorang on 21.03.17.
//

#ifndef SOCKETS_ENDIANNESS_H
#define SOCKETS_ENDIANNESS_H

#include <stdint.h>

uint64_t htond(double hostdouble);
double ntohd(uint64_t netdouble);
int is_big_endian(void);
#endif //SOCKETS_ENDIANNESS_H
