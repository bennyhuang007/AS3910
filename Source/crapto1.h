#ifndef _CRAPTO1_H_
#define _CRAPTO1_H_

#include "other.h"
#include "ams_types.h"

void crypto1_init(Crypto1_TypeDef *state, u8 *key);
void crypto1_get_lfsr(Crypto1_TypeDef *state, uint64_t *lfsr);
u32 crypto1_bit(Crypto1_TypeDef *state, u32 in_bit, u8 fb);
u32 crypto1_word(Crypto1_TypeDef *state, u32 in_word, u8 fb);
void prng_successor(u32 nT, u32 *aR,u32 *aT);

#endif