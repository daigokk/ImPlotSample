# 🧠 C言語 復習資料

---

## 基本構文：最初のプログラム「Hello, World!」
```c
#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    return 0;
}
```
- `#include <stdio.h>`：標準入出力ライブラリを読み込む
- `main()`：プログラムの開始点
- `printf()`：文字列を表示
- `return 0;`：正常終了を示す

---

## 変数とデータ型、及び演算子

### 🔹 変数とデータ型

| 型名     | 説明           | 例           |
|----------|----------------|--------------|
| int      | 整数           | `int x = 5;` |
| char     | 1文字          | `char c = 'A';` |
| double   | 倍精度実数     | `double d = 2.718;` |

### 🔹 演算子
- `=`（代入、イコールではない）
- `+`, `-`, `*`, `/`, `%`（加減乗除、剰余）
- `==`, `!=`, `<`, `>`（比較）

---

## 条件分岐と繰り返し
### 🔸 if文
```c
if (x > 0) {
    printf("正の数です\n");
} else {
    printf("0以下です\n");
}
```

### 🔸 for文
```c
for (int i = 0; i < 5; i++) {
    printf("%d\n", i);
}
```

### 🔸 while文
```c
int i = 0;
while (i < 5) {
    printf("%d\n", i);
    i++;
}
```

---

## 関数の使い方
```c
#include <stdio.h>

int add(int a, int b); // 関数のプロトタイプ宣言

int main() {
    int result = add(3, 4);
    printf("合計: %d\n", result);
    return 0;
}

//　関数の定義
int add(int a, int b) {
    return a + b;
}
```
- 関数は再利用可能な処理のまとまり

---

## 配列と文字列
```c
int nums[3] = {1, 2, 3};
char name[] = "Taro";
```
- 配列は同じ型のデータをまとめて扱える
- 文字列は `char` の配列として扱う

---

## ポインタ
```c
int x = 10;
int *p = &x;
printf("値: %d, アドレス: %p\n", *p, p);
```
- ポインタ(ポインタ変数)は変数の「アドレス」を扱う
- `*ポインタ` は「ポインタが指す値」、`&変数` は「アドレスを取得」
- 実は配列名はポインタ定数
- 例えば関数へ配列を渡すときに使う
 
---

## ファイル操作
```c
#include <stdio.h>

int main() {
    FILE *fp;
    int arr[5] = {10, 20, 30, 40, 50};

    fp = fopen("data.csv", "w"); // 書き込みモードでファイルを開く
    if (fp == NULL) {
        printf("ファイルを開けませんでした\n");
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        fprintf(fp, "%d\n", arr[i]); // ファイルに配列の値を書き込む
    }

    fclose(fp); // ファイルを閉じる
    printf("書き込み完了\n");

    return 0;
}

```
- `fp = fopen("data.csv", "w");`: `data.csv`ファイルを書き込みモード(`w`)で開く
- `fprintf(fp, "%d\n", arr[i]);`: ファイルに出力する（printf()のファイル版）
- `fclose(fp);`: ファイルを閉じる

---
