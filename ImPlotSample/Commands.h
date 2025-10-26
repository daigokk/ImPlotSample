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
    static void getSinWF(WaveformParams* pWaveformParams, double times[], double waveform[]);

    static void getSquareWF(WaveformParams* pWaveformParams, double times[], double waveform[]);

    static void getSawWF(WaveformParams* pWaveformParams, double times[], double waveform[]);

    static void getTriangleWF(WaveformParams* pWaveformParams, double times[], double waveform[]);

    static bool saveWaveforms(const int size, const char* filename, const double xs[], const double* ya, const int channels, const char label[] = "Time (s)");

    static bool loadWaveform(const int size, const char* filename, double times[], double waveform[]);

    static double runPsd(WaveformParams* pWaveformParams, const double waveform[], double* pX, double* pY);

    static void runFft(WaveformParams* pWaveformParams, double waveform[], double freqs[], double amps[]);

    static void runLpf(WaveformParams* pWaveformParams, int order, double cutoffFreq, const double input[], double output[]);

    static void makeRanges(const int size, double start, double ranges[]);
};
