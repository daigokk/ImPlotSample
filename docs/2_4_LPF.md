# 2.3. ローパスフィルタ

## 1. ローパスフィルタとは？

ここでは「通過域ができるだけ平坦」になるように設計されたButterworthフィルタについて説明します。

### Butterworthフィルタの数式（アナログ）

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

デジタルフィルタでは、Z変換を使うと以下のように表せます：

$$
H(z) = \frac{Y(z)}{X(z)} = \frac{a_0 + a_1 z^{-1}}{1 + b_1 z^{-1}}
$$

これは、入力 **X(z)** に対して出力 **Y(z)** を得るためのフィルタの周波数応答を表します。

この係数 $a_0, a_1, b_1$ を**Butterworth特性に合わせて設計**することで、デジタルButterworthフィルタが完成します。

---

### 🎯 1次のデジタル Butterworth LPF（簡易版）

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

class FirstOrderStage {
public:
    FirstOrderStage(double cutoffFreq, double sampleRate) {
        // プリワーピングを行い、正規化されたカットオフ角周波数を計算
        double wc_warped = tan(PI * cutoffFreq / sampleRate);

        // 双一次変換に基づいて係数を計算
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
```


## 3. 特徴
- リップルがない: Butterworthフィルタは通過域にリップル（波打ち）がなく、滑らかな減衰特性を持ちます。これはChebyshevフィルタとの大きな違いです。
- 元の波形からローパスフィルタのアウトプットを引くと、疑似的なハイパスフィルタが得られます。

## 4. レポート課題
1. カットオフ周波数100kHzとして、10kHzから1000kHzまでの周波数特性をボード線図(Gainと位相)で示せ。
   ```cpp
   void ShowWindow3(const char title[]) {
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 220 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 530 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    static double freqs[1000] = { 0 }, gains[3][1000] = { 0 }, phases[3][1000];
    static Commands::WaveformParams wfp;
    static std::string text = "";
    if (ImGui::Button("Run")) {
        // ボタンが押されたらここが実行される
        /*** 適切なコードを入力 ***************************************/
        wfp.amplitude = 1;
        wfp.dt = DT;
        wfp.size = SIZE;
        wfp.frequency = 10e3;
        // 周波数特性
        for (int j = 0; j < 3; j++) {
            wfp.frequency = 10e3;
            for (int i = 0; i < 1000; i++) {
                double waveform[SIZE] = { 0 };
                double x = 0, y = 0, wf_lpf[SIZE];
                Commands::getWaveform(&wfp, waveform);
                Commands::runLpf(&wfp, j+1, 100e3, waveform, wf_lpf);
                freqs[i] = wfp.frequency;
                gains[j][i] = 20.0 * log10(Commands::runPsd(&wfp, wf_lpf, &x, &y) / wfp.amplitude);
                phases[j][i] = atan2(y, x) / PI * 180;
                if (phases[j][i] > 0) phases[j][i] -= 360;
                wfp.frequency += 1e3;
            }
        }
        text = "[Error] Failed to open file for writing.";
        wfp.size = 1000;
        if (Commands::saveWaveforms(&wfp, "bode_gain.csv", freqs, gains[0], gains[1], gains[2])) {
            if (Commands::saveWaveforms(&wfp, "bode_phase.csv", freqs, phases[0], phases[1], phases[2])) {
                text = "Success.";
            }
        }
        /*** ここまで *************************************************/
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Gain", ImVec2(-1, 225 * Gui::monitorScale))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
        ImPlot::SetupAxis(ImAxis_Y1, "Gain (dB)");
        for (int j = 0; j < 3; j++) {
            std::string label = "Order " + std::to_string(j + 1);
            ImPlot::PlotLine(label.c_str(), freqs, gains[j], 1000);
        }
        ImPlot::EndPlot();
    }
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Phase", ImVec2(-1, 225 * Gui::monitorScale))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
        ImPlot::SetupAxis(ImAxis_Y1, "Phase (Deg.)");
        for (int j = 0; j < 3; j++) {
            std::string label = "Order " + std::to_string(j + 1);
            ImPlot::PlotLine(label.c_str(), freqs, phases[j], 1000);
        }
        ImPlot::EndPlot();
    }
    // ウィンドウ終了
    ImGui::End();
   }
   ```
   ![Hard copy](./images/HardCopy.png)
3. 異なる次数での周波数特性の違いを比較し、グラフで示せ。(二つ以上の曲線が含まれている事)
4. カットオフ周波数を変えて信号の通過具合を観察せよ。(二つ以上の曲線が含まれている事)
