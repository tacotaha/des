#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

#include "des.h"
#include "perms.h"

int main(int argc, char **argv) {
  int size, fd;
  struct stat st;
  uint64_t enc, msg, key, *keys, *base = NULL;
  char *payload = argv[argc > 1];

  fd = open(payload, O_RDONLY);
  if (fd < 0)
    return fd;

  stat(payload, &st);
  size = st.st_size;
  size += (size % 8) ? 8 - (size % 8) : 0;

  printf("Size = %d, Blks = %ld\n", size, size / sizeof(uint64_t));

  base = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  printf("Base addr = %p\n", (void *) base);

  keys = calloc(1, sizeof(uint64_t) * KEY_ROUNDS);
  if (!keys)
    return -1;

  key = 0x133457799BBCDFF1ULL;
  gen_subkeys(key, keys);

  for (int i = 0; i < KEY_ROUNDS; ++i) {
    printf("Key %02d: ", i);
    dump_bits(keys[i], 48);
  }

  msg = 0x123456789ABCDEFULL;
  enc = encrypt(msg, key, keys);

  printf("Encrypted = 0x%lx\n", enc);

  free(keys);

  return 0;
}
