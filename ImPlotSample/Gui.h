#pragma once

// リンカ設定（Windows API + OpenGL + GLFW）
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "./GLFW/glfw3_mt.lib")

// ImGui & ImPlot
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

// GLFW & 標準ライブラリ
#include <GLFW/glfw3.h>
#include <iostream>



class Gui {
public:
    static void Initialize();
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame();
	static GLFWwindow* GetWindow() { return window_; }
private:
    static GLFWwindow* window_;
};

