#include "../header.h"

#define MT_N 624
#define MT_M 397
#define MT_MATRIX_A 0x9908b0dfUL
#define MT_UPPER_MASK 0x80000000UL
#define MT_LOWER_MASK 0x7fffffffUL

static unsigned long mt[MT_N];
static int mti = MT_N + 1;

// Inizializza con seed 
void init_genrand(unsigned long s) {
    mt[0] = s & 0xffffffffUL;
    for (mti = 1; mti < MT_N; mti++) {
        mt[mti] = (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        mt[mti] &= 0xffffffffUL;
    }
}

// Genera un intero random a 32 bit 
unsigned long genrand_int32(void) {
    unsigned long y;
    static unsigned long mag01[2] = {0x0UL, MT_MATRIX_A};
    if (mti >= MT_N) {
        int kk;
        if (mti == MT_N + 1)   // se non è stato seedato 
            init_genrand(5489UL);

        for (kk = 0; kk < MT_N - MT_M; kk++) {
            y = (mt[kk] & MT_UPPER_MASK) | (mt[kk+1] & MT_LOWER_MASK);
            mt[kk] = mt[kk+MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (; kk < MT_N - 1; kk++) {
            y = (mt[kk] & MT_UPPER_MASK) | (mt[kk+1] & MT_LOWER_MASK);
            mt[kk] = mt[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[MT_N-1] & MT_UPPER_MASK) | (mt[0] & MT_LOWER_MASK);
        mt[MT_N-1] = mt[MT_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    // Tempering 
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* Restituisce un intero nell’intervallo [min, max] */
int random_between(int min, int max) {
    return (genrand_int32() % (max - min + 1)) + min;
}

/* --- ESEMPIO D'USO --- */
/*
int main(void) {
    init_genrand((unsigned long)time(NULL)); // inizializza con il tempo
        printf("%d\n", random_between(0, 100));
}*/