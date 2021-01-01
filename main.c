#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

#define RIGHT 0xffffffffULL
#define LEFT (RIGHT << 32)
#define BLK_SIZE (sizeof(uint64_t) << 3)

#define KEY_ROUNDS 16
#define DEBUG 0

const int p_0[] = { 57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26,
  18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52,
  44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46,
  38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5,
  28, 20, 12, 4
};

const int p_1[] = { 14, 17, 11, 24, 1, 5,
  3, 28, 15, 6, 21, 10,
  23, 19, 12, 4, 26, 8,
  16, 7, 27, 20, 13, 2,
  41, 52, 31, 37, 47, 55,
  30, 40, 51, 45, 33, 48,
  44, 49, 39, 56, 34, 53,
  46, 42, 50, 36, 29, 32
};

const int key_sched[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

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
  (*block) >>= 1;
  *block |= msb << (blen - 1);
}

uint64_t permute(uint64_t b, const int *permutation) {
  uint64_t temp, result = 0;

  for (int i = 0; i < 56; ++i) {
    temp = (b & (1ULL << (permutation[i] - 1))) >> (permutation[i] - 1);
    result |= temp << i;
  }

  return result;
}

void gen_keys(uint64_t key, uint64_t *keys) {
  uint32_t left, right;
  uint64_t c_0 = 0, d_0 = 0, p0;

  p0 = permute(key, p_0);
  left = p0 & 0xfffffffLL;
  right = (p0 & (0xfffffffLL << 28)) >> 28;

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
    keys[i] = permute(keys[i], p_1);

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

int main(int argc, char **argv) {
  int size, fd;
  struct stat st;
  uint64_t *keys, *base = NULL;
  char *payload = argv[argc > 1];

  fd = open(payload, O_RDONLY);
  if (fd < 0)
    return fd;

  stat(payload, &st);
  size = st.st_size;

  base = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  printf("Base addr = %p\n", (void *) base);

  keys = calloc(1, sizeof(uint64_t) * KEY_ROUNDS);
  if(!keys) return -1;

  gen_keys(0x8FFB3DD99EEA2CC8, keys);

  for(int i = 0; i < KEY_ROUNDS; ++i){
      printf("Key %02d: ", i);
      dump_bits(keys[i], 48);
  }

  free(keys);

  return 0;
}
