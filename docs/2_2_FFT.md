# FFT(高速フーリエ変換)

MITが作った世界最速のFFTライブラリとも言われるFFTWを使います。

## 1. FFTWってなに？
FFTWは、**高速フーリエ変換（FFT）**を行うためのC言語ライブラリ。信号処理やスペクトル解析など、電気系の分野でよく使われます。

---

## 2. 基本的な使い方（1次元FFT）

```c
#include <fftw3.h>
#include <stdio.h>
#include <math.h>

int main() {
    int N = 1024;               // サンプル数
    double Fs = 1000.0;         // サンプリング周波数（Hz）
    double in[N];
    fftw_complex out[N];
    fftw_plan p;

    // 入力信号（例：50Hzのサイン波）
    for (int i = 0; i < N; i++) {
        in[i] = sin(2 * M_PI * 50 * i / Fs);
    }

    // FFTプラン作成
    p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
    fftw_execute(p);

    // 周波数配列の作成と振幅表示
    for (int i = 0; i < N/2; i++) {
        double freq = i * Fs / N; // 周波数軸
        double mag = sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
        printf("Freq: %6.2f Hz, Magnitude: %f\n", freq, mag);
    }

    fftw_destroy_plan(p);
    fftw_cleanup();
    return 0;
}
```

---

## 3. よく使う関数

| 関数名 | 説明 |
|--------|------|
| `fftw_plan_dft_r2c_1d` | 実数→複素数の1次元FFT |
| `fftw_execute` | FFTの実行 |
| `fftw_destroy_plan` | プランの破棄 |
| `fftw_cleanup` | メモリの解放 |

---

## 4. 応用例や注意点

- データサイズは2のべき乗が高速！
- `FFTW_MEASURE` フラグで最適化されたプランを作れる
- 多次元FFTや逆変換も可能！
