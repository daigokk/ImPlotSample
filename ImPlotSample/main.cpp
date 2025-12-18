#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#undef SIZE
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <complex>

#include <visa.h>
#include "sub_visa.h"
#include "Gui.h"
#include "Butterworth.h"

#define PI acos(-1)
#define SIZE 32768
#define DT (0.1/SIZE)

// ImGuiのWindowの関数
void ShowWindow1(const char title[]);

int main() {
    // GUI初期化
    Gui::Initialize(
        "ImPlot sample",
        0, 30, 1100, 780
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
        ShowWindow1("Generate waveform");

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
    static double frequency = 10e3;
    static double amplitude = 1.0;
    static double phase_deg = 0.0;
    static double noise = 0.0; // 追加
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 220 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    if (ImGui::Button("Save")) {
        // ボタンが押されたらここが実行される
        text = "Success.";
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    // ウィンドウ終了
    ImGui::End();
}
