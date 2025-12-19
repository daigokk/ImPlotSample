```cpp
// 「#define」はmain関数より前に置く。
#define FREQS 1,2,5,10,20,50,100,200,500,1000,2000,5000
#define N_F 12
#define N_LENGTH 12500

void ShowWindow6(const ViSession awg, const ViSession scope) {
    static double freqs[] = { FREQS };
    static double gains[N_F] = { 0 }, phases[N_F] = { 0 };
    // ウィンドウ開始
    ImGui::SetNextWindowSize(ImVec2(500 * Gui::monitorScale, 450 * Gui::monitorScale), ImGuiCond_FirstUseEver);
    ImGui::Begin("Bode");
    if (ImGui::Button("Measure")) {
        // トリガーモードをNORMにする。50Hz以下でも波形を止めるため。
        viPrintf(scope, "XXXXXXXXXXXXXXXX\n");
        // トリガーレベルを0Vにする。
        viPrintf(scope, "XXXXXXXXXXXXXXXX\n");
        for (int i = 0; i < N_F; i++) {
            // ファンクションジェネレータの周波数を設定する。
            viPrintf(awg, "XXXXXXXXXXXXXXXX %e\n", freqs[i]);
            // オシロスコープのTime/divを設定する。
            viPrintf(scope, "XXXXXXXXXXXXXXXX %.3f\n", 1.0 / freqs[i]);
            printf("%f\n", freqs[i]);
            // 波形がオシロスコープの画面に表示されるまで待つ(Time/Divの10倍以上)。
            Sleep(1000 / freqs[i] * 10 * 2);
            // オシロスコープから波形データを取り込み、ファイルに保存する。
            os_save_Waveformsf(scope, "waveforms_%06.0f.csv", freqs[i]);
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Analysis")) {
        double freqs[] = { FREQS };
        char filepath[256];
        std::vector<double> times, voltages[2];
        times.resize(N_LENGTH);
        voltages[0].resize(N_LENGTH);
        voltages[1].resize(N_LENGTH);
        for (int i = 0; i < N_F; i++) {
            double times[N_LENGTH], ch1[N_LENGTH], ch2[N_LENGTH];
            FILE* fp;
            sprintf(filepath, "waveforms_%06.0f.csv");
            fp = fopen(filepath, "r");
            for (int i = 0; i < N_LENGTH; i++) {
                fscanf(fp, "%lf, %lf, %lf", &times[i], &ch1[i], &ch2[i]);
            }
            fclose(fp);
            /* PSDで振幅と位相差を計算する */
            /*** ここから *************************************************/



            /*** ここまで *************************************************/
        }
    }

    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("##Bode", ImVec2(-1, -1))) {
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (Hz)");
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxis(ImAxis_Y1, "Gain (dB)");
        ImPlot::SetupAxis(ImAxis_Y2, "Phase (Deg.)", ImPlotAxisFlags_Opposite);
        ImPlot::SetAxis(ImAxis_Y1);
        ImPlot::PlotScatter("Gain", freqs, gains, N_F);
        ImPlot::SetAxis(ImAxis_Y2);
        ImPlot::PlotScatter("Phase", freqs, phases, N_F);
        ImPlot::EndPlot();
    }
    // ウィンドウ終了
    ImGui::End();
}
```
