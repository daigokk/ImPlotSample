# 課題

## C言語の復習
1. C言語の基本構文(Lv. 1)
   - 標準出力(コンソール画面:黒いWindow)に`Hello world`を表示する。
1. scanfとprintf(Lv. 1)
   - 標準入力(キーボード)から入力された実数を、標準出力に表示する。
1. if文(Lv. 2)
   - 標準入力から入力された整数の、奇数または偶数を判定する。判定結果は標準出力に表示する。
1. for文(Lv. 3)
   - for文のネスト(入れ子構造)を用いて以下の文字列を出力する。
     ```
     A B C D E F G H I J
     K L M N O P Q R S T
     U V W X Y Z [ \ ] ^
     _ ` a b c d e f g h
     i j k l m n o p q r
     s t u v w x y z { |
     ```        
1. 配列(Lv. 4)
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
1. 配列(Lv. 5)
   - 以下のプログラムは配列に記録されている数値を昇順に並び替えて画面に表示して終了する。関数sortの定義部分を作成しなさい。
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
1. for文と数学関数(sin等)(Lv. 3)
   - 標準入力から得た振幅と位相を用いて、1kHzの正弦波1周期分の時間と値を`,`区切りの実数で標準出力に表示する。ただし、`dt`は1e-8sとする。
1. ファイル操作（fopen, fprintf, fclose）(Lv. 4)
   - 上記の結果を`data.csv`ファイルに出力する。
1. データの可視化(Lv. 100)
   - `data.csv`の値を[ImPlot](https://github.com/epezent/implot)を用いてグラフにして表示する。

---

## 信号処理
1. 位相敏感検波
1. FFT
1. ローパスフィルタ
1. ハイパスフィルタ

---

## 計測器との連携
1. VISA、SCPIの理解
2. 計測器（オシロスコープやファンクションジェネレータなど）への設定の書き込み、及び読み込み

---

## 応用: 周波数フィルタの特性測定

---

# 参考資料
## [Visual studioの使い方](./VisualStudio.md)
## [C言語 復習資料](./Clang.md)
## [計測器の共通言語！VISAとは？](./VISA.md)
