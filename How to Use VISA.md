# VISA: Virtual Instrument Software Architecture
```cpp
#include <visa.h>
#include <iostream>
#include <string>

int main() {
    ViSession defaultRM, instr;
    char buffer[256] = {0};

    // VISAリソースマネージャを初期化
    status = viOpenDefaultRM(&defaultRM);
    // 計測器に接続（例: USB接続のオシロスコープ）
    status = viOpen(defaultRM, "USB0::????????::INSTR", VI_NULL, VI_NULL, &instr);
    // SCPIコマンド送信
    viPrintf(instr, "*IDN?\n");
    // 応答受信（最大255文字）
    viScanf(instr, "%255t", buffer);
    std::cout << "Instrument ID: " << buffer << std::endl;

    // 接続を閉じる
    viClose(instr);
    viClose(defaultRM);
    return 0;
}
```
