#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "Gui.h"

#define PI acos(-1)

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
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 240 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    if (ImGui::Button("Save")) {
        // ボタンが押されたらここが実行される
		text = "Push the button.\n";
    }
	ImGui::Text(text.c_str());
    // ウィンドウ終了
    ImGui::End();
}
