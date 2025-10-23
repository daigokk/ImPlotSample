#pragma once

#include <vector>
#include "fft.h"
#include "Butterworth.h"


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
    static void getWaveform(WaveformParams* pWaveformParams, double* waveform) {
		if (pWaveformParams->size <= 0) throw std::runtime_error("size��0�ȊO�̒l�������Ă��������B");
		if (pWaveformParams->dt <= 0) throw std::runtime_error("dt��0�ȊO�̒l�������Ă��������B");
		if (pWaveformParams->frequency < 0) throw std::runtime_error("frequency��0�ȏ�̒l�������Ă��������B");
        /*** �������� *************************************************/
        double phase_rad = pWaveformParams->phase_deg * PI / 180.0;
        srand(time(NULL));
        for (int i = 0; i < pWaveformParams->size; i++) {
            waveform[i] = pWaveformParams->amplitude * std::sin(2 * PI * pWaveformParams->frequency * i * pWaveformParams->dt + phase_rad);
            waveform[i] += (double)rand() / RAND_MAX * 2 * pWaveformParams->noize - pWaveformParams->noize; // �ǉ�
        }
        /*** �����܂� *************************************************/
    }

    static bool saveWaveform(WaveformParams* pWaveformParams, const char* filename, const double* waveform) {
        if (pWaveformParams->size <= 0) throw std::runtime_error("size��0�ȊO�̒l�������Ă��������B");
        if (pWaveformParams->dt <= 0) throw std::runtime_error("dt��0�ȊO�̒l�������Ă��������B");
        /*** �������� *************************************************/
        FILE* fp = fopen(filename, "w");
        if (fp != NULL) {
            fprintf(fp, "# Time (s), Voltage (V)\n");
            for (int i = 0; i < pWaveformParams->size; ++i) {
                fprintf(fp, "%e, %e\n", i * pWaveformParams->dt, waveform[i]);
            }
            fclose(fp);
            return true;
        }
        /*** �����܂� *************************************************/
        return false;
    }

    static bool saveWaveforms(WaveformParams* pWaveformParams, const char* filename, const double* freqs, const double ch1[1000], const double ch2[1000], const double ch3[1000]) {
        if (pWaveformParams->size <= 0) throw std::runtime_error("size��0�ȊO�̒l�������Ă��������B");
        /*** �������� *************************************************/
        FILE* fp = fopen(filename, "w");
        if (fp != NULL) {
            fprintf(fp, "# Freq. (Hz), ch1, ch2, ch3\n");
            for (int i = 0; i < pWaveformParams->size; ++i) {
                fprintf(fp, "%e, %e, %e, %e\n", freqs[i], ch1[i], ch2[i], ch3[i]);
            }
            fclose(fp);
            return true;
        }
        /*** �����܂� *************************************************/
        return false;
    }

    static bool loadWaveform(WaveformParams* pWaveformParams, const char* filename, double* times, double* waveform) {
        if (pWaveformParams->size <= 0) throw std::runtime_error("size��0�ȊO�̒l�������Ă��������B");
        /*** �������� *************************************************/
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
        /*** �����܂� *************************************************/
        return false;
    }

    static double runPsd(WaveformParams* pWaveformParams, const double* waveform, double* pX, double* pY) {
        if (pWaveformParams->size <= 0) throw std::runtime_error("size��0�ȊO�̒l�������Ă��������B");
        if (pWaveformParams->dt <= 0) throw std::runtime_error("dt��0�ȊO�̒l�������Ă��������B");
        if (pWaveformParams->frequency < 0) throw std::runtime_error("frequency��0�ȏ�̒l�������Ă��������B");
        *pX = 0; *pY = 0;
        /*** �������� *************************************************/
        // ������1/2�̐����{�̓_���Ōv�Z
        size_t halfPeriodSamples = static_cast<size_t>(1.0 / (pWaveformParams->frequency * 2 * pWaveformParams->dt));
        size_t usableSize = halfPeriodSamples * (int)(pWaveformParams->size / halfPeriodSamples);
        // �|���Z
        for (int i = 0; i < usableSize; i++) {
            double wt = 2 * PI * pWaveformParams->frequency * pWaveformParams->dt * i;
            *pX += waveform[i] * 2 * sin(wt);
            *pY += waveform[i] * 2 * cos(wt);
        }
        // ���[�p�X�t�B���^�̑���ɕ��ς�p����
		*pX /= usableSize;
        *pY /= usableSize;
        /*** �����܂� *************************************************/
		return sqrt((*pX) * (*pX) + (*pY) * (*pY));
    }

    static void runFft(WaveformParams* pWaveformParams, double* waveform, double* freqs, double* amps) {
        if (pWaveformParams->size <= 0) throw std::runtime_error("size��0�ȊO�̒l�������Ă��������B");
        if (pWaveformParams->dt <= 0) throw std::runtime_error("dt��0�ȊO�̒l�������Ă��������B");
        // FFT�����������Ɏ���
        /*** �������� *************************************************/
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
        /*** �����܂� *************************************************/
    }

    static void runLpf(WaveformParams* pWaveformParams, int order, double cutoffFreq, const double* input, double* output) {
        if (pWaveformParams->size <= 0) throw std::runtime_error("size��0�ȊO�̒l�������Ă��������B");
        if (pWaveformParams->dt <= 0) throw std::runtime_error("dt��0�ȊO�̒l�������Ă��������B");
        /*** �������� *************************************************/
        ButterworthLPF lpf(order, cutoffFreq, 1.0 / pWaveformParams->dt);
        for (int i = 0; i < pWaveformParams->size; i++) {
            output[i] = lpf.process(input[i]);
        }
        /*** �����܂� *************************************************/
    }
};
