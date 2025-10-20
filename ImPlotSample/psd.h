#pragma once

#include <cmath>

#define PI acos(-1)

void psd(const double arr[], const double freq, const double dt, const int size, double* pX, double* pY) {
    *pX = 0; *pY = 0;
    // 掛け算
    for (int i = 0; i < size; i++) {
        double wt = 2 * PI * freq * dt * i;
        *pX += arr[i] * 2 * sin(wt);
        *pY += arr[i] * 2 * cos(wt);
    }
    // ローパスフィルタの代わりに平均を用いる
    *pX /= size;
    *pY /= size;
}