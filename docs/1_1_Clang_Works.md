# 1.1. 課題

1. C言語の基本構文(Lv. 1、15分、評価基準: 正しく「Hello world」が表示される)
   - 標準出力(コンソール画面:黒いWindow)に`Hello world`を表示する。
     ```
     Hello world
     ```
1. scanfとprintf(Lv. 1、15分、評価基準: 入力された実数が正しく表示される)
   - 標準入力(キーボード)から入力された実数を、標準出力に表示する。
     ```
     How tall are you?> 185
     Tall> 185.000000
     ```
1. if文(Lv. 2、15分、評価基準: 奇数・偶数の判定が正しく行われる)
   - 標準入力から入力された整数の、奇数または偶数を判定する。判定結果は標準出力に表示する。
     ```
     Input> 5
     Output> Odd number
     ```
1. for文(Lv. 3、30分、評価基準: 指定された文字列が正しく整形されて出力される。加点例: 文字列の並びを縦方向、または逆方向にする。)
   - for文のネスト(入れ子構造)を用いて以下の文字列を出力する。
     ```
     A B C D E F G H I J
     K L M N O P Q R S T
     U V W X Y Z [ \ ] ^
     _ ` a b c d e f g h
     i j k l m n o p q r
     s t u v w x y z { |
     ```
1. if文とfor文と文字配列(Lv. 4、30分、評価基準: 大文字で出力される。加点例: 小文字は大文字、大文字は小文字に変換する。)
   - 標準入力から入力された文字列を大文字に変換して出力する。
     ```
     Input> Ptu
     Output> PTU
     ```
1. 配列と関数(Lv. 4、45分、評価基準: minimax関数が正しく定義され、結果が正しく表示される。加点例: 平均$\bar{x}$や不偏標準偏差$ \sigma = \sqrt{\frac{1}{N-1} \sum_{i=1}^{N} (x_i - \bar{x})^2} $を表示する。)
   - 配列に記録されている数値の最小値と最大値を画面に表示して終了する。関数minimaxの定義部分を作成する。
     ```c
     #include <stdio.h>
     void minimax(int length, double *arr, double *minimum, double *maximum);
     int main(void) {
         double arr[] = { 1,5,4,3,2 }, minimum, maximum;
         minimax(5, arr, &minimum, &maximum);
         printf("%f, %f\n", minimum, maximum);
         return 0;
     }
     
     void minimax(int length, double *arr, double *minimum, double *maximum) {
         // ここに適切なコードを記入

         
     }
     ```
1. 配列と関数(Lv. 5、60分、評価基準: sort関数が正しく定義され、配列が昇順に並ぶ)
   - 以下のプログラムは配列に記録されている数値を昇順に並び替えて画面に表示して終了する。関数sortの定義部分を作成する。
     ```c
     #include <stdio.h>
     void sort(int length, double *arr);
     int main(void) {
         double arr[] = { 1,5,4,3,2 };
         sort(5, arr);
         for (int i = 0; i < 5; i++) {
             printf("%f\n", arr[i]);
         }
         return 0;
     }
     
     void sort(int length, double *arr) {
         // ここに適切なコードを記入
         
         
     }
     ```
1. for文と数学関数(sin等)(Lv. 3、30分、評価基準: 100kHzの波形が正しく生成され、時間・値が正確に出力される)
   - 標準入力から得た振幅、及び位相を用いて、100kHzの正弦波5周期分の時間と値を`,`区切りの実数で標準出力に表示する。ただし、`dt`は1e-8sとする。
     ```cpp
      #include <stdio.h>
      #include <math.h>
      
      #define PI acos(-1)
      #define DT 1e-8
      #define FREQ 100e3
      #define PERIOD 5
      #define SIZE (int)(PERIOD/FREQ/DT)
      
      void sine_wave(double amplitude, double phase_rad, double frequency, double dt, int size, double x[], double y[]);
      void print_array(double x[], double y[], int size);
      
      int main() {
          double amplitude, phase;
          double time[SIZE], volts[SIZE];
      
          printf("Input Amplitude> ");
          scanf("%lf", &amplitude);
          printf("Input Phase (in degrees)> ");
          scanf("%lf", &phase);
      
          // Convert phase from degrees to radians
          phase = phase * PI / 180.0;
      
          sine_wave(amplitude, phase, FREQ, DT, SIZE, time, volts);
          print_array(time, volts, SIZE);
      
          return 0;
      }
      
      void sine_wave(double amplitude, double phase_rad, double frequency, double dt, int size, double x[], double y[]) {
          // ここに適切なコードを記入

     
      }
      
      void print_array(double x[], double y[], int size){
          printf("# Time,Value\n");
          // ここに適切なコードを記入

     
      }
     ```
1. 測定データの保存（fopen, fprintf, fclose）(Lv. 5、15分、評価基準: data.csvが正しく作成され、内容が正確に記録されている)
   - 上記の結果を`data.csv`ファイルに出力する。
1. 外部ライブラリを用いたデータの可視化(Lv. 100、200分、評価基準: グラフが正しく表示され、ラベル・軸が適切に設定されている)
   - `data.csv`の値を[ImPlot](https://github.com/epezent/implot)を用いてグラフにして表示する。
   - [これ](https://github.com/daigokk/ImPlotSample/tree/master)を使えば簡単(Lv. 50くらい)になります。
     ![Raw waveform](.images/Clang_10.png)
1. モンテカルロ法を用いた円周率の導出(Lv. 8、60分、評価基準: 円周率を6桁(3.14159)以上表示する。加点例: `math.h`を用いない解法。マクローリン展開を用いた円周率の導出)
   - -1～1までの値を取る二つの乱数`x`,`y`を求める。
   - 得られた`xy`座標上の点が、原点を中心とする半径1mの円の中にあるかを判定する。
   - 上記を標準入力から得た`n`の回数実施し半径1mの円(面積$\pi \rm m^2$)の中にある点の数`k`回と一辺が2mの正方形(面積$4 \rm m^2$)の中にある点の数`n`の比から得られる式$\pi/4=k/n$を用いて円周率$\pi$を求める。
   - 検算には`math.h`を用いてもよい。

### 参考資料
- [Visual Studioの使い方](./1_3_VisualStudio.md)
- [C言語 on Colab](https://colab.research.google.com/drive/1fewkHpqIm40EXWWdZ9eu6EUIN9MWQNO4)
- [C言語 課題解決チートシート](./1_2_Clang_CheatSheet.md)
- [解答](./1_4_Clang_Answers.md), [解答 on Colab](https://colab.research.google.com/drive/1RiSF2mSxuruAgYWGswEd85AlZJ4JpZQR)
