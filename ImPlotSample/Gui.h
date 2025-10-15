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

// ESCキーでウィンドウを閉じるコールバック
void HandleKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

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

// static メンバー定義
GLFWwindow* Gui::window_ = nullptr;

// 初期化処理
void Gui::Initialize() {
    if (!glfwInit()) {
        std::cerr << "[Error] Failed to initialize GLFW\n";
        return;
    }

    // OpenGL コンテキスト設定
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ウィンドウ作成
    window_ = glfwCreateWindow(1280, 720, "ImGui + ImPlot Sample", nullptr, nullptr);
    if (!window_) {
        std::cerr << "[Error] Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // VSync 有効化

    // ImGui & ImPlot 初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();

    // バックエンド初期化
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // キー入力コールバック登録
    glfwSetKeyCallback(window_, HandleKeyInput);
}

// 終了処理
void Gui::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    glfwTerminate();
}

// フレーム開始処理
void Gui::BeginFrame() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

// フレーム描画処理
void Gui::EndFrame() {
    if (!window_) return;

    ImGui::Render();

    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_);
}
