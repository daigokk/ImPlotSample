#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "Gui.h"

#define PI acos(-1)
#define FILENAME "data.csv"
#define SIZE 5000
#define DT 1e-8

// ImGuiのWindowの関数
void ShowWindow1(const char title[]);
void ShowWindow2(const char title[]);
void ShowWindow3(const char title[]);

int main() {
    // GUI初期化
    Gui::Initialize("ImPlot sample", 0, 24, 1024, 720);
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
	static double phase_deg = 0.0, phase_rad=0.0;
	static double waveform[5000];
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(800, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    ImGui::InputDouble("Frequency (Hz)", &frequency, 100.0, 1000.0, "%.1f");
    ImGui::InputDouble("Amplitude (V)", &amplitude, 0.1, 1.0, "%.2f");
    if (ImGui::InputDouble("Phase (Deg.)", &phase_deg, 0.1, 1.0, "%.2f")) {
		phase_rad = phase_deg * PI / 180.0f;
    }
    if (ImGui::Button("Save")) {
        // ボタンが押されたらここが実行される
        /*** ここから *************************************************/
        // 波形データ保存


        /*** ここまで *************************************************/
    }
    ImGui::Text(text.c_str());
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow2(const char title[]) {
    static std::string text = "";
    static double times[5000], waveform[5000];
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    if (ImGui::Button("View")) {
        // ボタンが押されたらここが実行される
        /*** ここから *************************************************/
        // 波形データ読み込み


        /*** ここまで *************************************************/
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());

    // プロット描画
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Plot title", ImVec2(-1, -1))) {
        ImPlot::SetupAxis(ImAxis_X1, "Time (s)");
        ImPlot::SetupAxis(ImAxis_Y1, "v (V)");
        ImPlot::PlotLine("Ch1", times, waveform, SIZE);
        ImPlot::EndPlot();
    }
    
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow3(const char title[]) {
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
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
