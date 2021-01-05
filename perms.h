#ifndef PERMS_H
#define PERMS_H

extern const int key_sched[];
extern const int e_table[];

extern const int p_0[];
extern const int p_1[];
extern const int p_2[];
extern const int p_3[];
extern const int p_4[];

extern const int s_1[4][16];
extern const int s_2[4][16];
extern const int s_3[4][16];
extern const int s_4[4][16];
extern const int s_5[4][16];
extern const int s_6[4][16];
extern const int s_7[4][16];
extern const int s_8[4][16];

uint64_t permute(uint64_t, const int *, int);

#endif                          /* PERMS_H */
