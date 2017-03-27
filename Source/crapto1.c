#include "crapto1.h"

#define BIT(x, n) ((x) >> (n) & 1)

void crypto1_init(Crypto1_TypeDef *state, u8 *key)
{
    u8 countByte,countBit;
    
    state->odd = state->even = 0;
    for(u8 i = 0; i < 48; i += 2)
    {
        countByte = i >> 3;
        countBit = i & 0x07;
        state->odd  = state->odd  << 1 | BIT(*(key + countByte),countBit);
        state->even = state->even << 1 | BIT(*(key + countByte),countBit + 1);
    }
}

u32 crypto1_bit(Crypto1_TypeDef *state, u32 in_bit, u8 fb)
{
    u32 nf;
    
    in_bit = !!in_bit;
    in_bit ^= state->odd & 0x29CE5C;
    in_bit ^= state->even & 0x870804;
    in_bit ^= in_bit >> 16;
    in_bit ^= in_bit >> 8;
    in_bit ^= in_bit >> 4;
    in_bit &= 0xf;
    in_bit = BIT(0x6996, in_bit);
    
    nf =  0x0D938 >> (state->odd >> 0x10 & 0xf) &  1;
    nf |= 0x1e458 >> (state->odd >> 0x0C & 0xf) &  2;
    nf |= 0x3c8b0 >> (state->odd >> 0x08 & 0xf) &  4;
    nf |= 0x6c9c0 >> (state->odd >> 0x04 & 0xf) &  8;
    nf |= 0xf22c0 >> (state->odd >> 0x00 & 0xf) & 16;
    nf = BIT(0xEC57E80A, nf);
    
    if(fb)
        in_bit ^= nf;
    
    state->even = state->even << 1 | in_bit;
    
    state->odd ^= state->even;
    state->even ^= state->odd;
    state->odd ^= state->even;
    
    return nf;
}

u32 crypto1_word(Crypto1_TypeDef *state, u32 in_word, u8 fb)
{
    u32 i, ret = 0;
    
    for (i = 24; i < 32; i ^= 24, ++i, i ^= 24)
        ret |= crypto1_bit(state, BIT(in_word, i), fb) << i;
    
    return ret;
}

void prng_successor(u32 nT, u32 *aR,u32 *aT)
{
    u8 count = 0;
    u32 Temp;
    nT = nT >> 8 & 0xff00ff | (nT & 0xff00ff) << 8;
    nT = nT >> 16 | nT << 16;
    
    for(count = 0;count < 64;count++)
    {
        nT = nT >> 1;
        nT |= (BIT(nT, 15) ^ BIT(nT, 17) ^ BIT(nT, 18) ^ BIT(nT, 20)) << 31;
    }
    
    Temp = nT >> 8 & 0xff00ff | (nT & 0xff00ff) << 8;
    Temp = Temp >> 16 | Temp << 16;
    
    *aR = Temp;
    
    for(count = 0;count < 32;count++)
    {
        nT = nT >> 1;
        nT |= (BIT(nT, 15) ^ BIT(nT, 17) ^ BIT(nT, 18) ^ BIT(nT, 20)) << 31;
    }
    
    Temp = nT >> 8 & 0xff00ff | (nT & 0xff00ff) << 8;
    Temp = Temp >> 16 | Temp << 16;
    
    *aT = Temp;
}
