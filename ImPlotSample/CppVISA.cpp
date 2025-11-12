#include "CppVisa.h"

#pragma comment(lib, "./VISA/visa64.lib")

#include <cstdarg>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>

constexpr std::size_t kBufSize = 256;
constexpr ViUInt32 kDefaultTimeoutMs = 100;

ViSession CppVisa::resourceManager = VI_NULL;
std::vector<std::string> CppVisa::idns;
std::vector<std::string> CppVisa::addresses;
std::vector<ViSession> CppVisa::sessions;

namespace {

    // 小文字化（ASCII前提）
    std::string ToLowerCopy(const std::string& s) {
        std::string t = s;
        std::transform(t.begin(), t.end(), t.begin(), [](unsigned char c) { return std::tolower(c); });
        return t;
    }

    void PrintFriendlyError(ViStatus status) {
        switch (status) {
        case VI_ERROR_RSRC_NFOUND: // -1073807343
            std::cerr
                << " 計測器が見つからないという意味のエラーです。\n"
                << " VISAリソース名を間違えているとこのエラーが出ます。NI MAXの画面上で表示されるリソース名(USB0::????????::INSTR)をコピーして使いましょう。\n"
                << " まさか、計測器の電源が入っていない、ということはありませんか？\n"
                << " もしくは、計測器と自分が使っているPCにUSBケーブルで接続されているか確認してみましょう。\n"
                << " オシロスコープ(DLM2022)の場合、USBケーブルをLANのコネクタにさしてしまう人がよくいます。\n";
            break;
        case VI_ERROR_INV_SESSION: // -1073807346
            std::cerr
                << " 計測器のハンドルを取得せずに通信しようとすると出るエラーです。viOpenを実行していないか、失敗しています。\n"
                << " viOpenで指定するVISAリソース名(USB0::????????::INSTR)を確認してください。NI MAXの画面上で表示されるリソース名をコピーして使いましょう。\n";
            break;
        case VI_ERROR_TMO: // -1073807339
            std::cerr
                << " viScanfを実行した際に計測器から答えが帰って来なかった時に出るエラーです。\n"
                << " 直前にviPrintfで送った問い合わせコマンド(最後に?がついている)を間違えているか、改行コード(\\n)を送っていない可能性があります。\n"
                << " 問い合わせコマンドを送っていない場合は、viScanfの行を削除してください。\n"
                << " 問い合わせコマンドを送る必要がある場合は、直前のviPrintfの行を確認してください。\n";
            break;
        default:
            std::cerr
                << " 何かがおかしい、何かを間違えています。\n"
                << " まず、どの行で出ているエラーなのかを確認してください。\n"
                << " ソースコードを10回読んでもわからなければ、小坂を呼んでください。原因を一緒に考えましょう。\n";
        }
    }

    void PrintVisaStatusWithDesc(ViStatus status) {
        char desc[kBufSize] = { 0 };
        ViSession rm = VI_NULL;
        if (viOpenDefaultRM(&rm) >= VI_SUCCESS) {
            viStatusDesc(rm, status, desc);
            viClose(rm);
        }
        std::cerr << "\a--- VISA error code: " << status << " ---\n"
            << " " << desc << "\n";
    }

} // namespace

void CppVisa::CheckError(const ViStatus status)
{
    if (status >= VI_SUCCESS) return;

    PrintVisaStatusWithDesc(status);
    std::cerr << "--- 小坂のコメント----------------------------------------------\n";
    PrintFriendlyError(status);
    std::cerr << "----------------------------------------------------------------\n";
    std::exit(-1);
}

bool CppVisa::OpenRM() {
    ViStatus status = viOpenDefaultRM(&resourceManager);
    if (status < VI_SUCCESS) {
        CheckError(status);
        return false;
    }
    FindRsrc();
    return true;
}

ViSession CppVisa::OpenInstrument(const char address[]) {
    if (resourceManager == VI_NULL) {
        std::cerr << "[Error] VISAリソースマネージャーが初期化されていません。CppVisa::OpenRMを実行してください。\n";
        return VI_NULL;
    }

    ViSession instrument = VI_NULL;
    ViStatus status = viOpen(resourceManager, address, VI_NULL, VI_NULL, &instrument);
    if (status < VI_SUCCESS) {
        CheckError(status);
        return VI_NULL;
    }

    status = viSetAttribute(instrument, VI_ATTR_TMO_VALUE, kDefaultTimeoutMs);
    if (status < VI_SUCCESS) {
        CheckError(status);
        return VI_NULL;
    }
	sessions.push_back(instrument);
    return instrument;
}

