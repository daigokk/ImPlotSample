#pragma once

#pragma comment(lib, "./VISA/visa64.lib")
#include <visa.h>

// エラーチェック用ユーティリティ関数
void vi_checkError(ViStatus status, const char* filename, int line);

// VISAリソース検索
void vi_FindRsrc(const ViSession resourceManager);

// 波形保存（可変引数）
int scope_save_Waveformsf(const ViSession instrument, const char* format, ...);

class CVisa {
private:
    static ViSession resourceManager;

public:
    // リソースマネージャの初期化
    static bool OpenRM(const char* filename, const int line);

    // リソースマネージャの終了処理
    static void CloseRM();

    // 計測器のオープン
    static ViSession OpenInstrument(const char* address, const char* filename, const int line);

    // 計測器のクローズ
    static void CloseInstrument(ViSession instrument);

	// 計測器へコマンド送信（可変引数）必ず改行コード(\n)を含めること。
    static bool cviPrintf(const ViSession instrument, const char* filename, const int line, const char* format, ...);

    // 計測器からの応答受信
    // 注意：formatには %t を使用。%s は空白区切りの最初の単語しか取得できない。
    static bool cviScanf(const ViSession instrument, const char* filename, const int line, const char* format, void* output);

    // クエリ送信と応答取得（可変引数）
    static char* cviQueryf(const ViSession instrument, const char* filename, const int line, const char* format, ...);
};