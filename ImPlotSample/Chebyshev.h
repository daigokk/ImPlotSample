#pragma once

#include <cmath>
#include <complex>
#include <vector>
#include <iostream>

#define PI acos(-1.0)

class ChebyshevLPF {
public:
    ChebyshevLPF(int order, double cutoffFreq, double sampleRate, double ripple)
        : n(order), fc(cutoffFreq), fs(sampleRate), epsilon(ripple) {
        designFilter();
    }

    double process(double input) {
        x[0] = input;
        double output = 0.0;
        for (int i = 0; i <= n; ++i)
            output += b[i] * x[i];
        for (int i = 1; i <= n; ++i)
            output -= a[i] * y[i];
        for (int i = n; i > 0; --i) {
            x[i] = x[i - 1];
            y[i] = y[i - 1];
        }
        y[0] = output;
        return output;
    }

private:
    int n;
    double fc, fs, epsilon;
    std::vector<double> a, b, x, y;

    void designFilter() {
        a.resize(n + 1, 0.0);
        b.resize(n + 1, 0.0);
        x.resize(n + 1, 0.0);
        y.resize(n + 1, 0.0);

        // ���K�����g��
        double T = 1.0 / fs;
        double omega_c = 2 * PI * fc;

        // �ɂ̌v�Z�i�A�i���O�̈�j
        std::vector<std::complex<double>> analogPoles;
        double beta = asinh(1.0 / epsilon) / n;

        for (int k = 1; k <= n; ++k) {
            double theta = PI * (2.0 * k - 1) / (2.0 * n);
            double sigma = -sinh(beta) * sin(theta);
            double omega = cosh(beta) * cos(theta);
            analogPoles.push_back(std::complex<double>(sigma, omega));
        }

        // �o�ꎟ�ϊ��is �� z�j
        std::vector<std::complex<double>> digitalPoles;
        for (auto& s : analogPoles) {
            std::complex<double> z = (1.0 + s * T / 2.0) / (1.0 - s * T / 2.0);
            digitalPoles.push_back(z);
        }

        // �W���v�Z�i�ȈՁF���ۂ�z�̐ς���W���𓱏o�j
        // �����ł͉��̌W����ݒ�
        b[0] = 1.0;
        for (int i = 1; i <= n; ++i) {
            a[i] = -2.0 * real(digitalPoles[i - 1]); // �ȈՂȋߎ�
        }
    }
};