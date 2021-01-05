#include <stdio.h>
#include <inttypes.h>
#include "perms.h"
#include "des.h"

#define DEBUG 0

void dump_bits(uint64_t b, int blen) {
  for (int i = 63; i > 63 - blen; --i) {
    printf("%lld", (b & (1ULL << i)) >> i);
  }
  printf("\n");
}

void dump_block(uint64_t b) {
  dump_bits(b, 56);
}

void cshift(uint64_t * block, int blen) {
  uint64_t msb = (*block & (1ULL << 63)) >> 63;
  *block <<= 1;
  *block |= (msb << (64 - blen));
}

void gen_subkeys(uint64_t key, uint64_t * keys) {
  uint64_t left, right, c_0 = 0, d_0 = 0, p0;

  p0 = permute(key, p_0, 56);

  left = p0 & (0xfffffffULL << 36);
  right = (p0 & (0xfffffffULL << 8)) << 28;

#if DEBUG
  printf("Key = ");
  dump_bits(key, 64);

  printf("Initial Permutation = ");
  dump_bits(p0, 64);

  printf("Right = ");
  dump_bits(right, 64);

  printf("Left  = ");
  dump_bits(left, 64);
#endif

  c_0 = left;
  d_0 = right;

  for (int i = 0; i < KEY_ROUNDS; ++i) {
    for (int j = 0; j < key_sched[i]; ++j) {
      cshift(&c_0, 28);
      cshift(&d_0, 28);
    }

    keys[i] = c_0 | (d_0 >> 28);
    keys[i] = permute(keys[i], p_1, 48);

#if DEBUG
    printf("============================\n");
    printf("Iteration: %d\n", i);
    dump_bits(c_0, 28);
    dump_bits(d_0, 28);
    dump_bits(keys[i], 48);
    printf("============================\n");
#endif
  }

}

uint64_t f(uint64_t block, uint64_t subkey) {
  uint8_t r = 0, c = 0, j = 60;
  uint64_t res, blk, exp, result = 0;

  exp = permute(block, e_table, 48) ^ subkey;

  for (int i = 64; i > 16; i -= 6) {
    blk = (exp & (0x3fULL << (i - 6))) >> (i - 6);
    r = ((blk & (1 << 5)) >> 4) | (blk & 1);
    c = (blk & (0xf << 1)) >> 1;

    switch (i) {
      case 64:
        res = s_1[r][c];
        break;
      case 58:
        res = s_2[r][c];
        break;
      case 52:
        res = s_3[r][c];
        break;
      case 46:
        res = s_4[r][c];
        break;
      case 40:
        res = s_5[r][c];
        break;
      case 34:
        res = s_6[r][c];
        break;
      case 28:
        res = s_7[r][c];
        break;
      case 22:
        res = s_8[r][c];
        break;
    };

    result |= (res << j);
    j -= 4;
  }

  result = permute(result, p_3, 32);

#if DEBUG
  printf("f = ");
  dump_bits(result, 32);
#endif

  return result;
}

uint64_t encrypt(uint64_t msg, uint64_t key, uint64_t * keys) {
  uint64_t left, right, i_perm, tmp;

  i_perm = permute(msg, p_2, 64);
  left = i_perm & (0xffffffffULL << 32);
  right = (i_perm & 0xffffffffULL) << 32;

  for (int i = 0; i < KEY_ROUNDS; ++i) {
    tmp = left;
    left = right;
    right = tmp ^ f(right, keys[i]);
  }

  return permute(right | (left >> 32), p_4, 64);
}

uint64_t decrypt(uint64_t msg, uint64_t key, uint64_t * keys) {
  uint64_t left, right, i_perm, tmp;

  i_perm = permute(msg, p_2, 64);
  left = i_perm & (0xffffffffULL << 32);
  right = (i_perm & 0xffffffffULL) << 32;

  for (int i = KEY_ROUNDS - 1; i > -1; --i) {
    tmp = left;
    left = right;
    right = tmp ^ f(right, keys[i]);
  }

  return permute(right | (left >> 32), p_4, 64);
}
