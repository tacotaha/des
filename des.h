#ifndef DES_H
#define DES_H

#include <inttypes.h>

#define BLK_SIZE (sizeof(uint64_t) << 3)
#define KEY_ROUNDS 16

void dump_bits(uint64_t, int);
void dump_block(uint64_t);
void cshift(uint64_t *, int);

void gen_subkeys(uint64_t, uint64_t *);

uint64_t f(uint64_t, uint64_t);
uint64_t encrypt(uint64_t, uint64_t, uint64_t *);

#endif                          /* DES_H */
