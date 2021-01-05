#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "des.h"
#include "perms.h"

int main(int argc, char **argv) {
  int size, blks, fd;
  struct stat st;
  uint64_t enc, key, *keys, *base;
  char *payload = argv[1];
  char *out = argv[2];

  fd = open(payload, O_RDONLY);
  if (fd < 0)
    return fd;

  stat(payload, &st);
  size = st.st_size;
  size += (size % 8) ? 8 - (size % 8) : 0;
  blks = size / sizeof(uint64_t);

  printf("Size = %d, Blks = %ld\n", size, size / sizeof(uint64_t));

  base = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  fd = open(out, O_RDWR | O_CREAT, 0600);
  if (fd < 0)
    return fd;

  keys = calloc(1, sizeof(uint64_t) * KEY_ROUNDS);
  if (!keys)
    return -1;

  key = 0x133457799BBCDFF1ULL;
  gen_subkeys(key, keys);

  for (int i = 0; i < KEY_ROUNDS; ++i) {
    printf("Key %02d: ", i);
    dump_bits(keys[i], 48);
  }

  for (int i = 0; i < blks; ++i) {
    enc = encrypt(base[i], key, keys);
    write(fd, &enc, sizeof(uint64_t));
  }

  close(fd);
  free(keys);

  return 0;
}
