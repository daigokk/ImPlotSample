#pragma once

#include <vector>
#include <complex>
#define PI acos(-1)

void fft(std::vector<std::complex<double>>& a) {
    int N = a.size();
    if (N <= 1) return;

    // 偶数・奇数に分割 配列のサイズが半分になる！！！
    std::vector<std::complex<double>> even(N / 2), odd(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        even[i] = a[i * 2];
        odd[i] = a[i * 2 + 1];
    }

    // 再帰呼び出し
    fft(even);
    fft(odd);

    // 合成
    for (int k = 0; k < N / 2; ++k) {
        std::complex<double> t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        a[k] = even[k] + t;
        a[k + N / 2] = even[k] - t;
    }
}