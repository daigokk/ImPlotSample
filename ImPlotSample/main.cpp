#include <iostream>
#include <cmath>
#include <vector>
#include "Gui.h"
#define PI acos(-1)

// ImGuiのWindowの関数
void ShowPlotWindow1();
void ShowPlotWindow2();

int main() {
    // GUI初期化
    Gui::Initialize("ImPlot sample", 0, 0, 1024, 720);
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
        ShowPlotWindow1();
        ShowPlotWindow2();


		/*** ここまで **********************************/
        
        // フレーム描画・スワップ
        Gui::EndFrame();
    }

    // 終了処理
    Gui::Shutdown();
    return 0;
}

// プロットウィンドウの表示
void ShowPlotWindow1() {
	static double x_data[5000],y_data[5000];
	static int size = 5000;
    static double freq = 100e3;
	static double dt = 1e-8;
	// ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Window title 1");

    
    if (ImGui::Button("Plot Sine Wave")) {
        // ボタンが押されたらここが実行される
        // サイン波データ生成
        for (int i = 0; i < size; ++i) {
            x_data[i] = i * dt;
            y_data[i] = std::sin(2*PI*freq*x_data[i]);
        }
        freq += 10e3; // 周波数を少しずつ変化させる
    }

    // プロット描画
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Plot title", ImVec2(-1, -1))) {
        ImPlot::SetupAxis(ImAxis_X1, "Time (s)");
        ImPlot::SetupAxis(ImAxis_Y1, "v (V)");
        ImPlot::PlotLine("v = sin(wt)", x_data, y_data, size);
        ImPlot::EndPlot();
    }
	// ウィンドウ終了
    ImGui::End();
}

void ShowPlotWindow2() {
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(400, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Window title 2");
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    // https://github.com/ocornut/imgui
    // https://github.com/epezent/implot
	ImGui::Text("Hello world.");


    /*** ここまで *************************************************/
    // ウィンドウ終了
    ImGui::End();
}
