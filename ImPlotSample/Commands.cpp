#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Commands.h"
#include "fft.h"
#include "Butterworth.h"
#include "Chebyshev.h"

void Commands::getSinWF(WaveformParams* pWaveformParams, double times[], double waveform[]) {
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

void Commands::getSquareWF(WaveformParams* pWaveformParams, double times[], double waveform[]) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    if (pWaveformParams->frequency < 0) throw std::runtime_error("frequencyに0以上の値を代入してください。");
    double phase_rad = pWaveformParams->phase_deg * PI / 180.0;
    srand(time(NULL));
    for (int i = 0; i < pWaveformParams->size; i++) {
        times[i] = i * pWaveformParams->dt;
        for (int n = 0; n < 300; n++) {
            waveform[i] += (pWaveformParams->amplitude * (4.0 / ((2 * n + 1) * PI)) * std::sin(2 * PI * (2 * n + 1) * pWaveformParams->frequency * i * pWaveformParams->dt + phase_rad));
        }
        waveform[i] += (double)rand() / RAND_MAX * 2 * pWaveformParams->noize - pWaveformParams->noize;
    }
}

void Commands::getSawWF(WaveformParams* pWaveformParams, double times[], double waveform[]) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    if (pWaveformParams->frequency < 0) throw std::runtime_error("frequencyに0以上の値を代入してください。");
    double phase_rad = pWaveformParams->phase_deg * PI / 180.0;
    srand(time(NULL));
    for (int i = 0; i < pWaveformParams->size; i++) {
        times[i] = i * pWaveformParams->dt;
        for (int n = 1; n <= 500; n++) {
            waveform[i] += (pWaveformParams->amplitude * (2.0 / (n * PI)) * std::sin(2 * PI * n * pWaveformParams->frequency * i * pWaveformParams->dt + phase_rad) * std::pow(-1, n + 1));
        }
        waveform[i] += (double)rand() / RAND_MAX * 2 * pWaveformParams->noize - pWaveformParams->noize;
    }
}

void Commands::getTriangleWF(WaveformParams* pWaveformParams, double times[], double waveform[]) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    if (pWaveformParams->frequency < 0) throw std::runtime_error("frequencyに0以上の値を代入してください。");
    double phase_rad = pWaveformParams->phase_deg * PI / 180.0;
    srand(time(NULL));
    for (int i = 0; i < pWaveformParams->size; i++) {
        times[i] = i * pWaveformParams->dt;
        for (int n = 0; n <100; n++) {
            waveform[i] += (pWaveformParams->amplitude * (8.0 / (pow((2 * n + 1), 2) * pow(PI, 2))) * std::sin(2 * PI * (2 * n + 1) * pWaveformParams->frequency * i * pWaveformParams->dt + phase_rad) * std::pow(-1, ((2 * n + 1) - 1) / 2));
        }
        waveform[i] += (double)rand() / RAND_MAX * 2 * pWaveformParams->noize - pWaveformParams->noize;
    }
}

bool Commands::saveWaveforms(const int size, const char* filename, const double xs[], const double* ys, const int channels, const char label[]) {
    if (size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    FILE* fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "# %s, ch1", label); // 最初のチャンネル
        for (int j = 1; j < channels; j++) {
            fprintf(fp, ", ch%d", j + 1);// 2番目以降のチャンネル
        }
        fprintf(fp, "\n");
        for (int i = 0; i < size; ++i) {
            fprintf(fp, "%e, %e", xs[i], ys[i + 0 * size]);// 最初のチャンネル
            for (int j = 1; j < channels; j++) {
                fprintf(fp, ", %e", ys[i + j * size]);// 2番目以降のチャンネル
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
        return true;
    }
    return false;
}

bool Commands::loadWaveform(const int size, const char* filename, double times[], double waveform[]) {
    if (size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    FILE* fp = fopen(filename, "r");
    char buf[256];
    if (fp != NULL) {
        // 1行目は無視する
        fgets(buf, sizeof(buf), fp);  // 1行目を読み飛ばす
        for (int i = 0; i < size; i++) {
            fscanf(fp, "%lf,%lf", &times[i], &waveform[i]);
        }
        fclose(fp);
        return true;
    }
    return false;
}

double Commands::runPsd(WaveformParams* pWaveformParams, const double waveform[], double* pX, double* pY) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    if (pWaveformParams->frequency < 0) throw std::runtime_error("frequencyに0以上の値を代入してください。");
    *pX = 0; *pY = 0;
    // 周期の1/2の整数倍の点数で計算
    size_t halfPeriodSamples = static_cast<size_t>(1.0 / (pWaveformParams->frequency * 2 * pWaveformParams->dt));
    size_t usableSize = halfPeriodSamples * (int)(pWaveformParams->size / halfPeriodSamples);
    // 掛け算
    for (int i = 0; i < usableSize; i++) {
        double wt = 2 * PI * pWaveformParams->frequency * pWaveformParams->dt * i;
        *pX += waveform[i] * 2 * sin(wt);
        *pY += waveform[i] * 2 * cos(wt);
    }
    // ローパスフィルタの代わりに平均を用いる
    *pX /= usableSize;
    *pY /= usableSize;
    return sqrt((*pX) * (*pX) + (*pY) * (*pY));
}

void Commands::runFft(WaveformParams* pWaveformParams, double waveform[], double freqs[], double amps[]) {
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

void Commands::runLpf(WaveformParams* pWaveformParams, int order, double cutoffFreq, const double input[], double output[]) {
    if (pWaveformParams->size <= 0) throw std::runtime_error("sizeに0以外の値を代入してください。");
    if (pWaveformParams->dt <= 0) throw std::runtime_error("dtに0以外の値を代入してください。");
    ButterworthLPF lpf(order, cutoffFreq, 1.0 / pWaveformParams->dt);
    for (int i = 0; i < pWaveformParams->size; i++) {
        output[i] = lpf.process(input[i]);
    }
}

 void Commands::makeRanges(const int size, double start, double ranges[]) {
	 /* sizeはranges配列の要素数 */
	 /* startは最初の値 */ 
	 /* ranges配列に1,2,5,10,20,50,...の数列を格納する */
	 ranges[0] = start;
     std::cout << "ranges[" << 0 << "] = " << ranges[0] << std::endl;
     for (int i = 1; i < size; ++i) {
         if (i % 3 == 0) {
             ranges[i] = ranges[i - 1] * 2;
         }
         else if (i % 3 == 1) {
             ranges[i] = ranges[i - 1] * 2;
         }
         else {
             ranges[i] = ranges[i - 1] * 2.5;
         }
		 std::cout << "ranges[" << i << "] = " << ranges[i] << std::endl;
     }
 }