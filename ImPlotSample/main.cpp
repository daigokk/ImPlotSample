#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <complex> // for FFT
#include "Gui.h"
#include "psd.h"
#include "ButterworthLPF.h"

#define PI acos(-1)
#define FILENAME "data.csv"
#define SIZE 1024 // 2^10
#define DT (10.0/100e3/SIZE)

// ImGuiのWindowの関数
void ShowWindow1(const char title[]);
void ShowWindow2(const char title[]);
void ShowWindow3(const char title[]);

void fft(std::vector<std::complex<double>>& a);

int main() {
    // GUI初期化
    Gui::Initialize("ImPlot sample");
    if (Gui::GetWindow() == nullptr) {
        std::cerr << "[Error] Failed to initialize GUI\n";
        return -1;
    }

    // メインループ
    while (!glfwWindowShouldClose(Gui::GetWindow())) {
        // フレーム開始
        Gui::BeginFrame();
        
        // ウィンドウ描画
		/*** 描画したいImGuiのWindowをここに記述する ***/
        /*** ここから **********************************/
        ShowWindow1("Generate waveform");
        ShowWindow2("View waveform");
        ShowWindow3("Window title 3");


        /*** ここまで **********************************/
        
        // フレーム描画・スワップ
        Gui::EndFrame();
    }

    // GUI終了処理
    Gui::Shutdown();
    return 0;
}

// ウィンドウの定義
void ShowWindow1(const char title[]) {
    static std::string text = "";
    static double frequency = 100e3;
    static double amplitude = 1.0;
    static double phase_deg = 0.0, phase_rad = 0.0;
    static double waveform[SIZE];
    static double noize = 0.0; // 追加
	static double fft[SIZE];
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660, 220), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    ImGui::InputDouble("Frequency (Hz)", &frequency, 100.0, 1000.0, "%.1f");
    ImGui::InputDouble("Amplitude (V)", &amplitude, 0.1, 1.0, "%.2f");
    if (ImGui::InputDouble("Phase (Deg.)", &phase_deg, 0.1, 1.0, "%.2f")) {
        phase_rad = phase_deg * PI / 180.0f;
    }
    ImGui::InputDouble("Noize (V)", &noize, 0.1, 1.0, "%.2f"); // 追加
    if (ImGui::Button("Save")) {
        // ボタンが押されたらここが実行される
        /*** ここから *************************************************/
        // 波形データ保存
        // 生成
        srand(time(NULL));
        for (int i = 0; i < SIZE; i++) {
            waveform[i] = amplitude * std::sin(2 * PI * frequency * i * DT + phase_rad);
            waveform[i] += (double)rand() / RAND_MAX * 2 * noize - noize; // 追加
        }

        // 保存
        FILE* fp = fopen(FILENAME, "w");
        if (fp != NULL) {
            fprintf(fp, "# Time (s), Voltage (V)\n");
            for (int i = 0; i < SIZE; ++i) {
                fprintf(fp, "%e, %e\n", i * DT, waveform[i]);
            }
            fclose(fp);
            text = "Success.\n";
        }
        else {
            text = "[Error] Failed to open file for writing\n";
        }
		/*** ここまで *************************************************/
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow2(const char title[]) {
    static std::string text = "";
    static double times[SIZE], waveform[SIZE];
    static double freqs[SIZE], amps[SIZE];
    static double freq = 100e3, x = 0, y = 0;
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(660, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(425, 700), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputDouble("Freq. (Hz)", &freq, 100.0, 1000.0, "%.1f");
    if (ImGui::Button("View")) {
        // ボタンが押されたらここが実行される
        /*** ここから *************************************************/
        // 波形データ読み込み
        FILE* fp = fopen(FILENAME, "r");
        char buf[256];
        if (fp != NULL) {
            // 1行目は無視する
            fgets(buf, sizeof(buf), fp);  // 1行目を読み飛ばす
            for (int i = 0; i < SIZE; i++) {
                fscanf(fp, "%lf,%lf", &times[i], &waveform[i]);
            }
            fclose(fp);
            text = "Success.\n";
        }
        else {
            text = "[Error] Failed to open file for reading.\n";
        }
        // FFT計算
        std::vector<std::complex<double>> data(SIZE);
        for (int i = 0; i < SIZE; ++i) {
            data[i] = std::complex<double>(waveform[i], 0.0);
        }
        fft(data);
        for (int i = 0; i < SIZE; ++i) {
            freqs[i] = (double)i / (DT * SIZE); // 周波数軸に変換
            amps[i] = std::abs(data[i]) / SIZE; // 振幅スペクトルに変換
		}
        ImPlot::SetNextAxesToFit();
        // PSD
    //    double amp_ = 0;
    //    for (int i = 0; i < SIZE / 2; i++) {
    //        if (amp_ < amps[i]) {
    //            amp_ = amps[i];
				//maxFreq = freqs[i];
    //        }
    //    }
        psd(waveform, freq, times[1] - times[0], SIZE, &x, &y);
        /*** ここまで *************************************************/
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    ImGui::Text("X: %5.3f, Y: %5.3f", x, y);

    // プロット描画
    if (ImPlot::BeginPlot("Raw", ImVec2(-1, 300))) {
        ImPlot::SetupAxis(ImAxis_X1, "Time (s)");
        ImPlot::SetupAxis(ImAxis_Y1, "v (V)");
        ImPlot::PlotLine("Ch1", times, waveform, SIZE);
        ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("FFT", ImVec2(-1, 300))) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 1.0 / DT, ImPlotCond_Once);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 1.0, ImPlotCond_Once);
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
        ImPlot::SetupAxis(ImAxis_Y1, "v (V)");
        ImPlot::PlotLine("Ch1", freqs, amps, SIZE);
        ImPlot::EndPlot();
    }
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow3(const char title[]) {
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 250), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(640, 460), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    // https://github.com/ocornut/imgui
    // https://github.com/epezent/implot
    // https://github.com/daigokk/ImPlotSample
    /*** ここから *************************************************/
	

    /*** ここまで *************************************************/
    // ウィンドウ終了
    ImGui::End();
}


void fft(std::vector<std::complex<double>>& a) {
    int N = a.size();
    if (N <= 1) return;

    // 偶数・奇数に分割 配列のサイズが半分になる！！！
    std::vector<std::complex<double>> even(N / 2), odd(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        even[i] = a[i * 2];
        odd[i] = a[i * 2 + 1];
    }

    // 再帰呼び出し
    fft(even);
    fft(odd);

    // 合成
    for (int k = 0; k < N / 2; ++k) {
        std::complex<double> t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        a[k] = even[k] + t;
        a[k + N / 2] = even[k] - t;
    }
}