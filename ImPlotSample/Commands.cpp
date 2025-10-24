#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Commands.h"
#include "fft.h"
#include "Butterworth.h"
#include "Chebyshev.h"

void Commands::getWaveform(WaveformParams* pWaveformParams, double* times, double* waveform) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    if (pWaveformParams->frequency < 0) throw std::runtime_error("frequencyに0以上の値を代入してください。");
    double phase_rad = pWaveformParams->phase_deg * PI / 180.0;
    srand(time(NULL));
    for (int i = 0; i < pWaveformParams->size; i++) {
        /*** ここから *************************************************/
		times[i] = i * pWaveformParams->dt;
        waveform[i] = pWaveformParams->amplitude * std::sin(2 * PI * pWaveformParams->frequency * i * pWaveformParams->dt + phase_rad);
        waveform[i] += (double)rand() / RAND_MAX * 2 * pWaveformParams->noize - pWaveformParams->noize;
        /*** ここまで *************************************************/
    }
}

bool Commands::saveWaveform(WaveformParams* pWaveformParams, const char* filename, const double* times, const double* waveform, const char label[]) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    FILE* fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, label);
        /*** ここから *************************************************/
        for (int i = 0; i < pWaveformParams->size; ++i) {
            fprintf(fp, "%e, %e\n", times[i], waveform[i]);
        }
        /*** ここまで *************************************************/
        fclose(fp);
        return true;
    }
    return false;
}

bool Commands::saveWaveforms(WaveformParams* pWaveformParams, const char* filename, const double* freqs, const double ch1[1000], const double ch2[1000], const double ch3[1000], const char label[]) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    FILE* fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, label);
        /*** ここから *************************************************/
        for (int i = 0; i < pWaveformParams->size; ++i) {
            fprintf(fp, "%e, %e, %e, %e\n", freqs[i], ch1[i], ch2[i], ch3[i]);
        }
        /*** ここまで *************************************************/
        fclose(fp);
        return true;
    }
    return false;
}

bool Commands::loadWaveform(WaveformParams* pWaveformParams, const char* filename, double* times, double* waveform) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    FILE* fp = fopen(filename, "r");
    char buf[256];
    if (fp != NULL) {
        // 1行目は無視する
        fgets(buf, sizeof(buf), fp);  // 1行目を読み飛ばす
        /*** ここから *************************************************/
        for (int i = 0; i < pWaveformParams->size; i++) {
            fscanf(fp, "%lf,%lf", &times[i], &waveform[i]);
        }
        /*** ここまで *************************************************/
        fclose(fp);
        return true;
    }
    return false;
}

double Commands::runPsd(WaveformParams* pWaveformParams, const double* waveform, double* pX, double* pY) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    if (pWaveformParams->frequency < 0) throw std::runtime_error("frequencyに0以上の値を代入してください。");
    *pX = 0; *pY = 0;
    // 周期の1/2の整数倍の点数で計算
    size_t halfPeriodSamples = static_cast<size_t>(1.0 / (pWaveformParams->frequency * 2 * pWaveformParams->dt));
    size_t usableSize = halfPeriodSamples * (int)(pWaveformParams->size / halfPeriodSamples);
    // 掛け算
    /*** ここから *************************************************/
    for (int i = 0; i < usableSize; i++) {
        double wt = 2 * PI * pWaveformParams->frequency * pWaveformParams->dt * i;
        *pX += waveform[i] * 2 * sin(wt);
        *pY += waveform[i] * 2 * cos(wt);
    }
    // ローパスフィルタの代わりに平均を用いる
    *pX /= usableSize;
    *pY /= usableSize;
    /*** ここまで *************************************************/
    return sqrt((*pX) * (*pX) + (*pY) * (*pY));
}

void Commands::runFft(WaveformParams* pWaveformParams, double* waveform, double* freqs, double* amps) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if ((pWaveformParams->size & (pWaveformParams->size - 1)) != 0) throw std::runtime_error("sizeに2の累乗を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    std::vector<std::complex<double>> data(pWaveformParams->size);
    for (int i = 0; i < pWaveformParams->size; ++i) {
        data[i] = std::complex<double>(waveform[i], 0.0);
    }
    fft(data);
    for (int i = 0; i < pWaveformParams->size; ++i) {
        freqs[i] = (double)i / (pWaveformParams->dt * pWaveformParams->size); // 周波数軸に変換
        amps[i] = std::abs(data[i]) / pWaveformParams->size; // 振幅スペクトルに変換
    }
}

void Commands::runLpf(WaveformParams* pWaveformParams, int order, double cutoffFreq, const double* input, double* output) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    ButterworthLPF lpf(order, cutoffFreq, 1.0 / pWaveformParams->dt);
    for (int i = 0; i < pWaveformParams->size; i++) {
        output[i] = lpf.process(input[i]);
    }
}

 void Commands::makeRanges(const int size, double start, double* ranges) {
	 /* sizeはranges配列の要素数 */
	 /* startは最初の値 */ 
	 /* ranges配列に1,2,5,10,20,50,...の数列を格納する */
	 ranges[0] = start;
     for (int i = 1; i < size; ++i) {
         int prev = ranges[i - 1];
         if (i % 2 == 1) {
             ranges[i] = ranges[i - 1] * 2;
         }
         else {
             ranges[i] = ranges[i - 1] * 2.5;
         }
     }
 }