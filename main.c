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

#define DEBUG 1

const int initial_perm[] = {57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26,
                            18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52,
                            44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46,
                            38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5,
                            28, 20, 12, 4};

const int key_sched[] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

void dump_bits(uint64_t b, int blen){
    for(int i = 0; i < blen; ++i)
        printf("%lld", (b & (1ULL << i)) >> i);
    printf("\n");
}

void dump_block(uint64_t b){
    dump_bits(b, 56);
}

void cshift(uint64_t *block, int blen){
    uint8_t msb = *block & 1; 
    (*block) >>= 1;
    *block |= msb << (blen - 1);
}

uint64_t permute(uint64_t b, const int* permutation){
    uint64_t temp, result = 0;
    
    for(int i = 0; i < 56; ++i){
        temp = (b & (1ULL << (permutation[i] - 1))) >> (permutation[i] - 1);
        result |= temp << i;
    }

    return result;
}

void gen_keys(uint64_t key){
    uint64_t p1 = permute(key, initial_perm);
    uint32_t right = p1 & 0xfffffffLL; 
    uint32_t left = (p1 & (0xfffffffLL << 28)) >> 28; 
    uint64_t c_0 = left, d_0 = right;

#if DEBUG
    printf("Key = ");
    dump_block(key);

    printf("Initial Permutation = ");
    dump_block(p1);

    printf("Right = ");
    dump_bits(right, 28);

    printf("Left  = ");
    dump_bits(left, 28);
#endif
    
    for(int i = 0; i < 16; ++i){
        for(int j = 0; j < key_sched[i]; ++j){
            cshift(&c_0, 28);
            cshift(&d_0, 28);            
        }
#if DEBUG
        printf("============================\n");
        printf("Iteration: %d\n", i);
        dump_bits(d_0, 28);
        dump_bits(c_0, 28);
        printf("============================\n");
#endif
    }
}

int main(int argc, char** argv){
  int size, fd;
  struct stat st;
  uint64_t *base = NULL;
  char *payload = argv[argc > 1];

  fd = open(payload, O_RDONLY);
  if (fd < 0)
    return fd;

  stat(payload, &st);
  size = st.st_size;

  base = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);  
 
  /*
  for(int i = 0; i < size / sizeof(*base); ++i)
          printf("0x%llx ", base[i]);
  */

  printf("\nBase addr = %p\n", (void*)base);
  gen_keys(0x8FFB3DD99EEA2CC8);
  
  return 0;
}
