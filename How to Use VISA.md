# 🧪 VISAとは？Virtual Instrument Software Architecture

## 概要

VISA（Virtual Instrument Software Architecture、ビサ）は、計測器とコンピュータの間の通信を抽象化するためのソフトウェアインターフェースです。National Instruments（NI）によって開発され、現在では多くのベンダーがサポートしています。

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

| 関数名         | 役割 |
|----------------|------|
| `viOpenDefaultRM` | リソースマネージャの初期化 |
| `viFindRsrc`      | 利用可能な機器の検索 |
| `viOpen`          | 機器への接続 |
| `viWrite`         | コマンド送信 |
| `viRead`          | 応答受信 |
| `viPrintf` / `viScanf` | フォーマット付き送受信 |
| `viQuery` | フォーマット付き送受信。viPrintf` / `viScanf`を連続して実行する。 |
| `viClose`         | 接続終了 |

---

## 📄 SCPIとの関係

VISAは通信の手段であり、SCPI（Standard Commands for Programmable Instruments、スキッピ）は通信の内容（コマンド体系）です。SCPIは多くの計測器で使われており、VISA経由で `*IDN?` や `MEAS:VOLT?` などのコマンドを送信します。

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
#include <visa.h>
#include <iostream>
#include <string>

int main() {
    ViSession defaultRM, instr;
    char buffer[256] = {0};

    // VISAリソースマネージャを初期化
    viOpenDefaultRM(&defaultRM);
    // 計測器に接続（例: USB接続のオシロスコープ）
    viOpen(defaultRM, "USB0::????????::INSTR", VI_NULL, VI_NULL, &instr);
    // SCPIコマンド送信
    viPrintf(instr, "*IDN?\n");
    // 応答受信（最大255文字）
    viScanf(instr, "%255t", buffer);
    printf("Instrument ID: %s\n", buffer);

    // 接続を閉じる
    viClose(instr);
    viClose(defaultRM);
    return 0;
}
```
