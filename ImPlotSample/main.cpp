#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "Gui.h"

#include "Commands.h"

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
        /*** ここから *************************************************/
        // 波形データ保存
        Commands::WaveformParams wfp;
		wfp.amplitude = amplitude;
		wfp.dt = DT;
		wfp.frequency = frequency;
		wfp.noize = noize;
		wfp.phase_deg = phase_deg;
		wfp.size = SIZE;
		Commands::getWaveform(&wfp, waveform);

        // 保存
        if (Commands::saveWaveform(&wfp, FILENAME, waveform)) {
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
    static std::string text = "";
    static double times[SIZE], wf_raw[SIZE], wf_lpf[SIZE];
    static double freqs[SIZE], amps_raw[SIZE], amps_lpf[SIZE];
    static double freq = 100e3, x = 0, y = 0;
    static int order = 2;
    static float lpfreq = 1e4;

    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(660 * Gui::monitorScale, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(440 * Gui::monitorScale, 750 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    ImGui::SetNextItemWidth(200.0f * Gui::monitorScale);
    ImGui::InputDouble("Freq. (Hz)", &freq, 100.0, 1000.0, "%.1f");
    ImGui::Text("X: %5.3f, Y: %5.3f", x, y);
    ImGui::SetNextItemWidth(200.0f * Gui::monitorScale);
    if (ImGui::InputInt("Order", &order, 1, 10)) {
        if (order < 1) order = 1;
        Commands::WaveformParams wfp;
        wfp.dt = DT;
        wfp.size = SIZE;
        Commands::runLpf(&wfp, order, lpfreq, wf_raw, wf_lpf);
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
    }
    if (ImGui::Button("View")) {
        // ボタンが押されたらここが実行される
        /*** ここから *************************************************/
        // 波形データ読み込み
        Commands::WaveformParams wfp;
        wfp.size = SIZE;
		wfp.dt = DT;
		wfp.frequency = freq;
        if(Commands::loadWaveform(&wfp, FILENAME, times, wf_raw)) {
            text = "Success.\n";
        }
        else {
            text = "[Error] Failed to open file for reading.\n";
		}
        // PSD
        Commands::psd(&wfp, wf_raw, &x, &y);
        // FFT計算
        Commands::runFft(&wfp, wf_raw, freqs, amps_raw);
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
        /*** ここまで *************************************************/
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    
    if (ImGui::SliderFloat("LPF", &lpfreq, 1e4, 0.5e6, "%.0fHz")) {
        Commands::WaveformParams wfp;
		wfp.dt = DT;
        wfp.size = SIZE;
        Commands::runLpf(&wfp, order, lpfreq, wf_raw, wf_lpf);
		// FFT計算
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
    }
    
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
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 220 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 530 * Gui::monitorScale), ImGuiCond_FirstUseEver);
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


