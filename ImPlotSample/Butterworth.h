#pragma once

#include <cmath>
#include <vector>

#define PI acos(-1)

class FirstOrderStage {
public:
    FirstOrderStage(double cutoffFreq, double sampleRate) {
        double wc = 2.0 * PI * cutoffFreq;
        double T = 1.0 / sampleRate;
        double alpha = wc * T / (1.0 + wc * T);

        a0 = alpha;
        a1 = alpha;
        b1 = 1.0 - alpha;

        // DCÉQÉCÉìê≥ãKâª
        double dcGain = a0 + a1;
        a0 /= dcGain;
        a1 /= dcGain;

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

class ButterworthLPF {
public:
    ButterworthLPF(int order, double cutoffFreq, double sampleRate) {
        if (order < 1) {
            throw std::invalid_argument("Order must be >= 1");
        }

        if (order % 2 == 1) {
            // äÔêîéüêî Å® 1éüÉXÉeÅ[ÉWí«â¡
            firstOrderStage = std::make_unique<FirstOrderStage>(cutoffFreq, sampleRate);
        }

        double warped = tan(PI * cutoffFreq / sampleRate);
        for (int k = 0; k < order / 2; ++k) {
            double theta = PI * (2.0 * k + 1.0) / (2.0 * order);
            double sinTheta = sin(theta);
            double cosTheta = cos(theta);

            double a = 1.0 + 2.0 * warped * sinTheta + warped * warped;
            double b0 = warped * warped / a;
            double b1 = 2.0 * warped * warped / a;
            double b2 = warped * warped / a;
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



#include <cmath>
#include <vector>
#include <memory>
#include <stdexcept>

#define PI acos(-1)

class FirstOrderHPF {
public:
    FirstOrderHPF(double cutoffFreq, double sampleRate) {
        double wc = 2.0 * PI * cutoffFreq;
        double T = 1.0 / sampleRate;
        double alpha = 1.0 / (1.0 + wc * T);

        a0 = alpha;
        a1 = -alpha;
        b1 = 1.0 - alpha;

        // çÇé¸îgÉQÉCÉìÇ≈ê≥ãKâª
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

class BiquadHPF {
public:
    BiquadHPF(double b0, double b1, double b2, double a1, double a2)
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

class ButterworthHPF {
public:
    ButterworthHPF(int order, double cutoffFreq, double sampleRate) {
        if (order < 1) throw std::invalid_argument("Order must be >= 1");

        double warped = tan(PI * cutoffFreq / sampleRate);

        if (order % 2 == 1) {
            firstStage = std::make_unique<FirstOrderHPF>(cutoffFreq, sampleRate);
        }

        for (int k = 0; k < order / 2; ++k) {
            double theta = PI * (2.0 * k + 1.0) / (2.0 * order);
            double sinTheta = sin(theta);
            double cosTheta = cos(theta);

            double a = 1.0 + 2.0 * warped * sinTheta + warped * warped;
            double b0 = 1.0 / a;
            double b1 = -2.0 / a;
            double b2 = 1.0 / a;
            double a1 = 2.0 * (warped * warped - 1.0) / a;
            double a2 = (1.0 - 2.0 * warped * sinTheta + warped * warped) / a;

            stages.emplace_back(b0, b1, b2, a1, a2);
        }
    }

    double process(double input) {
        double output = input;
        if (firstStage) output = firstStage->process(output);
        for (auto& stage : stages) {
            output = stage.process(output);
        }
        return output;
    }

private:
    std::unique_ptr<FirstOrderHPF> firstStage;
    std::vector<BiquadHPF> stages;
};
