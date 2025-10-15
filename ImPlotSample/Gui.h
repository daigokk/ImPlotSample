#pragma once

// �����J�ݒ�iWindows API + OpenGL + GLFW�j
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "./GLFW/glfw3_mt.lib")

// ImGui & ImPlot
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

// GLFW & �W�����C�u����
#include <GLFW/glfw3.h>
#include <iostream>

// ESC�L�[�ŃE�B���h�E�����R�[���o�b�N
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

// static �����o�[��`
GLFWwindow* Gui::window_ = nullptr;

// ����������
void Gui::Initialize() {
    if (!glfwInit()) {
        std::cerr << "[Error] Failed to initialize GLFW\n";
        return;
    }

    // OpenGL �R���e�L�X�g�ݒ�
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // �E�B���h�E�쐬
    window_ = glfwCreateWindow(1280, 720, "ImGui + ImPlot Sample", nullptr, nullptr);
    if (!window_) {
        std::cerr << "[Error] Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // VSync �L����

    // ImGui & ImPlot ������
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();

    // �o�b�N�G���h������
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // �L�[���̓R�[���o�b�N�o�^
    glfwSetKeyCallback(window_, HandleKeyInput);
}

// �I������
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

// �t���[���J�n����
void Gui::BeginFrame() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

// �t���[���`�揈��
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