void CppVisa::CloseRM() {
    for(auto& session : sessions) {
        CloseInstrument(session);
	}
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

bool CppVisa::Printf(const ViSession instrument, const char* format, ...) {
    char cmd[kBufSize] = { 0 };

    va_list args;
    va_start(args, format);
#if defined(_MSC_VER)
    vsnprintf_s(cmd, kBufSize, _TRUNCATE, format, args);
#else
    vsnprintf(cmd, kBufSize, format, args);
#endif
    va_end(args);

    std::cout << cmd; // デバッグ用出力

    ViStatus status = viPrintf(instrument, "%s", cmd);
    if (status < VI_SUCCESS) {
        CheckError(status);
        return false;
    }
    return true;
}

// cviScanfでは、formatに%tを使う。%sは空白区切りの最初の単語しか読み取れない。
bool CppVisa::Scanf(const ViSession instrument, const char* format, void* output) {
    ViStatus status = viScanf(instrument, format, output);
    if (status < VI_SUCCESS) {
        CheckError(status);
        return false;
    }
    // 文字列を期待するケースではキャストして出力
    if (format && std::string(format).find("%t") != std::string::npos) {
        std::cout << static_cast<const char*>(output); // デバッグ用出力
    }
    return true;
}

// 返却値の安全性向上のため、std::string版を用意
std::string CppVisa::QueryfString(const ViSession instrument, const char* format, ...) {
    char cmd[kBufSize] = { 0 };

    va_list args;
    va_start(args, format);
#if defined(_MSC_VER)
    vsnprintf_s(cmd, kBufSize, _TRUNCATE, format, args);
#else
    vsnprintf(cmd, kBufSize, format, args);
#endif
    va_end(args);

    if (!Printf(instrument, "%s", cmd)) {
        return {};
    }

    char ret[kBufSize] = { 0 };
    if (!Scanf(instrument, "%255t", ret)) {
        return {};
    }

    return std::string(ret);
}

// char*インターフェイス（注意：バッファ寿命のため出力用staticバッファ）
char* CppVisa::Queryf(const ViSession instrument, const char* format, ...) {
    static thread_local char staticRet[kBufSize] = { 0 };

    char cmd[kBufSize] = { 0 };

    va_list args;
    va_start(args, format);
#if defined(_MSC_VER)
    vsnprintf_s(cmd, kBufSize, _TRUNCATE, format, args);
#else
    vsnprintf(cmd, kBufSize, format, args);
#endif
    va_end(args);

    // Printfはboolを返すのでエラーチェックを合わせる
    if (!Printf(instrument, "%s", cmd)) {
        return nullptr;
    }

    if (!Scanf(instrument, "%255t", staticRet)) {
        return nullptr;
    }

    return staticRet;
}

char CppVisa::Getchar(ViSession vi)
{
    char c = 0;
    CheckError(viScanf(vi, "%c", &c));
    return c;
}

inline bool CppVisa::GetIdn(const ViChar* instrDesc, char* ret) {
    ViSession instrument = VI_NULL;
    ViStatus status = viOpen(resourceManager, instrDesc, VI_NULL, 10, &instrument);
    if (status < VI_SUCCESS) {
        std::cerr << "[Error] 計測器のオープンに失敗しました。" << std::endl;
        return false;
    }

    // *IDN? に対して %255t で読み取る
    status = viQueryf(instrument, "%s", "%255t", "*IDN?\n", ret);
    if (status < VI_SUCCESS) {
        std::cerr << "[Error] 計測器の問い合わせに失敗しました。" << std::endl;
        viClose(instrument);
        return false;
    }

    status = viClose(instrument);
    if (status < VI_SUCCESS) {
        std::cerr << "[Error] 計測器のクローズに失敗しました。" << std::endl;
        return false;
    }
    return true;
}

void CppVisa::FindRsrc() {
    ViStatus status;
    ViFindList findList = 0;
    ViUInt32 numInstrs = 0;
    ViChar instrDesc[kBufSize] = { 0 };
    ViChar ret[kBufSize] = { 0 };

    status = viFindRsrc(resourceManager, "?*INSTR", &findList, &numInstrs, instrDesc);
    if (status < VI_SUCCESS) {
        std::cerr << "[Error] 計測器の検索に失敗しました。" << std::endl;
        return;
    }

    std::cout << "見つかった計測器の数: " << numInstrs << std::endl;
    if (numInstrs == 0) {
        std::cout << "[Error] 計測器が見つかりませんでした。" << std::endl;
        viClose(findList);
        return;
    }

    auto pushInstrumentInfo = [&](const ViChar* desc) {
        ret[0] = '\0';
        if (GetIdn(desc, ret)) {
            std::string idnLower = ToLowerCopy(ret);
            idns.push_back(idnLower);
            addresses.emplace_back(desc);
            std::cout << addresses.size() << ": " << desc << ", " << idnLower << std::endl;
        }
        };

    // 最初の計測器
    pushInstrumentInfo(instrDesc);

    // 残りの計測器
    for (ViUInt32 i = 1; i < numInstrs; ++i) {
        status = viFindNext(findList, instrDesc);
        if (status < VI_SUCCESS) {
            break;
        }
        pushInstrumentInfo(instrDesc);
    }

    viClose(findList);
}

bool CppVisa::GetAddress(const char keyword[], char address[])
{
    std::string keyLower = ToLowerCopy(keyword);

    for (std::size_t i = 0; i < idns.size(); ++i) {
        if (idns[i].find(keyLower) != std::string::npos) {
            strncpy_s(address, kBufSize, addresses[i].c_str(), _TRUNCATE);
            return true;
        }
    }

    std::cerr << "[Error] 指定したキーワード 「" << keyLower << "」 を含む計測器が見つかりません。" << std::endl;
    return false;
}
