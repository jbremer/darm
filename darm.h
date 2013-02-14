#ifndef __DARM__
#define __DARM__

#ifndef ARRAYSIZE
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

typedef struct _darm_t {
    char        mnemonic[12];
    uint32_t    cond;

    // register shift info
    uint32_t type;
    uint32_t Rs;
} darm_t;

const char *armv7_condition_info(int condition_flag,
    const char **meaning_integer, const char **meaning_floating_point,
    int omit_always_mnemonic);

int armv7_disassemble(darm_t *d, uint32_t w);

#endif
