# 2.3. ローパスフィルタ

## 1. ローパスフィルタとは？

ここでは「通過域ができるだけ平坦」になるように設計されたバターワースフィルタについて説明します。

### 🌈 バターワースフィルタの数式（アナログ）

#### 🔹 振幅特性（ゲイン）：

$
|H(j\omega)| = \frac{1}{\sqrt{1 + \left( \frac{\omega}{\omega_c} \right)^{2n}}}
$

- \( \omega \)：角周波数（rad/s）
- \( \omega_c \)：カットオフ周波数
- \( n \)：フィルタの次数（高いほど急峻）

---

#### 🔹 特徴

- 通過域（\( \omega < \omega_c \)）ではゲイン ≈ 1
- 遮断域（\( \omega > \omega_c \)）では急激に減衰
- \( n \) が大きいほど、**理想的なフィルタに近づく**！

---

### 💻 デジタル化（双2次構造の例）

デジタルフィルタでは、Z変換を使って以下のような形になるよ：

\[
H(z) = \frac{a_0 + a_1 z^{-1} + a_2 z^{-2}}{1 + b_1 z^{-1} + b_2 z^{-2}}
\]

この係数 \( a_0, a_1, a_2, b_1, b_2 \) を**バターワース特性に合わせて設計**することで、デジタルバターワースフィルタが完成するんだ！

---

### 🎯 1次のデジタルバターワースLPF（簡易版）

\[
y[n] = a_0 x[n] + a_1 x[n-1] - b_1 y[n-1]
\]

- \( x[n] \)：入力信号
- \( y[n] \)：出力信号
- 係数はカットオフ周波数とサンプリング周波数から計算！

---

## 2. サンプルプログラム
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

## 4. レポート課題

