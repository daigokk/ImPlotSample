#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "Gui.h"
#include "Commands.h"
#include "CppVisa.h"

#define PI acos(-1)
#define FILENAME_RAW "01_raw.csv"
#define FILENAME_FFT "02_fft.csv"
#define FILENAME_LPF "03_lpf.csv"
#define FILENAME_BODE_GAIN "04_bode_gain.csv"
#define FILENAME_BODE_PHASE "05_bode_phase.csv"
#define SIZE 1024 // 2^10
#define DT (10.0/100e3/SIZE)
#define N_TH 3

// ImGuiのWindowの関数
void ShowWindow1(const char title[]);
void ShowWindow2(const char title[]);
void ShowWindow3(const char title[]);
void ShowWindow4(const ViSession awg);
void ShowWindow5(const ViSession scop);

int main() {
    // GUI初期化
    Gui::Initialize(
        "ImPlot sample",
        0, 30, 1100, 750
    );
    if (Gui::GetWindow() == nullptr) {
        std::cerr << "[Error] Failed to initialize GUI\n";
        return -1;
    }

	CppVisa::OpenRM(__FILE__, __LINE__);
	vi_FindRsrc(CppVisa::resourceManager);
	auto scope = CppVisa::OpenInstrument("USB0::0x0699::0x0368::C038755::INSTR", __FILE__, __LINE__);
    // メインループ
    while (!glfwWindowShouldClose(Gui::GetWindow())) {
        // フレーム開始
        Gui::BeginFrame();
        
        // ウィンドウ描画
		/*** 描画したいImGuiのWindowをここに記述する ***/
        /*** ここから **********************************/
        ShowWindow1("Generate waveform");
        ShowWindow2("View waveform");
        ShowWindow3("Bode plots");
        ShowWindow4(scope);
        ShowWindow5(scope);


        /*** ここまで **********************************/
        
        // フレーム描画・スワップ
        Gui::EndFrame();
    }
	CppVisa::CloseInstrument(scope);
	CppVisa::CloseRM();
    // GUI終了処理
    Gui::Shutdown();
    return 0;
}

