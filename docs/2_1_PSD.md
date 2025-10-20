# 2.2. 位相敏感検波

## 1. 位相敏感検波とは？
位相感応検出 (PSD) または同期検出として知られる技術は、測定波形から既知の周波数の波形の振幅を求めるアルゴリズムです。

この検出技術の原理を下図に示します。[Youtube](https://www.youtube.com/watch?v=pHyuB1YW4qY)でも解説しています。

![PSD](https://github.com/daigokk/LIA/blob/master/docs/images/PSD.png?raw=true)

Amplitude: $A=\sqrt{x^2+y^2}$, Phase: $\theta=\arctan{\frac{y}{x}}$

## 2. サンプルプログラム
```cpp
#include <cmath>

#define PI acos(-1)

void psd(const double arr[], const double freq, const double dt, const int size, double* pX, double* pY) {
    *pX = 0; *pY = 0;
    // 掛け算
    for (int i = 0; i < size; i++) {
        double wt = 2 * PI * freq * dt * i;
        *pX += arr[i] * sin(wt);
        *pY += arr[i] * cos(wt);
    }
    // ローパスフィルタの代わりに平均を用いる
    *pX /= size;
    *pY /= size;
}
```

- `arr[]`: 入力信号（測定波形）
- `freq`: 検出したい周波数（Hz）
- `dt`: サンプリング間隔（秒）
- `size`: サンプル数
- `pX`, `pY`: 出力される直交成分のポインタ
- 平均を取ることで高周波成分($2\omega$)が打ち消され、ローパスフィルタの代替として機能します。
