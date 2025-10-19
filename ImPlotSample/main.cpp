#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "Gui.h"

#define PI acos(-1)

//#define VISA
#ifdef VISA
    #include "CppVisa.h"
#endif // VISA


// ImGuiのWindowの関数
void ShowWindow1(const char title[], int scope);
void ShowWindow2(const char title[]);
void ShowWindow3(const char title[]);

int main() {
    // GUI初期化
    Gui::Initialize("ImPlot sample", 0, 24, 1024, 720);
    if (Gui::GetWindow() == nullptr) {
        std::cerr << "[Error] Failed to initialize GUI\n";
        return -1;
    }
#ifdef VISA
	// VISA初期化
    /*** ここから **********************************/
    ViSession resourceManager = 0, scope = 0;
    vi_checkError(viOpenDefaultRM(&resourceManager), __FILE__, __LINE__);
    vi_FindRsrc(resourceManager);
    vi_checkError(viOpen(resourceManager, "USB0::????????::INSTR", VI_NULL, VI_NULL, &scope), __FILE__, __LINE__);


    /*** ここまで **********************************/
#else
	int scope = 0;
#endif // VISA
    // メインループ
    while (!glfwWindowShouldClose(Gui::GetWindow())) {
        // フレーム開始
        Gui::BeginFrame();
        
        // ウィンドウ描画
		/*** 描画したいImGuiのWindowをここに記述する ***/
        /*** ここから **********************************/
        ShowWindow1("Window title 1", scope);
        ShowWindow2("Window title 2");
        ShowWindow3("Window title 3");


        /*** ここまで **********************************/
        
        // フレーム描画・スワップ
        Gui::EndFrame();
    }
#ifdef VISA
	// VISA終了処理
    /*** ここから **********************************/


	vi_checkError(viClose(scope), __FILE__, __LINE__);
    vi_checkError(viClose(resourceManager), __FILE__, __LINE__);
    /*** ここまで **********************************/
#endif // VISA
    // GUI終了処理
    Gui::Shutdown();
    return 0;
}

// ウィンドウの定義
void ShowWindow1(const char title[], int scope) {
    static std::string text;
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(800, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    /*** ここから *************************************************/
    if (ImGui::Button("View text")) {
        // ボタンが押されたらここが実行される
#ifdef VISA
        static char ret[256];
        vi_checkError(viPrintf(scope, "*IDN?\n"), __FILE__, __LINE__);
        vi_checkError(viScanf(scope, "%255t", ret), __FILE__, __LINE__);
        //vi_checkError(viQueryf(scope, "%s", "%255t", "*IDN?\n", ret), __FILE__, __LINE__); //viQueryfはviPrintfとviScanfを連続して呼び出す関数
        text += ret + '\n';
#else
		text += "Hello!\n";
#endif // VISA
    }
    ImGui::Text(text.c_str());
    /*** ここまで *************************************************/
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow2(const char title[]) {
    static double x_data[5000], y_data[5000];
    static int size = 5000;
    static double freq = 100e3;
    static double dt = 1e-8;
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    /*** ここから *************************************************/
    if (ImGui::Button("Plot Sine Wave")) {
        // ボタンが押されたらここが実行される
        // サイン波データ生成
        for (int i = 0; i < size; ++i) {
            x_data[i] = i * dt;
            y_data[i] = std::sin(2 * PI * freq * x_data[i]);
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
    /*** ここまで *************************************************/
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
