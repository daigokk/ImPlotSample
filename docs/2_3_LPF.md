# 2.3. ローパスフィルタ

## 1. ローパスフィルタとは？

## 2. バターワースフィルタ
```cpp
#include <cmath>
#define PI acos(-1)

class ButterworthStage {
public:
    ButterworthStage(double cutoffFreq, double sampleRate) {
        double wc = 2 * M_PI * cutoffFreq;
        double T = 1.0 / sampleRate;
        double alpha = wc * T / (1.0 + wc * T);

        a0 = alpha;
        a1 = alpha;
        b1 = 1.0 - alpha;

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

class ButterworthLPF {
public:
    ButterworthLPF(int order, double cutoffFreq, double sampleRate) {
        for (int i = 0; i < order; ++i) {
            stages.emplace_back(cutoffFreq, sampleRate);
        }
    }

    double process(double input) {
        double output = input;
        for (auto& stage : stages) {
            output = stage.process(output);
        }
        return output;
    }

private:
    std::vector<ButterworthStage> stages;
};
```


## 3. 特徴
- order は偶数
- リップルがない

## 4. ポート課題

