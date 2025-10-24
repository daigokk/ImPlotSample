#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "Gui.h"

#include "Commands.h"

#define PI acos(-1)
#define FILENAME_RAW "01_raw.csv"
#define FILENAME_FFT "02_fft.csv"
#define FILENAME_LPF "03_lpf.csv"
#define FILENAME_BODE_GAIN "04_bode_gain.csv"
#define FILENAME_BODE_PHASE "05_bode_phase.csv"
#define SIZE 1024 // 2^10
#define DT (10.0/100e3/SIZE)

// ImGuiのWindowの関数
void ShowWindow1(const char title[]);
void ShowWindow2(const char title[]);
void ShowWindow3(const char title[]);
void ShowWindow4(const char title[]);

int main() {
    // GUI初期化
    Gui::Initialize(
        "ImPlot sample",
        0, 30, 1100, 750
    );
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
        ShowWindow3("Bode plots");
        //ShowWindow4("Window title 4");


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
    static double times[SIZE] = { 0 }, waveform[SIZE] = { 0 };
    static double noize = 0.0; // 追加
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 220 * Gui::monitorScale), ImGuiCond_FirstUseEver);
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
        // 波形データ生成
        /*** 適切なコードを入力 ***************************************/
        Commands::WaveformParams wfp;
		wfp.amplitude = amplitude;
		wfp.dt = DT;
		wfp.frequency = frequency;
		wfp.noize = noize;
		wfp.phase_deg = phase_deg;
		wfp.size = SIZE;
		Commands::getWaveform(&wfp, times, waveform);
        /*** ここまで *************************************************/
        // 保存
        /*** 適切なコードを入力 ***************************************/
        if (Commands::saveWaveform(&wfp, FILENAME_RAW, times, waveform)) {
            text = "Success.\n";
        }
        else {
            text = "[Error] Failed to open file for writing.\n";
		}
        /*** ここまで *************************************************/
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow2(const char title[]) {
    static std::string text1 = "", text2 = "";
    static double times[SIZE] = { 0 }, wf_raw[SIZE] = { 0 }, wf_lpf[SIZE] = { 0 };
    static double freqs[SIZE] = { 0 }, amps_raw[SIZE] = { 0 }, amps_lpf[SIZE] = { 0 };
    static double freq = 100e3, x = 0, y = 0, x_lpf = 0, y_lpf = 0;
    static int order = 2;
    static float lpfreq = 1e4;
    static Commands::WaveformParams wfp;
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(660 * Gui::monitorScale, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(440 * Gui::monitorScale, 750 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    ImGui::SetNextItemWidth(200.0f * Gui::monitorScale);
    ImGui::InputDouble("Freq. (Hz)", &freq, 100.0, 1000.0, "%.1f");
    ImGui::SetNextItemWidth(200.0f * Gui::monitorScale);
    if (ImGui::InputInt("Order", &order, 1, 10)) {
        if (order < 1) order = 1;
        wfp.dt = DT;
        wfp.size = SIZE;
		wfp.frequency = freq;
        Commands::runLpf(&wfp, order, lpfreq, wf_raw, wf_lpf);
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
        wfp.size = 1.0 / wfp.frequency / wfp.dt;
        Commands::runPsd(&wfp, &wf_lpf[SIZE - wfp.size], &x_lpf, &y_lpf);
    }
    if (ImGui::Button("View")) {
        // ボタンが押されたらここが実行される
        /*** 適切なコードを入力 ***************************************/
        // 波形データ読み込み
        wfp.size = SIZE;
		wfp.dt = DT;
		wfp.frequency = freq;
        if(Commands::loadWaveform(&wfp, FILENAME_RAW, times, wf_raw)) {
            text1 = "Success.";
        }
        else {
            text1 = "[Error] Failed to open file for reading.";
		}
        // PSD
        Commands::runPsd(&wfp, wf_raw, &x, &y);
        // Raw波形のFFT計算
        Commands::runFft(&wfp, wf_raw, freqs, amps_raw);
		// ローパスフィルタ処理とFFT計算
        Commands::runLpf(&wfp, order, lpfreq, wf_raw, wf_lpf);
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
        wfp.size = 1.0 / wfp.frequency / wfp.dt;
        Commands::runPsd(&wfp, &wf_lpf[SIZE - wfp.size], &x_lpf, &y_lpf);
        wfp.size = SIZE;
        /*** ここまで *************************************************/
        ImPlot::SetNextAxesToFit();
    }
    ImGui::SameLine();
    ImGui::Text(text1.c_str());
    if (text1 == "Success.") {
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            if (Commands::saveWaveform(&wfp, FILENAME_FFT, freqs, amps_raw, "# Freq. (Hz), Voltage (V)\n")) {
                text2 = "Success.";
            }
            else {
                text2 = "[Error] Failed to open file for writing.";
            }
            if (Commands::saveWaveform(&wfp, FILENAME_LPF, times, wf_lpf)) {
                text2 = "Success.";
            }
            else {
                text2 = "[Error] Failed to open file for writing.";
            }
        }
        ImGui::SameLine();
        ImGui::Text(text2.c_str());
    }
    if (ImGui::SliderFloat("LPF", &lpfreq, 1e4, 0.5e6, "%.0fHz")) {
        Commands::WaveformParams wfp;
		wfp.dt = DT;
        wfp.size = SIZE;
		wfp.frequency = freq;
        Commands::runLpf(&wfp, order, lpfreq, wf_raw, wf_lpf);
		// FFT計算
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
        wfp.size = 1.0 / wfp.frequency / wfp.dt;
        Commands::runPsd(&wfp, &wf_lpf[SIZE - wfp.size], &x_lpf, &y_lpf);
    }
    ImGui::Text("RAW X: %5.3f, Y: %5.3f", x, y);
    ImGui::Text("LPF X: %5.3f, Y: %5.3f", x_lpf, y_lpf);
    // プロット描画
    if (ImPlot::BeginPlot("Waveform", ImVec2(-1, 250 * Gui::monitorScale))) {
        ImPlot::SetupAxis(ImAxis_X1, "Time (s)");
        ImPlot::SetupAxis(ImAxis_Y1, "v (V)");
        ImPlot::PlotLine("Ch1", times, wf_raw, SIZE);
        ImPlot::PlotLine("LPF", times, wf_lpf, SIZE);
        ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("FFT", ImVec2(-1, 250 * Gui::monitorScale))) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 1.0 / DT, ImPlotCond_Once);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 1.0, ImPlotCond_Once);
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
        ImPlot::SetupAxis(ImAxis_Y1, "v (V)");
        ImPlot::PlotLine("Ch1", freqs, amps_raw, SIZE);
        ImPlot::PlotLine("LPF", freqs, amps_lpf, SIZE);
        ImPlot::EndPlot();
    }
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow3(const char title[]) {
    static double freqs[1000] = { 0 }, gains[3][1000] = { 0 }, phases[3][1000];
    static Commands::WaveformParams wfp;
    static std::string text = "";
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 220 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 530 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    if (ImGui::Button("Run")) {
        // ボタンが押されたらここが実行される
        wfp.amplitude = 1;
        wfp.dt = DT;
        wfp.size = SIZE;
        wfp.frequency = 10e3;
        // 周波数特性
        for (int j = 0; j < 3; j++) {
            wfp.frequency = 10e3;
            for (int i = 0; i < 1000; i++) {
                double times[SIZE] = { 0 }, waveform[SIZE] = { 0 };
                double x = 0, y = 0, wf_lpf[SIZE];
                Commands::getWaveform(&wfp, times, waveform);
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
        if (Commands::saveWaveforms(&wfp, FILENAME_BODE_GAIN, freqs, gains[0], gains[1], gains[2])) {
            if (Commands::saveWaveforms(&wfp, FILENAME_BODE_PHASE, freqs, phases[0], phases[1], phases[2])) {
                text = "Success.";
            }
        }
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

void ShowWindow4(const char title[]) {
    // ウィンドウ開始
    ImGui::SetNextWindowSize(ImVec2(600 * Gui::monitorScale, 450 * Gui::monitorScale), ImGuiCond_FirstUseEver);
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
