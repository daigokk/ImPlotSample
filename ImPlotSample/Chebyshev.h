#pragma once

#include <cmath>
#include <complex>
#include <vector>
#include <numeric> 
#include <stdexcept> // エラーハンドリング用
#include <iostream>  // デバッグ・警告用

#define PI acos(-1.0)

class ChebyshevLPF {
public:
    ChebyshevLPF(int order, double cutoffFreq, double sampleRate, double ripple_dB)
        : n(order), fc(cutoffFreq), fs(sampleRate) {

        // --- 1. 入力パラメータの検証 ---
        if (n <= 0) {
            throw std::invalid_argument("Filter order must be positive.");
        }
        if (fs <= 0) {
            throw std::invalid_argument("Sample rate must be positive.");
        }
        // カットオフ周波数は 0 < fc < fs/2 の範囲内である必要がある
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

        // --- 2. プリワーピングの計算と検証 ---
        double wc = 2.0 * PI * fc;
        double T = 1.0 / fs;
        double tan_arg = wc * T / 2.0;

        // tan_arg は 0 < tan_arg < pi/2 の範囲にあるはず
        if (tan_arg >= PI / 2.0) {
            // 通常は発生しないが、数値誤差で発生する可能性
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

        // --- 3. NaN/Inf チェックと状態リセット ---
        // フィルタが不安定になった場合 (outputがNaN/Infになる)
        if (std::isnan(output) || std::isinf(output)) {
            // 状態をリセットして発散を防ぐ
            std::fill(x.begin(), x.end(), 0.0);
            std::fill(y.begin(), y.end(), 0.0);
            return 0.0; // 安全な値 (0.0) を返す
        }

        // 状態変数の更新
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

        // --- 1. アナログ極の計算 ---
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

        // --- 2. 双一次変換 ---
        std::vector<std::complex<double>> digitalPoles;
        for (const auto& s : analogPoles) {
            std::complex<double> z_den = 1.0 - s * T / 2.0;
            if (std::abs(z_den) < 1e-10) { // ほぼゼロ除算
                throw std::runtime_error("Bilinear transform denominator is near zero. Check parameters.");
            }
            std::complex<double> z_num = 1.0 + s * T / 2.0;
            digitalPoles.push_back(z_num / z_den);
        }

        // --- 3. 分母係数 a[i] の計算 (ロジックは変更なし) ---
        // このロジックは n=2 (複素共役ペア) でも n=1 (実数極) でも正しく機能する
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

        // --- 4. 分子係数 b[i] の計算 (変更なし) ---
        b[0] = 1.0;
        for (int i = 1; i <= n; ++i) {
            b[i] = b[i - 1] * (n - i + 1.0) / i;
        }

        // --- 5. DCゲインの調整 (検証強化) ---
        double b_sum = std::accumulate(b.begin(), b.end(), 0.0);
        double a_sum = std::accumulate(a.begin(), a.end(), 0.0);

        // b_sum は (1+1)^n = 2^n となるため、n>0 なら 0 にはならない
        if (std::abs(b_sum) < 1e-30) {
            throw std::runtime_error("Denominator 'b_sum' for gain scaling is zero.");
        }
        // a_sum は fc -> 0 のときに 0 に近づく
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