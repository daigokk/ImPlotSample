#pragma once

#include <vector>


class Commands {
public:
    typedef struct WaveformParams {
        double amplitude = 0; // �U��
        double phase_deg = 0; // �ʑ��i�x�j
        double noize = 0;     // �m�C�Y�U��
        double dt = 0;        // �T���v�����O�Ԋu
        double frequency = 0; // ���g��
        int size = 0;         // �f�[�^�_��
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
