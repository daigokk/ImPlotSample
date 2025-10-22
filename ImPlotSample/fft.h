#pragma once

#include <vector>
#include <complex>
#define PI acos(-1)

void fft(std::vector<std::complex<double>>& a) {
    int N = a.size();
    if (N <= 1) return;

    // �����E��ɕ��� �z��̃T�C�Y�������ɂȂ�I�I�I
    std::vector<std::complex<double>> even(N / 2), odd(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        even[i] = a[i * 2];
        odd[i] = a[i * 2 + 1];
    }

    // �ċA�Ăяo��
    fft(even);
    fft(odd);

    // ����
    for (int k = 0; k < N / 2; ++k) {
        std::complex<double> t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        a[k] = even[k] + t;
        a[k + N / 2] = even[k] - t;
    }
}