#pragma once

#include <vector>
#include "fft.h"
#include "Butterworth.h"


class Commands {
public:
    typedef struct WaveformParams {
        double amplitude; // �U��
        double phase_deg; // �ʑ��i�x�j
        double noize;     // �m�C�Y�U��
        double dt;        // �T���v�����O�Ԋu
        double frequency; // ���g��
        int size;         // �f�[�^�_��
    };
    static void getWaveform(WaveformParams* pWaveformParams, double* waveform) {
        double phase_rad = pWaveformParams->phase_deg * PI / 180.0;
        srand(time(NULL));
        for (int i = 0; i < pWaveformParams->size; i++) {
            waveform[i] = pWaveformParams->amplitude * std::sin(2 * PI * pWaveformParams->frequency * i * pWaveformParams->dt + phase_rad);
            waveform[i] += (double)rand() / RAND_MAX * 2 * pWaveformParams->noize - pWaveformParams->noize; // �ǉ�
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
            // 1�s�ڂ͖�������
            fgets(buf, sizeof(buf), fp);  // 1�s�ڂ�ǂݔ�΂�
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
        // �|���Z
        for (int i = 0; i < pWaveformParams->size; i++) {
            double wt = 2 * PI * pWaveformParams->frequency * pWaveformParams->dt * i;
            *pX += waveform[i] * 2 * sin(wt);
            *pY += waveform[i] * 2 * cos(wt);
        }
        // ���[�p�X�t�B���^�̑���ɕ��ς�p����
        *pX /= pWaveformParams->size;
        *pY /= pWaveformParams->size;
    }

    static void runFft(WaveformParams* pWaveformParams, double* waveform, double* freqs, double* amps) {
        // FFT�����������Ɏ���
        std::vector<std::complex<double>> data(pWaveformParams->size);
        for (int i = 0; i < pWaveformParams->size; ++i) {
            data[i] = std::complex<double>(waveform[i], 0.0);
        }
        fft(data);
        for (int i = 0; i < pWaveformParams->size; ++i) {
            freqs[i] = (double)i / (pWaveformParams->dt * pWaveformParams->size); // ���g�����ɕϊ�
            amps[i] = std::abs(data[i]) / pWaveformParams->size; // �U���X�y�N�g���ɕϊ�
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
