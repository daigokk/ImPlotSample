#include <iostream>
#include <cmath>
#include <vector>
#include "Gui.h"
#define PI acos(-1)

// �֐��錾
void ShowPlotWindow1();
void ShowPlotWindow2();

int main() {
    // GUI������
    Gui::Initialize();
    if (Gui::GetWindow() == nullptr) {
        std::cerr << "[Error] Failed to initialize GUI\n";
        return -1;
    }
    
    // ���C�����[�v
    while (!glfwWindowShouldClose(Gui::GetWindow())) {
        // �t���[���J�n
        Gui::BeginFrame();
        
        // �E�B���h�E�`��
        // �`�悵�����v�f�������ɋL�q����
        ShowPlotWindow1();
        ShowPlotWindow2();


		// �����܂�
        
        // �t���[���`��E�X���b�v
        Gui::EndFrame();
    }

    // �I������
    Gui::Shutdown();
    return 0;
}

// �v���b�g�E�B���h�E�̕\��
void ShowPlotWindow1() {
	static double x_data[5000],y_data[5000];
	static int size = 5000;
    static double freq = 100e3;
	static double dt = 1e-8;
	// �E�B���h�E�J�n
    ImGui::Begin("Window title 1");

    
    if (ImGui::Button("Plot Sine Wave")) {
        // �{�^���������ꂽ�炱�������s�����
        // �T�C���g�f�[�^����
        for (int i = 0; i < size; ++i) {
            x_data[i] = i * dt;
            y_data[i] = std::sin(2*PI*freq*x_data[i]);
        }
        freq += 10e3; // ���g�����������ω�������
    }

    // �v���b�g�`��
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Plot title", ImVec2(-1, -1))) {
        ImPlot::PlotLine("y = sin(x)", x_data, y_data, size);
        ImPlot::EndPlot();
    }
	// �E�B���h�E�I��
    ImGui::End();
}

void ShowPlotWindow2() {
    // �E�B���h�E�J�n
    ImGui::SetNextWindowPos(ImVec2(400, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Window title 2");


    
    // �E�B���h�E�I��
    ImGui::End();
}