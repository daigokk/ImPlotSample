## 🧠 C言語復習課題 模範コード＆解説

### 1. Hello World（Lv.1）
```c
#include <stdio.h>
int main() {
    printf("Hello world\n");
    return 0;
}
```
- `printf()`で文字列を標準出力に表示。

---

### 2. scanfとprintf（Lv.1）
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

### 3. if文で奇数・偶数判定（Lv.2）
```c
#include <stdio.h>
int main() {
    int x;
    scanf("%d", &x);
    if (x % 2 == 0)
        printf("偶数です\n");
    else
        printf("奇数です\n");
    return 0;
}
```
- `%`演算子で剰余を取得。

---

### 4. for文で文字列整形（Lv.3）
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

### 5. minimax関数（Lv.4）
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

### 6. sort関数（Lv.5）
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

### 7. sin波形生成（Lv.3）
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

### 8. CSV出力（Lv.5）
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
### 9. ImPlot
- 省略
---

### 10. モンテカルロ法で円周率（Lv.8）
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
