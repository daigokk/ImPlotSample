#pragma once

#include <visa.h>
#include <vector>
#include <string>

class CppVisa {
public:
    static std::vector<std::string> idns;
    static std::vector<std::string> addresses;
	static std::vector<ViSession> sessions;

    static void CheckError(const ViStatus status);

    static ViSession resourceManager;
    // リソースマネージャの初期化
    static bool OpenRM();

    // リソースマネージャの終了処理
    static void CloseRM();

    // 計測器のオープン
    static ViSession OpenInstrument(const char address[]);

    // 計測器のクローズ
    static void CloseInstrument(ViSession instrument);

	// 計測器へコマンド送信（可変引数）必ず改行コード(\n)を含めること。
    static bool Printf(const ViSession instrument, const char* format, ...);

    // 計測器からの応答受信
    // 注意：formatには %t を使用。%s は空白区切りの最初の単語しか取得できない。
    static bool Scanf(const ViSession instrument, const char* format, void* output);

    // クエリ送信と応答取得（可変引数）
    static std::string QueryfString(const ViSession instrument, const char* format, ...);
    static char* Queryf(const ViSession instrument, const char* format, ...);

	static char Getchar(ViSession vi);

    static bool GetIdn(const ViChar* instrDesc, char* ret);

	static void FindRsrc();

    static bool GetAddress(const char keyword[], char address[]);
};
