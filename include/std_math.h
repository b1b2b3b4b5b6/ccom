#ifndef STD_MATH_H
#define STD_MATH_H

#include "std_common.h"

typedef struct kalman_t{
    float R;
    float Q;

    float x_last;
    float x_mid;
    float x_now;
    float p_last;
    float p_mid ;
    float p_now;
    float kg;
}kalman_t;

kalman_t *std_build_kalman(float Q, float R, float init_value);
float std_calcu_kalman(kalman_t *k, float raw);
void std_free_kalman(kalman_t *k);

uint64_t std_up_aline(uint64_t num, uint64_t aline);

#endif