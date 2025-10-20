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
    double x;
    scanf("%lf", &x);
    printf("入力された値: %f\n", x);
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
            printf("%c ", ch++);
        }
        printf("\n");
    }
    return 0;
}
```
- `char`型は数値としても扱える。

---

### 5. if文とfor文と文字配列（大文字変換）（Lv. 4）
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

### 8. sin波形生成（Lv. 3）
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
- 1kHzの波形を1周期分生成。
- DegreeからRadianに変換してから`sin`関数に入れる。

---

### 9. CSV出力（Lv. 5）
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
- `rand()`で乱数生成、円内判定。

---
