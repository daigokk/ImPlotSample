# 2.1. `rand`関数による雑音を含んだ模擬測定データの作成

![完成イメージ](./images/signal_01_01.png)

---

## 1. 雑音とは？
- 偶然誤差としての雑音の例(ホワイトノイズ): $v(t)=A \sin(\omega t + \theta) + noize$
- 系統誤差としての雑音の例(商用電源からのノイズ): $v(t)=A \sin(\omega t + \theta) + noize\times\sin(2\pi\times 50 t+\theta')$

---

## 2. 課題
1. [サンプルプログラム](https://github.com/daigokk/ImPlotSample/archive/refs/heads/master.zip)をダウンロードして雑音を含んだ模擬測定データ(data.csv)を作成し、表示するプログラムを完成させてください。
   1. 「Save」ボタンを押すと「data.csv」ファイルに模擬測定データ(時間, 電圧)が保存される。
      - (例)データ数: 10000
      - (例)dt: 1e-6 [s]
   1. 「View」ボタンを押すとWindowに模擬測定データの波形が表示される。
   1. ここでは雑音を含んだ信号を以下のように定義する:
      - $v(t)=A \sin(\omega t + \theta) + noize$
      - ただし、 $noize$ は「 $\pm$ 任意の値」のランダムな実数とする。
     
1. 加点例: 矩形波を作成する。
   - 矩形波: $v(t) = \sum_{i=0}^{\infty} \frac{4A}{(2i+1)\pi} \sin\left(2\pi f (2i+1) t\right)$
  
   ![矩形波](./images/signal_01_02.png)
- Generate waveform window
	```cpp
	void ShowWindow1(const char title[]) {
	    static std::string text = "";
	    static double frequency = 100e3;
	    static double amplitude = 1.0;
	    static double phase_deg = 0.0, phase_rad = 0.0;
	    static double noize = 0.0; // 追加
	    static double fft[SIZE] = { 0 };
	    // ウィンドウ開始
	    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize(ImVec2(660 * Gui::monitorScale, 220 * Gui::monitorScale), ImGuiCond_FirstUseEver);
	    ImGui::Begin(title);
	    /*** 描画したいImGuiのWidgetやImPlotのPlotをここに記述する ***/
	    ImGui::InputDouble("Frequency (Hz)", &frequency, 100.0, 1000.0, "%.1f");
	    ImGui::InputDouble("Amplitude (V)", &amplitude, 0.1, 1.0, "%.2f");
	    if (ImGui::InputDouble("Phase (Deg.)", &phase_deg, 0.1, 1.0, "%.2f")) {
	        phase_rad = phase_deg * PI / 180.0f;
	    }
	    ImGui::InputDouble("Noize (V)", &noize, 0.1, 1.0, "%.2f"); // 追加
	    if (ImGui::Button("Save")) {
	        // ボタンが押されたらここが実行される
	        // 波形データ生成
	        double times[SIZE] = { 0 }, waveform[SIZE] = { 0 };
	        /*** 適切なコードを入力 ***************************************/
	        srand(time(NULL));
	        for (int i = 0; i < SIZE; i++) {
	            times[i] = i * DT;
	            waveform[i] = amplitude * std::sin(2 * PI * frequency * times[i] + phase_deg * PI / 180.0);
	            waveform[i] += (double)rand() / RAND_MAX * 2 * noize - noize;
	        }
	        /*** ここまで *************************************************/
	        // 保存
	        /*** 適切なコードを入力 ***************************************/
	        FILE* fp = fopen(FILENAME_RAW, "w");
	        if (fp != NULL) {
	            fprintf(fp, "# Time (S), Voltage (V)\n");
	            for (int i = 0; i < SIZE; ++i) {
	                fprintf(fp, "%e, %e\n", times[i], waveform[i]);
	            }
	            fclose(fp);
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
	```

- View waveform window
  
	```cpp
	void ShowWindow2(const char title[]) {
	    static std::string text1 = "", text2 = "";
	    static double times[SIZE] = { 0 }, wf_raw[SIZE] = { 0 }, wf_lpf[SIZE] = { 0 };
	    static double freqs[SIZE] = { 0 }, amps_raw[SIZE] = { 0 }, amps_lpf[SIZE] = { 0 };
	    static double freq = 100e3, x = 0, y = 0, x_lpf = 0, y_lpf = 0;
	    static int order = 2;
	    static float lpfreq = 1e4;
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
	        Commands::WaveformParams wfp;
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
	        if(Commands::loadWaveform(&wfp, FILENAME_RAW, times, wf_raw)) {
	            text1 = "Success.";
	        }
	        else {
	            text1 = "[Error] Failed to open file for reading.";
			}
	        // PSD
	        
	        // Raw波形のFFT計算
	        
			// ローパスフィルタ処理とFFT計算
	        
	        /*** ここまで *************************************************/
	        ImPlot::SetNextAxesToFit();
	    }
	    ImGui::SameLine();
	    ImGui::Text(text1.c_str());
	    if (text1 == "Success.") {
	        ImGui::SameLine();
	        if (ImGui::Button("Save")) {
	            if (Commands::saveWaveform(&wfp, FILENAME_LPF, wf_lpf)) {
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
	```

---

## ※ ダウンロードした`sln`がビルドできない場合

- 確認1: プロジェクトの設定
  1. メニューの「プロジェクト」⇒「プロパティ」をクリック
    ![VSのメニュー](./images/vs_10.png)
  1. 「構成プロパティ」⇒「全般」⇒「プラットフォームツールセット」⇒「Visual Studio 2019(v142)」が使用しているバージョンに合わせて選択されていることを確認
    ![プロジェクトのプロパティ](./images/vs_11.png)
- 確認2: 改行コード
  
  ![ソースファイルの改行コード](./images/vs_12.png)
