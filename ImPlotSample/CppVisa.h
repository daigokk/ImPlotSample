#pragma once

#include <visa.h>


class CppVisa {
public:
    static ViSession resourceManager;
    // リソースマネージャの初期化
    static bool OpenRM(const char* filename, const int line);

    // リソースマネージャの終了処理
    static void CloseRM();

    // 計測器のオープン
    static ViSession OpenInstrument(const char address[], const char* filename, const int line);

    // 計測器のクローズ
    static void CloseInstrument(ViSession instrument);

	// 計測器へコマンド送信（可変引数）必ず改行コード(\n)を含めること。
    static bool Printf(const ViSession instrument, const char* filename, const int line, const char* format, ...);

    // 計測器からの応答受信
    // 注意：formatには %t を使用。%s は空白区切りの最初の単語しか取得できない。
    static bool Scanf(const ViSession instrument, const char* filename, const int line, const char* format, void* output);

    // クエリ送信と応答取得（可変引数）
    static char* Queryf(const ViSession instrument, const char* filename, const int line, const char* format, ...);

    static void GetIdn(const ViChar* instrDesc, char* ret);

	static void FindRsrc();
};
