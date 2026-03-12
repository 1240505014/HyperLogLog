#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

typedef struct {
    int p;
    int m;
    uint8_t *registers;
    double alpha;
} HyperLogLog;

double get_alpha(int m) {
    if (m == 16) return 0.673;
    if (m == 32) return 0.697;
    if (m == 64) return 0.709;
    return 0.7213 / (1.0 + 1.079 / m);
}

HyperLogLog* hll_create(int p) {
    HyperLogLog *hll = (HyperLogLog*)malloc(sizeof(HyperLogLog));
    hll->p = p;
    hll->m = 1 << p;
    hll->registers = (uint8_t*)calloc(hll->m, sizeof(uint8_t));
    hll->alpha = get_alpha(hll->m);
    return hll;
}

uint32_t hash_fnv1a(const char* key, int len) {
    uint32_t hash = 2166136261u;
    int i;
    for (i = 0; i < len; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619u;
    }
    return hash;
}

void hll_add(HyperLogLog *hll, const char *item) {
    uint32_t x = hash_fnv1a(item, strlen(item));
    uint32_t j = x >> (32 - hll->p);
    uint32_t w = x << hll->p; 
    
    uint8_t rho = 1;
    if (w == 0) {
        rho = 32 - hll->p + 1;
    } else {
        rho = __builtin_clz(w) + 1;
    }
    
    if (rho > hll->registers[j]) {
        hll->registers[j] = rho;
    }
}

double hll_count(HyperLogLog *hll) {
    double Z = 0.0;
    int V = 0;
    int i;
    for (i = 0; i < hll->m; i++) {
        Z += 1.0 / (1 << hll->registers[i]);
        if (hll->registers[i] == 0) {
            V++;
        }
    }
    
    double E = hll->alpha * (double)hll->m * (double)hll->m / Z;
    
    if (E <= 2.5 * hll->m) {
        if (V != 0) {
            E = hll->m * log((double)hll->m / V);
        }
    } else if (E > (1LL << 32) / 30.0) {
        E = -(1LL << 32) * log(1.0 - E / (1LL << 32));
    }
    
    return E;
}

HyperLogLog* hll_merge(HyperLogLog *hll1, HyperLogLog *hll2) {
    if (hll1->p != hll2->p) return NULL;
    
    HyperLogLog *merged = hll_create(hll1->p);
    int i;
    for (i = 0; i < hll1->m; i++) {
        merged->registers[i] = (hll1->registers[i] > hll2->registers[i]) ? hll1->registers[i] : hll2->registers[i];
    }
    return merged;
}

int main() {
    HyperLogLog *hll1 = hll_create(10);
    HyperLogLog *hll2 = hll_create(10);
    char item[32];
    int i;

    for (i = 0; i < 10000; i++) {
        sprintf(item, "item_%d", i);
        hll_add(hll1, item);
    }

    for (i = 5000; i < 15000; i++) {
        sprintf(item, "item_%d", i);
        hll_add(hll2, item);
    }

    printf("HLL 1 Tahmini: %f\n", hll_count(hll1));
    printf("HLL 2 Tahmini: %f\n", hll_count(hll2));

    HyperLogLog *merged = hll_merge(hll1, hll2);
    printf("Birlestirilmis HLL Tahmini: %f\n", hll_count(merged));

    free(hll1->registers); free(hll1);
    free(hll2->registers); free(hll2);
    free(merged->registers); free(merged);

    return 0;
}
