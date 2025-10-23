#pragma once

#include <vector>


class Commands {
public:
    typedef struct WaveformParams {
        double amplitude = 0; // 振幅
        double phase_deg = 0; // 位相（度）
        double noize = 0;     // ノイズ振幅
        double dt = 0;        // サンプリング間隔
        double frequency = 0; // 周波数
        int size = 0;         // データ点数
    };
    static void getWaveform(WaveformParams* pWaveformParams, double* waveform);

    static bool saveWaveform(WaveformParams* pWaveformParams, const char* filename, const double* waveform);

    static bool saveWaveforms(WaveformParams* pWaveformParams, const char* filename, const double* freqs, const double ch1[1000], const double ch2[1000], const double ch3[1000]);

    static bool loadWaveform(WaveformParams* pWaveformParams, const char* filename, double* times, double* waveform);

    static double runPsd(WaveformParams* pWaveformParams, const double* waveform, double* pX, double* pY);

    static void runFft(WaveformParams* pWaveformParams, double* waveform, double* freqs, double* amps);

    static void runLpf(WaveformParams* pWaveformParams, int order, double cutoffFreq, const double* input, double* output);
};
