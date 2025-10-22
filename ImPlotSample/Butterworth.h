#pragma once

#include <cmath>
#include <vector>
#include <memory>
#include <stdexcept>

#define PI acos(-1.0)


class FirstOrderStage {
public:
    FirstOrderStage(double cutoffFreq, double sampleRate) {
        // プリワーピングを行い、正規化されたカットオフ角周波数を計算
        double wc_warped = tan(PI * cutoffFreq / sampleRate);

        // 双一次変換に基づいて係数を計算
        // 元の process メソッドの式: output = a0*x[n] + a1*x[n-1] - b1*y[n-1]
        // 理論上の差分方程式:      y[n]   = b0*x[n] + b1*x[n-1] - a1*y[n-1]
        // したがって、a0=b0, a1=b1, b1=a1 と対応させる
        a0 = wc_warped / (1.0 + wc_warped);
        a1 = a0;
        b1 = (wc_warped - 1.0) / (1.0 + wc_warped);

        // 状態変数を初期化
        prevInput = 0.0;
        prevOutput = 0.0;
    }

    double process(double input) {
        double output = a0 * input + a1 * prevInput - b1 * prevOutput;
        prevInput = input;
        prevOutput = output;
        return output;
    }

private:
    double a0, a1, b1;
    double prevInput, prevOutput;
};

/**
 * @class BiquadStage
 * @brief 2次IIRフィルタステージ
 */
class BiquadStage {
public:
    BiquadStage(double b0, double b1, double b2, double a1, double a2)
        : b0(b0), b1(b1), b2(b2), a1(a1), a2(a2),
        x1(0.0), x2(0.0), y1(0.0), y2(0.0) {
    }

    double process(double x0) {
        double y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        x2 = x1;
        x1 = x0;
        y2 = y1;
        y1 = y0;
        return y0;
    }

private:
    double b0, b1, b2, a1, a2;
    double x1, x2, y1, y2;
};

/**
 * @class ButterworthLPF
 * @brief バターワース・ローパスフィルタ
 */
class ButterworthLPF {
public:
    ButterworthLPF(int order, double cutoffFreq, double sampleRate) {
        if (order < 1) {
            throw std::invalid_argument("Order must be >= 1");
        }

        if (order % 2 == 1) {
            // 奇数次数 → 1次ステージ追加
            firstOrderStage = std::make_unique<FirstOrderStage>(cutoffFreq, sampleRate);
        }

        double warped = tan(PI * cutoffFreq / sampleRate);
        for (int k = 0; k < order / 2; ++k) {
            double theta = PI * (2.0 * k + 1.0) / (2.0 * order);
            double sinTheta = sin(theta);

            double a = 1.0 + 2.0 * warped * sinTheta + warped * warped;
            double b0 = warped * warped / a;
            double b1 = 2.0 * b0;
            double b2 = b0;
            double a1 = 2.0 * (warped * warped - 1.0) / a;
            double a2 = (1.0 - 2.0 * warped * sinTheta + warped * warped) / a;

            stages.emplace_back(b0, b1, b2, a1, a2);
        }
    }

    double process(double input) {
        double output = input;

        if (firstOrderStage) {
            output = firstOrderStage->process(output);
        }

        for (auto& stage : stages) {
            output = stage.process(output);
        }

        return output;
    }
private:
    std::unique_ptr<FirstOrderStage> firstOrderStage;
    std::vector<BiquadStage> stages;
};


class FirstOrderHPF {
public:
    FirstOrderHPF(double cutoffFreq, double sampleRate) {
        double wc = 2.0 * PI * cutoffFreq;
        double T = 1.0 / sampleRate;
        double alpha = 1.0 / (1.0 + wc * T);

        a0 = alpha;
        a1 = -alpha;
        b1 = 1.0 - alpha;

        // 高周波ゲインで正規化
        double hfGain = a0 - a1;
        a0 /= hfGain;
        a1 /= hfGain;

        x1 = 0.0;
        y1 = 0.0;
    }

    double process(double x0) {
        double y0 = a0 * x0 + a1 * x1 - b1 * y1;
        x1 = x0;
        y1 = y0;
        return y0;
    }

private:
    double a0, a1, b1;
    double x1, y1;
};

class FirstOrderStageHPF {
public:
    FirstOrderStageHPF(double cutoffFreq, double sampleRate) {
        double wc_warped = tan(PI * cutoffFreq / sampleRate);
        double norm = 1.0 / (1.0 + wc_warped);

        // HPFの係数を計算
        // H(z) = (b0 + b1*z^-1) / (1 + a1*z^-1)
        b0 = norm;
        b1 = -norm;
        a1 = (wc_warped - 1.0) * norm;

        x1 = 0.0;
        y1 = 0.0;
    }

    double process(double x0) {
        // 差分方程式: y[n] = b0*x[n] + b1*x[n-1] - a1*y[n-1]
        double y0 = b0 * x0 + b1 * x1 - a1 * y1;
        x1 = x0;
        y1 = y0;
        return y0;
    }

private:
    double b0, b1, a1;
    double x1, y1;
};


/**
 * @class ButterworthHPF
 * @brief バターワース・ハイパスフィルタ
 */
class ButterworthHPF {
public:
    ButterworthHPF(int order, double cutoffFreq, double sampleRate) {
        if (order < 1) {
            throw std::invalid_argument("Order must be >= 1");
        }

        if (order % 2 == 1) {
            // 奇数次数 → 1次HPFステージ追加
            firstOrderStage = std::make_unique<FirstOrderStageHPF>(cutoffFreq, sampleRate);
        }

        double warped = tan(PI * cutoffFreq / sampleRate);
        for (int k = 0; k < order / 2; ++k) {
            double theta = PI * (2.0 * k + 1.0) / (2.0 * order);
            double sinTheta = sin(theta);

            // 分母の係数はLPFと同じ
            double A0 = 1.0 + 2.0 * warped * sinTheta + warped * warped;
            double a1 = 2.0 * (warped * warped - 1.0) / A0;
            double a2 = (1.0 - 2.0 * warped * sinTheta + warped * warped) / A0;

            // 分子の係数をHPF用に変更
            double b0 = 1.0 / A0;
            double b1 = -2.0 / A0;
            double b2 = 1.0 / A0;

            stages.emplace_back(b0, b1, b2, a1, a2);
        }
    }

    double process(double input) {
        double output = input;

        if (firstOrderStage) {
            output = firstOrderStage->process(output);
        }

        for (auto& stage : stages) {
            output = stage.process(output);
        }

        return output;
    }
private:
    // 1次ステージはHPF用のものを使用
    std::unique_ptr<FirstOrderStageHPF> firstOrderStage;
    std::vector<BiquadStage> stages;
};