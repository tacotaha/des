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
  struct stat st;
  int size, blks, fd, enc;
  char *kfd, *payload, *out;
  uint64_t res, key, *keys, *base;

  if (argc < 3) {
    fprintf(stderr, "Usage: %s -[d/e] <key> <infile> <outfile>\n", argv[0]);
    return -1;
  }

  enc = strcmp(argv[1], "-d");
  kfd = argv[2];
  payload = argv[3];
  out = argv[4];

  fd = open(kfd, O_RDONLY);
  if (fd < 0)
    return fd;
  read(fd, &key, sizeof(uint64_t));
  close(fd);

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

  keys = calloc(1, sizeof(uint64_t) * KEY_ROUNDS);
  if (!keys)
    return -1;

  gen_subkeys(key, keys);

  for (int i = 0; i < KEY_ROUNDS; ++i) {
    printf("Key %02d: ", i);
    dump_bits(keys[i], 48);
  }

  fd = open(out, O_RDWR | O_CREAT, 0600);
  if (fd < 0)
    return fd;

  for (int i = 0; i < blks; ++i) {
    res = enc ? encrypt(base[i], key, keys) : decrypt(base[i], key, keys);
    write(fd, &res, sizeof(uint64_t));
  }

  close(fd);
  free(keys);

  return 0;
}
