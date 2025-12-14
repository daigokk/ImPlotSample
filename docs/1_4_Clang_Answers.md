## 🧠 C言語復習課題 模範コード＆解説

### 1. Hello World（Lv. 1）
```c
#include <stdio.h>
int main() {
    printf("Hello world\n");
    return 0;
}
```
- `printf()`で文字列を標準出力に表示。

---

### 2. scanfとprintf（Lv. 1）
```c
#include <stdio.h>
int main() {
    double tall;
    printf("How tall are you?> ");
    scanf("%lf", &tall);
    printf("Tall> %f\n", tall);
    return 0;
}
```
- `%lf`はdouble型の入力に使用。

---

### 3. if文で奇数・偶数判定（Lv. 2）
```c
#include <stdio.h>
int main(void) {
    int num;
    printf("Input> ");
    scanf("%d", &num);
    if (num % 2 == 0)
        printf("Output> Even number\n");
    else
        printf("Output> Odd number\n");
    return 0;
}
```
- `%`演算子で剰余を取得。

---

### 4. for文で文字列整形（Lv. 3）
```c
#include <stdio.h>
int main() {
    char ch = 'A';
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%c ", ch+6*j);
        }
        printf("\n");
        ch++;
    }
    return 0;
}
```
- `char`型は数値としても扱える。

---

### 5. for文とif文と文字配列（大文字変換）（Lv. 4）
```c
#include <stdio.h>
int main(void) {
    char str[256], def = 'A' - 'a';
    printf("Input> ");
    scanf("%255s", str);
    printf("Output> ");
    for (int i = 0; str[i] != '\0'; i++) {
        if('a' <= str[i] && str[i] <= 'z') {
            str[i] -= def;
        }
    }
    printf("%s\n", str);
    return 0;
}
```
### 6. minimax関数（Lv. 4）
```c
void minimax(int length, double *arr, double *minimum, double *maximum) {
    *minimum = *maximum = arr[0];
    for (int i = 1; i < length; i++) {
        if (arr[i] < *minimum) *minimum = arr[i];
        if (arr[i] > *maximum) *maximum = arr[i];
    }
}
```
- ポインタで結果を返す。

---

### 7. sort関数（Lv. 5）
```c
void sort(int length, double *arr) {
    for (int i = 0; i < length - 1; i++) {
        for (int j = i + 1; j < length; j++) {
            if (arr[i] > arr[j]) {
                double temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}
```
- バブルソートの簡易版。

---

### 8. sin波形生成（Lv. 5）
```c
#include <stdio.h>
#include <math.h>
#define PI acos(-1)
int main() {
    double amp, phase_deg;
    double omega = 2 * PI * 100e3;
    scanf("%lf %lf", &amp, &phase_deg);
    double phase_rad = phase_deg / 180.0 * PI;
    for (int i = 0; i < 5000; i++) {
        double t = i * 1e-8;
        double y = amp * sin(omega * t + phase_rad);
        printf("%e,%e\n", t, y);
    }
    return 0;
}
```
- 100kHzの波形を5周期分生成。
- DegreeからRadianに変換してから`sin`関数に入れる。

---

### 9. CSV出力（Lv. 6）
```c
#include <stdio.h>
#include <math.h>
#define PI acos(-1)
int main() {
    FILE *fp = fopen("data.csv", "w");
    if (fp == NULL) return 1;
    for (int i = 0; i < 5000; i++) {
        double t = i * 1e-8;
        double y = sin(2 * PI * 100e3 * t);
        fprintf(fp, "%e,%e\n", t, y);
    }
    fclose(fp);
    return 0;
}
```
- `fprintf()`でCSV形式に出力。
---
### 10. ImPlot（Lv. 100）
- 波形データ保存
```c
// 生成
for (int i = 0; i < SIZE; i++) {
    waveform[i] = amplitude * std::sin(2 * PI * frequency * i * DT + phase_rad);
}

// 保存
FILE* fp = fopen(FILENAME, "w");
if (fp != NULL) {
    fprintf(fp, "# Time (s), Voltage (V)\n");
    for (int i = 0; i < SIZE; ++i) {
        fprintf(fp, "%e, %e\n", i * DT, waveform[i]);
    }
    fclose(fp);
    text = "Success.\n";
}
else {
    text = "[Error] Failed to open file for writing\n";
}
```
- 波形データ読み込み
```c
FILE* fp = fopen(FILENAME, "r");
char buf[256];
if (fp != NULL) {
    // 1行目は無視する
    fgets(buf, sizeof(buf), fp);  // 1行目を読み飛ばす
    for (int i = 0; i < SIZE; i++) {
        fscanf(fp, "%lf,%lf", &times[i], &waveform[i]);
    }
    fclose(fp);
    text = "Success.\n";
}
else {
    text = "[Error] Failed to open file for reading\n";
}
```
---

