# 2.3. ローパスフィルタ

## 1. ローパスフィルタとは？

ここでは「通過域ができるだけ平坦」になるように設計されたバターワースフィルタについて説明します。

### バターワースフィルタの数式（アナログ）

#### 🔹 振幅特性（ゲイン）：

$$
|H(j\omega)| = \frac{1}{\sqrt{1 + \left( \frac{\omega}{\omega_c} \right)^{2n}}}
$$

- $\omega$：角周波数（rad/s）
- $\omega_c$：カットオフ周波数
- $n$：フィルタの次数（高いほど急峻）

---

#### 🔹 特徴

- 通過域 $(\omega < \omega_c)$ ではゲイン ≈ 1
- 遮断域 $(\omega > \omega_c)$ では急激に減衰
- $n$が大きいほど、**理想的なフィルタに近づく**！

---

### 💻 離散化(1次Butterworth LPF)

デジタルフィルタでは、Z変換を使って以下のような形になります：

$$
H(z) = \frac{Y(z)}{X(z)} = \frac{a_0 + a_1 z^{-1}}{1 + b_1 z^{-1}}
$$

これは、**入力 X(z)** に対して出力 **Y(z)** を得るためのフィルタの周波数応答を表します。

この係数 $a_0, a_1, b_1$ を**バターワース特性に合わせて設計**することで、デジタルバターワースフィルタが完成します。

---

### 🎯 1次のデジタルバターワースLPF（簡易版）

$$
y[n] = a_0 x[n] + a_1 x[n-1] - b_1 y[n-1]
$$

ここで：
- $x[n]$：現在の入力
- $x[n-1]$：1つ前の入力
- $y[n]$：現在の出力
- $y[n-1]$：1つ前の出力
- 係数$a_0, a_1, b_1$はカットオフ周波数とサンプリング周波数から計算

---

## 2. サンプルプログラム

![Hard copy LPF](./images/signal_lpf_01.png)

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

// 疑似的なハイパスフィルタ
class MockHPF {
public:
    MockHPF(int order, double cutoffFreq, double sampleRate) {
        for (int i = 0; i < order; ++i) {
            stages.emplace_back(cutoffFreq, sampleRate);
        }
    }

    double process(double input) {
        double output = input;
        for (auto& stage : stages) {
            output = stage.process(output);
        }
        // 元の値からローパスフィルタの値を引く
        return input - output;
    }

private:
    std::vector<ButterworthStage> stages;
};
```


## 3. 特徴
- order は偶数: ButterworthLPFは1次ステージを直列に並べて構成しているため、偶数次で設計すると対称性が保たれ、安定した特性が得られます。
- リップルがない: バターワースフィルタは通過域にリップル（波打ち）がなく、滑らかな減衰特性を持ちます。これはチェビシェフフィルタとの大きな違いです。
- 元の波形からローパスフィルタのアウトプットを引くと、疑似的なハイパスフィルタが得られます。

## 4. レポート課題
1. 異なる次数での周波数特性の違いを比較し、グラフで示せ。(二つ以上の曲線が含まれている事)
2. カットオフ周波数を変えて信号の通過具合を観察せよ。(二つ以上の曲線が含まれている事)
