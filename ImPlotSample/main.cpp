#include <iostream>
#include <cmath>
#include <vector>
#include "Gui.h"
#define PI acos(-1)

// �֐��錾
void ShowPlotWindow();

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
        ShowPlotWindow();




		// �����܂�
        
        // �t���[���`��E�X���b�v
        Gui::EndFrame();
    }

    // �I������
    Gui::Shutdown();
    return 0;
}

// �v���b�g�E�B���h�E�̕\��
void ShowPlotWindow() {
	static double x_data[5000],y_data[5000];
	static int size = 5000;
    static double freq = 100e3;
	static double dt = 1e-8;

    ImGui::Begin("Hello, ImGui + ImPlot!");

    
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
    if (ImPlot::BeginPlot("Sine Wave", ImVec2(-1, -1))) {
        ImPlot::PlotLine("y = sin(x)", x_data, y_data, size);
        ImPlot::EndPlot();
    }

    ImGui::End();
}
