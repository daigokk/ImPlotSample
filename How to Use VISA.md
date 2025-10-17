# VISA: Virtual Instrument Software Architecture
```cpp
#include <visa.h>
#include <iostream>
#include <string>

int main() {
    ViSession defaultRM, instr;
    ViStatus status;
    ViUInt32 retCount;
    char buffer[256] = {0};

    // VISAリソースマネージャを初期化
    status = viOpenDefaultRM(&defaultRM);
    if (status != VI_SUCCESS) {
        std::cerr << "Failed to open default resource manager.\n";
        return 1;
    }

    // 計測器に接続（例: USB接続のオシロスコープ）
    status = viOpen(defaultRM, "USB0::0x1234::0x5678::INSTR", VI_NULL, VI_NULL, &instr);
    if (status != VI_SUCCESS) {
        std::cerr << "Failed to open instrument.\n";
        viClose(defaultRM);
        return 1;
    }

    // SCPIコマンド送信（例: *IDN?）
    status = viWrite(instr, (ViBuf)"*IDN?\n", 6, &retCount);
    if (status != VI_SUCCESS) {
        std::cerr << "Failed to write to instrument.\n";
        viClose(instr);
        viClose(defaultRM);
        return 1;
    }

    // 応答を読み取り
    status = viRead(instr, (ViBuf)buffer, sizeof(buffer), &retCount);
    if (status == VI_SUCCESS) {
        std::cout << "Instrument response: " << buffer << std::endl;
    } else {
        std::cerr << "Failed to read from instrument.\n";
    }

    // 接続を閉じる
    viClose(instr);
    viClose(defaultRM);
    return 0;
}
```
