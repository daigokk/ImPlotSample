# 2.3. ローパスフィルタ

## 1. ローパスフィルタとは？

ここでは「通過域ができるだけ平坦」になるように設計されたバターワースフィルタについて説明します。

### バターワースフィルタの数式（アナログ）

#### 🔹 振幅特性（ゲイン）：

```math
|H(j\omega)| = \frac{1}{\sqrt{1 + \left( \frac{\omega}{\omega_c} \right)^{2n}}}
```

- $\omega$：角周波数（rad/s）
- $\omega_c$：カットオフ周波数
- $n$：フィルタの次数（高いほど急峻）

---

#### 🔹 特徴

- 通過域 $(\omega < \omega_c)$ ではゲイン ≈ 1
- 遮断域 $(\omega > \omega_c)$ では急激に減衰
- $n$が大きいほど、**理想的なフィルタに近づく**！

---

### 💻 デジタル化（双2次構造の例）

デジタルフィルタでは、Z変換を使って以下のような形になります：

```math
H(z) = \frac{a_0 + a_1 z^{-1} + a_2 z^{-2}}{1 + b_1 z^{-1} + b_2 z^{-2}}
```

この係数 $a_0, a_1, a_2, b_1, b_2$ を**バターワース特性に合わせて設計**することで、デジタルバターワースフィルタが完成します。

---

### 🎯 1次のデジタルバターワースLPF（簡易版）

```math
y[n] = a_0 x[n] + a_1 x[n-1] - b_1 y[n-1]
```

- $x[n]$：入力信号
- $y[n]$：出力信号
- 係数はカットオフ周波数とサンプリング周波数から計算

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
1. 異なる次数での周波数特性の違いを比較し、グラフで示せ。(二つ以上の次数のグラフを作成する事)
2. カットオフ周波数を変えて信号の通過具合を観察せよ。(二つ以上のカットオフ周波数を用いてグラフを作成する事)
