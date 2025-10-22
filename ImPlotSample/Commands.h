#pragma once

#include <vector>
#include "fft.h"
#include "Butterworth.h"


class Commands {
public:
    typedef struct WaveformParams {
        double amplitude; // 振幅
        double phase_deg; // 位相（度）
        double noize;     // ノイズ振幅
        double dt;        // サンプリング間隔
        double frequency; // 周波数
        int size;         // データ点数
    };
    static void getWaveform(WaveformParams* pWaveformParams, double* waveform) {
        double phase_rad = pWaveformParams->phase_deg * PI / 180.0;
        srand(time(NULL));
        for (int i = 0; i < pWaveformParams->size; i++) {
            waveform[i] = pWaveformParams->amplitude * std::sin(2 * PI * pWaveformParams->frequency * i * pWaveformParams->dt + phase_rad);
            waveform[i] += (double)rand() / RAND_MAX * 2 * pWaveformParams->noize - pWaveformParams->noize; // 追加
        }
    }

    static bool saveWaveform(WaveformParams* pWaveformParams, const char* filename, const double* waveform) {
        FILE* fp = fopen(filename, "w");
        if (fp != NULL) {
            fprintf(fp, "# Time (s), Voltage (V)\n");
            for (int i = 0; i < pWaveformParams->size; ++i) {
                fprintf(fp, "%e, %e\n", i * pWaveformParams->dt, waveform[i]);
            }
            fclose(fp);
            return true;
        }
        else {
            return false;
        }
    }

    static bool loadWaveform(WaveformParams* pWaveformParams, const char* filename, double* times, double* waveform) {
        FILE* fp = fopen(filename, "r");
        char buf[256];
        if (fp != NULL) {
            // 1行目は無視する
            fgets(buf, sizeof(buf), fp);  // 1行目を読み飛ばす
            for (int i = 0; i < pWaveformParams->size; i++) {
                fscanf(fp, "%lf,%lf", &times[i], &waveform[i]);
            }
            fclose(fp);
            return true;
        }
        else {
            return false;
        }
    }

    static void psd(WaveformParams* pWaveformParams, const double* waveform, double* pX, double* pY) {
        *pX = 0; *pY = 0;
        // 掛け算
        for (int i = 0; i < pWaveformParams->size; i++) {
            double wt = 2 * PI * pWaveformParams->frequency * pWaveformParams->dt * i;
            *pX += waveform[i] * 2 * sin(wt);
            *pY += waveform[i] * 2 * cos(wt);
        }
        // ローパスフィルタの代わりに平均を用いる
        *pX /= pWaveformParams->size;
        *pY /= pWaveformParams->size;
    }

    static void runFft(WaveformParams* pWaveformParams, double* waveform, double* freqs, double* amps) {
        // FFT処理をここに実装
        std::vector<std::complex<double>> data(pWaveformParams->size);
        for (int i = 0; i < pWaveformParams->size; ++i) {
            data[i] = std::complex<double>(waveform[i], 0.0);
        }
        fft(data);
        for (int i = 0; i < pWaveformParams->size; ++i) {
            freqs[i] = (double)i / (pWaveformParams->dt * pWaveformParams->size); // 周波数軸に変換
            amps[i] = std::abs(data[i]) / pWaveformParams->size; // 振幅スペクトルに変換
        }
        ImPlot::SetNextAxesToFit();
    }

    static void runLpf(WaveformParams* pWaveformParams, int order, double cutoffFreq, const double* input, double* output) {
        ButterworthLPF lpf(order, cutoffFreq, 1.0 / pWaveformParams->dt);
        for (int i = 0; i < pWaveformParams->size; i++) {
            output[i] = lpf.process(input[i]);
        }
    }
};
