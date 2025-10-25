#pragma once

// リンカ設定（Windows API + OpenGL + GLFW）
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "./GLFW/glfw3_mt.lib")

// ImGui & ImPlot
#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>
#include <IMGUI/implot.h>

// GLFW & 標準ライブラリ
#include <GLFW/glfw3.h>
#include <iostream>

bool MyKnob(const char* label, float* p_value, const float v_min, const float v_max);
bool MyKnobInt(const char* label, int* p_value, const int v_min, const int v_max); 

class Gui {
public:
    static void Initialize(
        const char title[],
        const int windowPosX = 0, const int windowPosY = 30,
        const int windowWidth = 1100, const int windowHeight = 750
    );
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame();
    static GLFWwindow* GetWindow() { return window_; }
    static void setStyle(const int theme);
    static void ImGui_SetGrayTheme();
    static void ImGui_SetNeonBlueTheme();
    static void ImGui_SetNeonGreenTheme();
    static void ImGui_SetNeonRedTheme();
    static void ImGui_SetEvaTheme();
    static float monitorScale;
private:
    static GLFWwindow* window_;
    static int monitorWidth;
    static int monitorHeight;
    static int windowFlag;
    // --- Grouped Configuration Structs ---
    struct WindowCfg {
        struct XYint {
            int x, y;
        };
        XYint size = { 1024, 720 };
        XYint pos = { 0, 30 };
        float monitorScale = 1.0f; // <<< ここに再度追加します
    };
    static WindowCfg window;
};
