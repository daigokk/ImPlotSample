#pragma once

#include <cmath>
#include <complex>
#include <vector>
#include <numeric> 
#include <stdexcept> // �G���[�n���h�����O�p
#include <iostream>  // �f�o�b�O�E�x���p

#define PI acos(-1.0)

class ChebyshevLPF {
public:
    ChebyshevLPF(int order, double cutoffFreq, double sampleRate, double ripple_dB)
        : n(order), fc(cutoffFreq), fs(sampleRate) {

        // --- 1. ���̓p�����[�^�̌��� ---
        if (n <= 0) {
            throw std::invalid_argument("Filter order must be positive.");
        }
        if (fs <= 0) {
            throw std::invalid_argument("Sample rate must be positive.");
        }
        // �J�b�g�I�t���g���� 0 < fc < fs/2 �͈͓̔��ł���K�v������
        if (fc <= 0 || fc >= fs / 2.0) {
            throw std::invalid_argument("Cutoff frequency must be strictly between 0 and fs/2.");
        }
        if (ripple_dB <= 0) {
            throw std::invalid_argument("Ripple must be positive (dB).");
        }

        epsilon = std::sqrt(std::pow(10.0, ripple_dB / 10.0) - 1.0);
        if (std::isnan(epsilon) || epsilon == 0) {
            throw std::runtime_error("Invalid epsilon calculation from ripple_dB.");
        }

        // --- 2. �v�����[�s���O�̌v�Z�ƌ��� ---
        double wc = 2.0 * PI * fc;
        double T = 1.0 / fs;
        double tan_arg = wc * T / 2.0;

        // tan_arg �� 0 < tan_arg < pi/2 �͈̔͂ɂ���͂�
        if (tan_arg >= PI / 2.0) {
            // �ʏ�͔������Ȃ����A���l�덷�Ŕ�������\��
            throw std::runtime_error("Prewarping argument error (>= PI/2).");
        }

        prewarp_wc = (2.0 / T) * std::tan(tan_arg);
        if (std::isnan(prewarp_wc) || std::isinf(prewarp_wc)) {
            throw std::runtime_error("Prewarping calculation failed (NaN/Inf). Check fc near fs/2.");
        }

        designFilter();
    }

    double process(double input) {
        x[0] = input;
        double output = 0.0;

        for (int i = 0; i <= n; ++i)
            output += b[i] * x[i];
        for (int i = 1; i <= n; ++i)
            output -= a[i] * y[i];

        // --- 3. NaN/Inf �`�F�b�N�Ə�ԃ��Z�b�g ---
        // �t�B���^���s����ɂȂ����ꍇ (output��NaN/Inf�ɂȂ�)
        if (std::isnan(output) || std::isinf(output)) {
            // ��Ԃ����Z�b�g���Ĕ��U��h��
            std::fill(x.begin(), x.end(), 0.0);
            std::fill(y.begin(), y.end(), 0.0);
            return 0.0; // ���S�Ȓl (0.0) ��Ԃ�
        }

        // ��ԕϐ��̍X�V
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
    double prewarp_wc;
    std::vector<double> a, b, x, y;

    void designFilter() {
        a.resize(n + 1, 0.0);
        b.resize(n + 1, 0.0);
        x.resize(n + 1, 0.0);
        y.resize(n + 1, 0.0);

        double T = 1.0 / fs;

        // --- 1. �A�i���O�ɂ̌v�Z ---
        std::vector<std::complex<double>> analogPoles;
        double asinh_val = std::asinh(1.0 / epsilon);

        double beta = asinh_val / n;
        double sinh_beta = std::sinh(beta);
        double cosh_beta = std::cosh(beta);

        for (int k = 1; k <= n; ++k) {
            double theta = PI * (2.0 * k - 1.0) / (2.0 * n);
            double sigma_norm = -sinh_beta * std::sin(theta);
            double omega_norm = cosh_beta * std::cos(theta);

            std::complex<double> s_k(sigma_norm * prewarp_wc, omega_norm * prewarp_wc);
            analogPoles.push_back(s_k);
        }

        // --- 2. �o�ꎟ�ϊ� ---
        std::vector<std::complex<double>> digitalPoles;
        for (const auto& s : analogPoles) {
            std::complex<double> z_den = 1.0 - s * T / 2.0;
            if (std::abs(z_den) < 1e-10) { // �قڃ[�����Z
                throw std::runtime_error("Bilinear transform denominator is near zero. Check parameters.");
            }
            std::complex<double> z_num = 1.0 + s * T / 2.0;
            digitalPoles.push_back(z_num / z_den);
        }

        // --- 3. ����W�� a[i] �̌v�Z (���W�b�N�͕ύX�Ȃ�) ---
        // ���̃��W�b�N�� n=2 (���f�����y�A) �ł� n=1 (������) �ł��������@�\����
        std::vector<std::complex<double>> a_poly_complex = { 1.0 };

        for (const auto& p_k : digitalPoles) {
            std::vector<std::complex<double>> current_factor = { 1.0, -p_k };
            int size1 = a_poly_complex.size();
            int size2 = current_factor.size();
            std::vector<std::complex<double>> next_poly(size1 + size2 - 1, 0.0);

            for (int i = 0; i < size1; ++i) {
                for (int j = 0; j < size2; ++j) {
                    next_poly[i + j] += a_poly_complex[i] * current_factor[j];
                }
            }
            a_poly_complex = next_poly;
        }

        for (int i = 0; i <= n; ++i) {
            a[i] = std::real(a_poly_complex[i]);
            if (std::isnan(a[i]) || std::isinf(a[i])) {
                throw std::runtime_error("Coefficient 'a' calculation resulted in NaN/Inf.");
            }
        }

        // --- 4. ���q�W�� b[i] �̌v�Z (�ύX�Ȃ�) ---
        b[0] = 1.0;
        for (int i = 1; i <= n; ++i) {
            b[i] = b[i - 1] * (n - i + 1.0) / i;
        }

        // --- 5. DC�Q�C���̒��� (���؋���) ---
        double b_sum = std::accumulate(b.begin(), b.end(), 0.0);
        double a_sum = std::accumulate(a.begin(), a.end(), 0.0);

        // b_sum �� (1+1)^n = 2^n �ƂȂ邽�߁An>0 �Ȃ� 0 �ɂ͂Ȃ�Ȃ�
        if (std::abs(b_sum) < 1e-30) {
            throw std::runtime_error("Denominator 'b_sum' for gain scaling is zero.");
        }
        // a_sum �� fc -> 0 �̂Ƃ��� 0 �ɋ߂Â�
        if (std::isnan(a_sum) || std::isinf(a_sum)) {
            throw std::runtime_error("'a_sum' for gain scaling is NaN/Inf.");
        }

        double gain_scale = a_sum / b_sum;
        if (std::isnan(gain_scale) || std::isinf(gain_scale)) {
            throw std::runtime_error("Gain scale calculation resulted in NaN/Inf. Check fc near 0.");
        }

        for (int i = 0; i <= n; ++i) {
            b[i] *= gain_scale;
        }
    }
};