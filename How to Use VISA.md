# 🧪 VISAとは？Virtual Instrument Software Architecture

## 概要

VISA（Virtual Instrument Software Architecture、ビサ）は、計測器とコンピュータの間の通信方法の違いを気にせず、同じ方法で操作できるようにするためのソフトウェアインターフェースです。NI（National Instruments）によって開発され、現在では多くのメーカーが対応しています。

例えば、USBやLAN、GPIBなど異なる接続方式でも、VISAを使えば同じ命令で機器を制御できます。

---

## 🎯 VISAの目的

- 異なる通信方式（GPIB, USB, RS-232, TCP/IP など）を統一的なAPIで扱えるようにする
- 計測器制御を簡素化・標準化する
- 複数ベンダーの機器を同じコードで制御可能にする

---

## 🔌 対応する通信方式

| 通信方式 | 説明 |
|----------|------|
| GPIB     | HPが開発した古典的な計測器通信。多くのラボ機器で使用 |
| USB      | USBTMC（Test & Measurement Class）対応機器 |
| RS-232   | シリアル通信。古い機器でよく使われる |
| TCP/IP   | LAN経由での通信。SCPI-over-Socketなど |
| PXI/VXI  | モジュール型計測器システム |

---

## 🧰 主なVISA関数（C/C++）

| 関数名         | 役割 | |
|----------------|------|-----|
| `viOpenDefaultRM` | リソースマネージャの初期化 | `status = viOpenDefaultRM(&defaultRM);` |
| `viFindRsrc`      | 利用可能な機器の検索 | `status = viFindRsrc(resourceManager, "?*INSTR", &findList, &numInstrs, instrDesc);`|
| `viOpen`          | 機器への接続 | `status = viOpen(defaultRM, "USB0::????????::INSTR", VI_NULL, VI_NULL, &instr);` |
| `viWrite`         | コマンド送信。バイナリ/テキスト問わず | `status = viWrite(instr, (ViBuf)"*IDN?\n", 6, &count);`|
| `viRead`          | 応答受信。バイナリ/テキスト問わず | `status = viRead(instr, (ViBuf)buffer, sizeof(buffer), &count);`|
| `viPrintf` / `viScanf` | フォーマット付き送受信 | `status = viPrintf(instr, "*IDN?\n");`<br>'status = viScanf(instr, "%255t", buffer);'|
| `viQueryf` | フォーマット付き送受信<br>`viPrintf`, `viScanf`を連続して実行する。 | `status = viQueryf(instr, "*IDN?\n", "%t", idn);`|
| `viClose`         | 接続終了 | `viClose(instr);''viClose(defaultRM);` |

---

## 📄 SCPIとの関係

VISAは「通信の方法」を統一する仕組みであり、SCPI（Standard Commands for Programmable Instruments、スキッピ）は「通信の中身（命令）」を標準化する仕組みです。

例えば、`*IDN?` というSCPIコマンドは「あなたは誰ですか？」という意味で、機器のメーカー名や型番などを返します。VISAを使えば、このコマンドをどの接続方式でも同じように送ることができます。

---

## 📦 VISAアドレスの例

| アドレス形式 | 意味 |
|--------------|------|
| `GPIB0::5::INSTR` | GPIBバスの5番ポート |
| `USB0::0x1234::0x5678::INSTR` | USB接続の機器（Vendor/Product ID） |
| `TCPIP0::192.168.0.10::INSTR` | LAN接続の機器 |

---

## 🧭 補助ツール

- **NI MAX (Measurement & Automation Explorer)**  
  VISAアドレスの確認、接続テスト、デバッグに便利なGUIツール。

---

## 🧠 まとめ

VISAは、計測器制御の世界で欠かせない共通言語のような存在です。複雑な通信方式を隠蔽し、統一されたAPIで機器を扱えるため、開発効率と保守性が大幅に向上します。

## 最小のサンプルプログラム
```cpp
// このサンプルは、USB接続されたSCPI対応機器に対して識別情報を取得する最小構成の例です。
// "USB0::????????::INSTR" の部分は、NI MAXで確認した実際のアドレスに置き換えてください。

#pragma comment(lib, "visa64.lib")
#include <visa.h>
#include <stdio.h>

int main() {
    ViSession defaultRM, instr;
    char buffer[256] = {0};

    viOpenDefaultRM(&defaultRM); // VISAリソースマネージャを初期化
    viOpen(defaultRM, "USB0::????????::INSTR", VI_NULL, VI_NULL, &instr); // 計測器に接続
    viPrintf(instr, "*IDN?\n"); // SCPIコマンド送信
    viScanf(instr, "%255t", buffer); // 応答受信
    printf("Instrument ID: %s\n", buffer); // 結果表示

    viClose(instr); // 接続を閉じる
    viClose(defaultRM);
    return 0;
}
```