// ウィンドウの定義
void ShowWindow1(const char title[]) {
    static std::string text = "";
    static double frequency = 100e3;
    static double amplitude = 1.0;
    static double phase_deg = 0.0;
    static double noize = 0.0; // 追加
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 220 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    ImGui::InputDouble("Frequency (Hz)", &frequency, 100.0, 1000.0, "%.1f");
    ImGui::InputDouble("Amplitude (V)", &amplitude, 0.1, 1.0, "%.2f");
    ImGui::InputDouble("Phase (Deg.)", &phase_deg, 0.1, 1.0, "%.2f");
    ImGui::InputDouble("Noize (V)", &noize, 0.1, 1.0, "%.2f"); // 追加
    if (ImGui::Button("Save")) {
        // ボタンが押されたらここが実行される
        // 波形データ生成
		double times[SIZE] = { 0 }, waveform[SIZE] = { 0 };
        /*** 適切なコードを入力 ***************************************/
        Commands::WaveformParams wfp;
		wfp.amplitude = amplitude;
		wfp.dt = DT;
		wfp.frequency = frequency;
		wfp.noize = noize;
		wfp.phase_deg = phase_deg;
		wfp.size = SIZE;
		Commands::getWaveform(&wfp, times, waveform);
        /*** ここまで *************************************************/
        // 保存
        /*** 適切なコードを入力 ***************************************/
        if (Commands::saveWaveforms(wfp.size, FILENAME_RAW, times, waveform, 1)) {
            text = "Success.\n";
        }
        else {
            text = "[Error] Failed to open file for writing.\n";
		}
        /*** ここまで *************************************************/
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow2(const char title[]) {
    static std::string text1 = "", text2 = "";
    static double times[SIZE] = { 0 }, wf_raw[SIZE] = { 0 }, wf_lpf[SIZE] = { 0 };
    static double freqs[SIZE] = { 0 }, amps_raw[SIZE] = { 0 }, amps_lpf[SIZE] = { 0 };
    static double freq = 100e3, x = 0, y = 0, x_lpf = 0, y_lpf = 0;
    static int order = 2;
    static float lpfreq = 100e3;
    static Commands::WaveformParams wfp;
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(660 * Gui::monitorScale, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(440 * Gui::monitorScale, 750 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    ImGui::SetNextItemWidth(200.0f * Gui::monitorScale);
    ImGui::InputDouble("Freq. (Hz)", &freq, 100.0, 1000.0, "%.1f");
    ImGui::SetNextItemWidth(200.0f * Gui::monitorScale);
    if (ImGui::InputInt("Order", &order, 1, 10)) {
        if (order < 1) order = 1;
        wfp.dt = DT;
        wfp.size = SIZE;
		wfp.frequency = freq;
        Commands::runLpf(&wfp, order, lpfreq, wf_raw, wf_lpf);
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
        wfp.size = 1.0 / wfp.frequency / wfp.dt;
        Commands::runPsd(&wfp, &wf_lpf[SIZE - wfp.size], &x_lpf, &y_lpf);
    }
    if (ImGui::Button("View")) {
        // ボタンが押されたらここが実行される
        /*** 適切なコードを入力 ***************************************/
        // 波形データ読み込み
        wfp.size = SIZE;
		wfp.dt = DT;
		wfp.frequency = freq;
        if(Commands::loadWaveform(wfp.size, FILENAME_RAW, times, wf_raw)) {
            text1 = "Success.";
        }
        else {
            text1 = "[Error] Failed to open file for reading.";
		}
        // PSD
        Commands::runPsd(&wfp, wf_raw, &x, &y);
        // Raw波形のFFT計算
        Commands::runFft(&wfp, wf_raw, freqs, amps_raw);
		// ローパスフィルタ処理とFFT計算
        Commands::runLpf(&wfp, order, lpfreq, wf_raw, wf_lpf);
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
        wfp.size = 1.0 / wfp.frequency / wfp.dt;
        Commands::runPsd(&wfp, &wf_lpf[SIZE - wfp.size], &x_lpf, &y_lpf);
        wfp.size = SIZE;
        /*** ここまで *************************************************/
        ImPlot::SetNextAxesToFit();
    }
    ImGui::SameLine();
    ImGui::Text(text1.c_str());
    if (text1 == "Success.") {
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            if (Commands::saveWaveforms(wfp.size, FILENAME_FFT, freqs, amps_raw, 1, "Freq. (Hz)")) {
                text2 = "Success.";
            }
            else {
                text2 = "[Error] Failed to open file for writing.";
            }
            if (Commands::saveWaveforms(wfp.size, FILENAME_LPF, times, wf_lpf, 1)) {
                text2 = "Success.";
            }
            else {
                text2 = "[Error] Failed to open file for writing.";
            }
        }
        ImGui::SameLine();
        ImGui::Text(text2.c_str());
    }
    if (ImGui::SliderFloat("LPF", &lpfreq, 1e4, 0.5e6, "%.0fHz")) {
        Commands::WaveformParams wfp;
		wfp.dt = DT;
        wfp.size = SIZE;
		wfp.frequency = freq;
        Commands::runLpf(&wfp, order, lpfreq, wf_raw, wf_lpf);
		// FFT計算
        Commands::runFft(&wfp, wf_lpf, freqs, amps_lpf);
        wfp.size = 1.0 / wfp.frequency / wfp.dt;
        Commands::runPsd(&wfp, &wf_lpf[SIZE - wfp.size], &x_lpf, &y_lpf);
    }
    ImGui::Text("RAW X: %5.3f, Y: %5.3f", x, y);
    ImGui::Text("LPF X: %5.3f, Y: %5.3f", x_lpf, y_lpf);
    // プロット描画
    if (ImPlot::BeginPlot("Waveform", ImVec2(-1, 250 * Gui::monitorScale))) {
        ImPlot::SetupAxis(ImAxis_X1, "Time (s)");
        ImPlot::SetupAxis(ImAxis_Y1, "v (V)");
        ImPlot::PlotLine("Ch1", times, wf_raw, SIZE);
        ImPlot::PlotLine("LPF", times, wf_lpf, SIZE);
        ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("FFT", ImVec2(-1, 250 * Gui::monitorScale))) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 1.0 / DT, ImPlotCond_Once);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 1.0, ImPlotCond_Once);
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
        ImPlot::SetupAxis(ImAxis_Y1, "v (V)");
        ImPlot::PlotLine("Ch1", freqs, amps_raw, SIZE);
        ImPlot::PlotLine("LPF", freqs, amps_lpf, SIZE);
        ImPlot::EndPlot();
    }
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow3(const char title[]) {
    static double freqs[1000] = { 0 }, gains[N_TH][1000] = { 0 }, phases[N_TH][1000];
    static Commands::WaveformParams wfp;
    static std::string text = "";
    // ウィンドウ開始
    ImGui::SetNextWindowPos(ImVec2(0, 220 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 530 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    if (ImGui::Button("Run")) {
        // ボタンが押されたらここが実行される
        wfp.amplitude = 1;
        wfp.dt = DT;
        wfp.size = SIZE;
        wfp.frequency = 10e3;
        // 周波数特性
        /*** 適切なコードを入力 ***************************************/
        for (int j = 0; j < N_TH; j++) {
            wfp.frequency = 10e3;
            for (int i = 0; i < 1000; i++) {
                double times[SIZE] = { 0 }, waveform[SIZE] = { 0 };
                double x = 0, y = 0, wf_lpf[SIZE];
                Commands::getWaveform(&wfp, times, waveform);
                Commands::runLpf(&wfp, j+1, 100e3, waveform, wf_lpf);
                freqs[i] = wfp.frequency;
                gains[j][i] = 20.0 * log10(Commands::runPsd(&wfp, wf_lpf, &x, &y) / wfp.amplitude);
                phases[j][i] = atan2(y, x) / PI * 180;
				if (phases[j][i] > 0) phases[j][i] -= 360;
                wfp.frequency += 1e3;
            }
        }
        /*** ここまで *************************************************/
        text = "[Error] Failed to open file for writing.";
        wfp.size = 1000;
        if (Commands::saveWaveforms(wfp.size, FILENAME_BODE_GAIN, freqs, (double*)gains, N_TH)) {
            if (Commands::saveWaveforms(wfp.size, FILENAME_BODE_PHASE, freqs, (double*)phases, N_TH)) {
                text = "Success.";
            }
        }
    }
    ImGui::SameLine();
    ImGui::Text(text.c_str());
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Gain", ImVec2(-1, 225 * Gui::monitorScale))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
        ImPlot::SetupAxis(ImAxis_Y1, "Gain (dB)");
        for (int j = 0; j < N_TH; j++) {
            std::string label = "Order " + std::to_string(j + 1);
            ImPlot::PlotLine(label.c_str(), freqs, gains[j], 1000);
        }
        ImPlot::EndPlot();
    }
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Phase", ImVec2(-1, 225 * Gui::monitorScale))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
        ImPlot::SetupAxis(ImAxis_Y1, "Phase (Deg.)");
        for (int j = 0; j < N_TH; j++) {
            std::string label = "Order " + std::to_string(j + 1);
            ImPlot::PlotLine(label.c_str(), freqs, phases[j], 1000);
        }
        ImPlot::EndPlot();
    }
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow4(const ViSession awg) {
    // ウィンドウ開始
    ImGui::SetNextWindowSize(ImVec2(500 * Gui::monitorScale, 450 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin("AWG");
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    // https://github.com/ocornut/imgui
    // https://github.com/epezent/implot
    // https://github.com/daigokk/ImPlotSample
    /*** ここから *************************************************/
    static float freq = 0, ampl = 0;
    ImGui::SetNextItemWidth(200.0f * Gui::monitorScale);
    if (ImGui::InputFloat("Freq.", &freq, 1, 1)) {
        static std::string ret;
		CppVisa::cviPrintf(awg, __FILE__, __LINE__, "HORizontal:SECdiv %e\n", freq);
        ret = CppVisa::cviQueryf(awg, __FILE__, __LINE__, "HORizontal:SECdiv?\n");
		freq = atof(ret.c_str());
    }
	ImGui::SameLine();
	ImGui::Text(": %f Hz", freq);
    ImGui::SetNextItemWidth(200.0f * Gui::monitorScale);
    if (ImGui::InputFloat("Ampl.", &ampl, 1, 1)) {
        static std::string ret;
        CppVisa::cviPrintf(awg, __FILE__, __LINE__, "HORizontal:SECdiv %e\n", ampl);
        ret = CppVisa::cviQueryf(awg, __FILE__, __LINE__, "HORizontal:SECdiv?\n");
        freq = atof(ret.c_str());
    }
    ImGui::SameLine();
    ImGui::Text(": %f V", ampl);
    /*** ここまで *************************************************/
    // ウィンドウ終了
    ImGui::End();
}

void ShowWindow5(const ViSession scope) {
    // ウィンドウ開始
    ImGui::SetNextWindowSize(ImVec2(500 * Gui::monitorScale, 450 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin("Scope");
    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
    // https://github.com/ocornut/imgui
    // https://github.com/epezent/implot
    // https://github.com/daigokk/ImPlotSample
    /*** ここから *************************************************/
    static int t_index = 0, v_index[2] = { 0 };
    static double timediv = 0, voltsdiv[2] = { 0 };
    ImGui::SetNextItemWidth(20.0f * Gui::monitorScale);
    if (ImGui::InputInt("Time/div", &t_index, 1, 1)) {
        if (t_index < 0) t_index = 0;
        if (t_index > 29) t_index = 29;
        static std::string ret;
        static double t_ranges[30] = {};
        Commands::makeRanges(30, 5e-9, t_ranges);
        CppVisa::cviPrintf(scope, __FILE__, __LINE__, "HORizontal:SECdiv %e\n", t_ranges[t_index]);
        ret = CppVisa::cviQueryf(scope, __FILE__, __LINE__, "HORizontal:SECdiv?\n");
        timediv = atof(ret.c_str());
    }
    ImGui::SameLine();
    ImGui::Text(": %.0e s", timediv);
    for(int ch =0; ch < 2; ch++) {
        ImGui::SetNextItemWidth(20.0f * Gui::monitorScale);
        std::string label = "Ch" + std::to_string(ch + 1) + " Volts/div";
        if (ImGui::InputInt(label.c_str(), &v_index[ch], 1, 1)) {
            if (v_index[ch] < 0) v_index[ch] = 0;
            if (v_index[ch] > 29) v_index[ch] = 29;
            static std::string ret;
            static double v_ranges[30] = {};
            Commands::makeRanges(30, 20e-3, v_ranges);
            CppVisa::cviPrintf(scope, __FILE__, __LINE__, "CH%d:VOLts %e\n", ch + 1, v_ranges[v_index[ch]]);
            ret = CppVisa::cviQueryf(scope, __FILE__, __LINE__, "CH%d:VOLts?\n", ch + 1);
            voltsdiv[ch] = atof(ret.c_str());
        }
        ImGui::SameLine();
        ImGui::Text(": %.0e V", voltsdiv[ch]);
	}
    if(ImGui::Button("Auto")) {
        CppVisa::cviPrintf(scope, __FILE__, __LINE__, "AUTOSet EXECute\n");
	}
    ImGui::SameLine();
    if (ImGui::Button("Run/Stop")) {
		char* ret = CppVisa::cviQueryf(scope, __FILE__, __LINE__, "ACQuire:STATE?\n");
        if(ret != nullptr && std::string(ret) == "1\n") {
            CppVisa::cviPrintf(scope, __FILE__, __LINE__, "ACQuire:STATE STOP\n");
        }
        else {
            CppVisa::cviPrintf(scope, __FILE__, __LINE__, "ACQuire:STATE RUN\n");
        }
    }
    ImGui::SameLine();
	static std::vector<double> times, voltages[2];
    if(ImGui::Button("Capture")) {
        double offset, mult;
        int length, count;
        char c;

        length = atoi(CppVisa::cviQueryf(scope, __FILE__, __LINE__, "HORizontal:RECOrdlength?\n"));

        CppVisa::cviPrintf(scope, __FILE__, __LINE__, "DATa:start 1;DATa:stop %d\n", length);
        CppVisa::cviPrintf(scope, __FILE__, __LINE__, "DATa:ENCdg RPBinary\n");
        CppVisa::cviPrintf(scope, __FILE__, __LINE__, "DATa:WIDth 1");
        offset = atof(CppVisa::cviQueryf(scope, __FILE__, __LINE__, "WFMPre:YOFf?\n"));
        mult = atof(CppVisa::cviQueryf(scope, __FILE__, __LINE__, "WFMPre:YMUlt?\n"));
        
        times.resize(length);
        for(int ch = 0; ch <2; ch++) {
            voltages[ch].resize(length);
            CppVisa::cviPrintf(scope, __FILE__, __LINE__, "DATa:SOUrce CH%d\n", ch);
            CppVisa::cviPrintf(scope, __FILE__, __LINE__, "CURVe?\n");
            viScanf(scope, "%c", &c);
            if (c != '#') exit(EXIT_FAILURE);
            viScanf(scope, "%c", &c);
            if (c < '0' && '9' < c) exit(EXIT_FAILURE);
            count = c - '0';
            for (int i = 0; i < count; i++)
            {
                viScanf(scope, "%c", &c);
                if (c < '0' && '9' < c) exit(EXIT_FAILURE);
            }
            for (int i = 0; i < length; i++)
            {
                viScanf(scope, "%c", &c);
                voltages[ch][i] = ((unsigned char)c - offset) * mult;
            }
            viScanf(scope, "%c", &c); // For deleting a delimiter in the read buffer.
		}
	}
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Waveform", ImVec2(-1, -1))) {
        ImPlot::SetupAxis(ImAxis_X1, "Time (s)");
        ImPlot::SetupAxis(ImAxis_Y1, "Volts (V)");
        ImPlot::PlotLine("Ch1", times.data(), voltages[0].data(), times.size());
        ImPlot::PlotLine("Ch2", times.data(), voltages[1].data(), times.size());
		ImPlot::EndPlot();
    }
    /*** ここまで *************************************************/
    // ウィンドウ終了
    ImGui::End();
}
