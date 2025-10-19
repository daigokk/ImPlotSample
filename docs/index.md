# 課題

## C言語の復習

「コンピュータプログラミング実習」、「コンピュータプログラミン」で習得した内容を確認します。

1. C言語の基本構文(Lv. 1、15分、評価基準: 正しく「Hello world」が表示される)
   - 標準出力(コンソール画面:黒いWindow)に`Hello world`を表示する。
1. scanfとprintf(Lv. 1、15分、評価基準: 入力された実数が正しく表示される)
   - 標準入力(キーボード)から入力された実数を、標準出力に表示する。
1. if文(Lv. 2、15分、評価基準: 奇数・偶数の判定が正しく行われる)
   - 標準入力から入力された整数の、奇数または偶数を判定する。判定結果は標準出力に表示する。
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
1. 配列(Lv. 4、45分、評価基準: minimax関数が正しく定義され、結果が正しく表示される。加点例: 平均$`\bar{x}`$や不偏標準偏差$`\sigma = \sqrt{\frac{1}{N-1} \sum_{i=1}\^{N} (x_i - \bar{x})\^2}`$を表示する。)
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
1. 配列(Lv. 5、60分、評価基準: sort関数が正しく定義され、配列が昇順に並ぶ)
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
1. for文と数学関数(sin等)(Lv. 3、30分、評価基準: 1kHzの波形が正しく生成され、時間・値が正確に出力される)
   - 標準入力から得た振幅、及び位相を用いて、1kHzの正弦波1周期分の時間と値を`,`区切りの実数で標準出力に表示する。ただし、`dt`は1e-8sとする。
1. 測定データの保存（fopen, fprintf, fclose）(Lv. 5、15分、評価基準: data.csvが正しく作成され、内容が正確に記録されている)
   - 上記の結果を`data.csv`ファイルに出力する。
1. 外部ライブラリを用いたデータの可視化(Lv. 100、、200分、評価基準: グラフが正しく表示され、ラベル・軸が適切に設定されている)
   - `data.csv`の値を[ImPlot](https://github.com/epezent/implot)を用いてグラフにして表示する。

## 参考資料
### [Visual Studioの使い方](./VisualStudio.md)
### [C言語 復習資料](./Clang.md)

---

## 信号処理
1. `rand`関数を用いた雑音を含んだ模擬測定データの作成
1. 位相敏感検波
1. FFT
1. ローパスフィルタ
1. ハイパスフィルタ

---

## 計測器との連携
1. VISA、SCPIの理解
2. 計測器（オシロスコープやファンクションジェネレータなど）への設定の書き込み、及び読み込み

# 参考資料
## [計測器の共通言語！VISAとは？](./VISA.md)

---

## 応用: 周波数フィルタの特性測定
