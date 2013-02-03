#ifndef __DARM__
#define __DARM__

const char *armv7_condition_info(int condition_flag,
    const char **meaning_integer, const char **meaning_floating_point,
    int omit_always_mnemonic);

#endif