### 11. モンテカルロ法で円周率（Lv. 8）
```c
#include <stdio.h>
#include <stdlib.h> // for srand, rand
#include <time.h>
#include <math.h>
#define PI acos(-1)
int main() {
    int N = 1000000, count = 0;
    double pi;
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        double x = (double)rand() / RAND_MAX * 2 - 1;
        double y = (double)rand() / RAND_MAX * 2 - 1;
        if (x*x + y*y <= 1) count++;
        pi = 4.0 * count / (i + 1);
        if (fabs(pi - PI) < 1e-5) break;
        printf("%d: %f\n", i+1, pi);
    }
    printf("π ≒ %.6f\n", pi);
    return 0;
}
```
- `rand()`で乱数生成、次に円内判定。

---

### 12. ニュートン・ラプソン法
```c
#include <stdio.h>
#include <math.h> // fabs()を使うために必要

int main() {
    double x = 1.0;        // 初期値（適当な正の数）
    double prev_x;         // 前回のxの値を保持する変数
    const double EPSILON = 0.000000001; // 許容誤差（これ以下なら計算終了）
    int max_iter = 100;    // 無限ループ防止用の最大反復回数
    int i;

    printf("ニュートン法によるルート2の導出:\n");
    printf("初期値: %f\n\n", x);

    for (i = 1; i <= max_iter; i++) {
        prev_x = x; // 現在の値を保存

        // ニュートン法の更新式: x = (x + 2/x) / 2
        x = (x + 2.0 / x) / 2.0;

        printf("%d回目: %.15f\n", i, x);

        // 収束判定: 前回の値との差が誤差範囲内なら終了
        // fabsは絶対値を求める関数です
        if (fabs(x - prev_x) < EPSILON) {
            printf("\n収束しました。\n");
            break;
        }
    }

    printf("\n--- 結果 ---\n");
    printf("計算結果: %.15f\n", x);
    printf("実際の値: %.15f (sqrt(2))\n", sqrt(2.0));

    return 0;
}
```

---

### 13. モンティ・ホール問題
```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int i;
    int trials = 10000; // 試行回数
    int win_stay = 0;   // 変えないで勝った回数
    int win_switch = 0; // 変えて勝った回数

    // 乱数の種を初期化（これがないと毎回同じ結果になります）
    srand(time(NULL));

    printf("モンティ・ホール問題を %d 回シミュレーションします...\n", trials);

    for (i = 0; i < trials; i++) {
        // 1. 当たりのドアを決める (0, 1, 2 のいずれか)
        int car = rand() % 3;

        // 2. プレイヤーが最初に選ぶドアを決める
        int choice = rand() % 3;

        // 3. 司会者(モンティ)が開けるドアを決める
        // 司会者は「当たり(car)ではなく」かつ「プレイヤーが選んだドア(choice)ではない」ドアを開ける
        int monty_open;
        do {
            monty_open = rand() % 3;
        } while (monty_open == car || monty_open == choice);

        // --- 勝敗判定 ---

        // 戦略A: ドアを変えない場合 (Stay)
        // 最初の選択が当たりなら勝ち
        if (choice == car) {
            win_stay++;
        }

        // 戦略B: ドアを変える場合 (Switch)
        // 「残っているドア」に変える。
        // つまり、今の選択(choice)でもなく、モンティが開けたドア(monty_open)でもないドアが新しい選択になる。
        // 実はもっと単純に、「最初にハズレを選んでいれば、変えると必ず当たる」というロジックでも判定可能ですが、
        // ここでは丁寧にシミュレーションします。
        int new_choice;
        for (int j = 0; j < 3; j++) {
            if (j != choice && j != monty_open) {
                new_choice = j;
                break;
            }
        }

        if (new_choice == car) {
            win_switch++;
        }
    }

    // 結果発表
    printf("\n--- 結果 ---\n");
    printf("変えない場合の勝率: %.2f%%\n", (double)win_stay / trials * 100.0);
    printf("変える場合の勝率  : %.2f%%\n", (double)win_switch / trials * 100.0);

    return 0;
}
```
