#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "des.h"

typedef unsigned long long ull;

int main() {
  uint64_t r, enc, dec, key, keys[16];

  srand(time(0));

  key = rand() | ((ull) rand() << 32);
  gen_subkeys(key, keys);

  printf("Key = ");
  dump_bits(key, 64);

  for(int i = 0; i < KEY_ROUNDS; ++i){
      printf("Subkey %02d: ", i);
      dump_bits(keys[i], 64);
  }

  for (int i = 0; i < (1 << 10); ++i) {
    r = rand() | ((ull) rand() << 32);
    enc = encrypt(r, key, keys);
    dec = decrypt(enc, key, keys);
    assert(r == dec);
  }

  return 0;
}
