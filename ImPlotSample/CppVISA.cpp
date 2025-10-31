#include "CppVisa.h"

#pragma comment(lib, "./VISA/visa64.lib")

#include <cstdarg>   // va_list など
#include <iostream>

ViSession CppVisa::resourceManager = VI_NULL;

void vi_checkError(const ViStatus status, const char filename[], const int line)
{
    if (status < VI_SUCCESS)
    {
        char buffer[256];
        ViSession rm;
        viOpenDefaultRM(&rm);
        viStatusDesc(rm, status, buffer);
        printf("\a");
        printf("--- VISA error code: %10d ---\n", status);
        printf(" %s\n", buffer);
        printf(" File: %s, Line: %3d ---\n", filename, line);
        printf("--- 小坂のコメント----------------------------------------------\n");
        switch (status)
        {
        case -1073807343: //viOpen error
            printf(" 計測器が見つからないという意味のエラーです。\n");
            printf(" VISAリソース名を間違えているとこのエラーが出ます。NI MAXの画面上で表示されるリソース名(USB0::????????::INSTR)をコピーして使いましょう。\n");
            printf(" まさか、計測器の電源が入っていない、ということはありませんか？\n");
            printf(" もしくは、計測器と自分が使っているPCにUSBケーブルで接続されているか確認してみましょう。\n");
            printf(" オシロスコープ(DLM2022)の場合、USBケーブルをLANのコネクタにさしてしまう人がよくいます。\n");
            break;
        case -1073807346: // 
            printf(" 計測器のハンドルを取得せずに通信しようとすると出るエラーです。viOpenを実行していないか、失敗しています。\n");
            printf(" viOpenで指定するVISAリソース名(USB0::????????::INSTR)を確認してください。NI MAXの画面上で表示されるリソース名をコピーして使いましょう。\n");
            break;
        case -1073807339: //Timeout
            printf(" viScanfを実行した際に計測器から答えが帰って来なかった時に出るエラーです。\n");
            printf(" 直前にviPrintfで送った問い合わせコマンド(最後に?がついている)を間違えているか、改行コード(\\n)を送っていない可能性があります。\n");
            printf(" 問い合わせコマンドを送っていない場合は、viScanfの行を削除してください。\n");
            printf(" 問い合わせコマンドを送る必要がある場合は、直前のviPrintfの行を確認してください。\n");
            break;
        default:
            printf(" 何かがおかしい、何かを間違えています。\n");
            printf(" まず、どの行で出ているエラーなのかを確認してください。\n");
            printf(" ソースコードを10回読んでもわからなければ、小坂を呼んでください。原因を一緒に考えましょう。\n");
        }
        printf("----------------------------------------------------------------\n");
        viClose(rm);
        exit(-1);
    }
}


bool CppVisa::OpenRM(const char* filename, const int line) {
    ViStatus status = viOpenDefaultRM(&resourceManager);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
    FindRsrc();
    return true;
}

ViSession CppVisa::OpenInstrument(const char address[], const char* filename, const int line) {
    if (resourceManager == VI_NULL) {
        std::cerr << "[Error] VISAリソースマネージャーが初期化されていません。CppVisa::OpenRMを実行してください。\n";
    }
    ViSession instrument = VI_NULL;
    ViStatus status = viOpen(resourceManager, address, VI_NULL, VI_NULL, &instrument);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return VI_NULL;
    }
    // Timeout属性の設定（ミリ秒単位）
    status = viSetAttribute(instrument, VI_ATTR_TMO_VALUE, 100);
    if (status < VI_SUCCESS) {
        std::cerr << "[Error] viSetAttribute failed: " << status << "\n";
        return VI_NULL;
    }

    return instrument;
}

void CppVisa::CloseRM() {
    if (resourceManager) {
        viClose(resourceManager);
        resourceManager = VI_NULL;
    }
}

void CppVisa::CloseInstrument(ViSession instrument) {
    if (instrument) {
        viClose(instrument);
    }
}

bool CppVisa::Printf(const ViSession instrument, const char* filename, const int line, const char* format, ...) {
    char cmd[256];
    va_list args;
    va_start(args, format);
    vsprintf_s(cmd, format, args);
    va_end(args);
    std::cout << cmd; // デバッグ用出力
    ViStatus status = viPrintf(instrument, cmd);

    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
    return true;
}

// cviScanfにおいてformatで用いる文字列は%tを使う。%sを使うと空白で区切られた最初の単語しか読み取れない。
bool CppVisa::Scanf(const ViSession instrument, const char* filename, int line, const char* format, void* output) {
    ViStatus status = viScanf(instrument, format, output);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
    std::cout << (char*)output; // デバッグ用出力
    return true;
}

char* CppVisa::Queryf(const ViSession instrument, const char* filename, const int line, const char* format, ...) {
    char ret[256];
    va_list args;

    va_start(args, format);
    vsprintf_s(ret, format, args);
    va_end(args);
    ViStatus status = Printf(instrument, filename, line, ret);

    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return nullptr;
    }

    status = Scanf(instrument, filename, line, "%255t", ret);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return nullptr;
    }

    return ret;
}

inline void CppVisa::GetIdn(const ViChar* instrDesc, char* ret) {
    ViSession instrument;
    ViStatus status;
    status = viOpen(resourceManager, instrDesc, VI_NULL, 10, &instrument);
    if (status < VI_SUCCESS) {
        std::cerr << "計測器のオープンに失敗しました。" << std::endl;
        return;
    }
    status = viQueryf(instrument, "%s", "%255t", "*IDN?\n", ret);
    if (status < VI_SUCCESS) {
        std::cerr << "計測器の問い合わせに失敗しました。" << std::endl;
        return;
    }
    status = viClose(instrument);
    if (status < VI_SUCCESS) {
        std::cerr << "計測器のクローズに失敗しました。" << std::endl;
        return;
    }
}

void CppVisa::FindRsrc() {
    // 接続されている計測器を検索（例: GPIB, USB, TCPIPなど）
    ViStatus status;
    ViFindList findList = 0;
    ViUInt32 numInstrs;
    ViChar instrDesc[256], ret[256] = { 0 };
    status = viFindRsrc(resourceManager, "?*INSTR", &findList, &numInstrs, instrDesc);
    if (status < VI_SUCCESS) {
        std::cerr << "計測器の検索に失敗しました。" << std::endl;
        return;
    }
    std::cout << "見つかった計測器の数: " << numInstrs << std::endl;
    if (numInstrs == 0) {
        std::cout << "計測器が見つかりませんでした。" << std::endl;
        return;
    }
    // 最初の計測器を取得
    GetIdn(instrDesc, ret);
    std::cout << "1: " << instrDesc << ", " << ret << std::endl;

    // 残りの計測器を取得
    for (ViUInt32 i = 1; i < numInstrs; ++i) {
        ret[0] = '\0';
        status = viFindNext(findList, instrDesc);
        if (status < VI_SUCCESS) break;
        GetIdn(instrDesc, ret);
        std::cout << i + 1 << ": " << instrDesc << ", " << ret << std::endl;
    }
    viClose(findList);
}

char visa_viGetchar(ViSession vi)
{
    char c;
    vi_checkError(viScanf(vi, "%c", &c), __FILE__, __LINE__);
    return c;
}
