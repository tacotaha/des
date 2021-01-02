#include <stdio.h>
#include <inttypes.h>

#include "perms.h"
#include "des.h"

#define DEBUG 0

void dump_bits(uint64_t b, int blen) {
  for (int i = 0; i < blen; ++i)
    printf("%lld", (b & (1ULL << i)) >> i);
  printf("\n");
}

void dump_block(uint64_t b) {
  dump_bits(b, 56);
}

void cshift(uint64_t * block, int blen) {
  uint8_t msb = *block & 1;
  *block >>= 1;
  *block |= msb << (blen - 1);
}

void gen_subkeys(uint64_t key, uint64_t * keys) {
  uint32_t left, right;
  uint64_t c_0 = 0, d_0 = 0, p0;

  p0 = permute(key, p_0, 56);
  left = p0 & 0xfffffffULL;
  right = (p0 & (0xfffffffULL << 28)) >> 28;

#if DEBUG
  printf("Key = ");
  dump_block(key);

  printf("Initial Permutation = ");
  dump_block(p0);

  printf("Right = ");
  dump_bits(right, 28);

  printf("Left  = ");
  dump_bits(left, 28);
#endif

  c_0 = left;
  d_0 = right;

  for (int i = 0; i < KEY_ROUNDS; ++i) {
    for (int j = 0; j < key_sched[i]; ++j) {
      cshift(&c_0, 28);
      cshift(&d_0, 28);
    }

    keys[i] = 0;
    keys[i] |= c_0;
    keys[i] |= d_0 << 28;
    keys[i] = permute(keys[i], p_1, 48);

#if DEBUG
    printf("============================\n");
    printf("Iteration: %d\n", i);
    dump_bits(c_0, 28);
    dump_bits(d_0, 28);
    dump_bits(keys[i], 56);
    printf("============================\n");
#endif
  }

}

uint64_t f(uint64_t block, uint64_t subkey) {
  uint64_t res = permute(block, e_table, 48);
  return res;
}

void encrypt(uint64_t msg, uint64_t key, uint64_t * keys) {
  uint64_t left, right, i_perm, res;

  i_perm = permute(msg, p_2, 64);
  left = i_perm & 0xffffffff;
  right = (i_perm & (0xffffffffULL << 32)) >> 32;

  for (int i = 0; i < KEY_ROUNDS; ++i) {
    res = f(right, keys[i]);
    res ^= keys[i];
    dump_bits(res, 48);
  }

  dump_bits(left, 32);
}
