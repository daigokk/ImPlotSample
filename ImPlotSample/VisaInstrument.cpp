#pragma comment(lib, "./VISA/visa64.lib")

#include "VisaInstrument.hpp"

// .cpp ファイル内でのみ必要な C++ 標準ライブラリ
#include <vector>
#include <map>
#include <mutex>        // std::mutex, std::lock_guard, std::weak_ptr
#include <iostream>     // デバッグ・ログ用 (std::cout, std::cerr)
#include <thread>       // スキャン時のタイムアウト用 (オプション)
#include <chrono>       // スキャン時のタイムアウト用 (オプション)

// --- エラーチェック ヘルパー関数 (static でこのファイル内のみ) ---

/**
 * @brief VISAステータスをチェックし、エラーなら VisaException をスローする
 */
static void checkStatus(ViStatus status, ViSession session, const std::string& context_message) {
    if (status < VI_SUCCESS) {
        // VI_ERROR_TMO (タイムアウト) なども全て例外として処理する
        std::cerr << "--- 小坂のコメント----------------------------------------------\n";
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
        std::cerr << "----------------------------------------------------------------\n";
        throw VisaException(context_message, status);
    }
}


// --- VisaException 実装 ---

/**
 * @brief エラーメッセージ構築用の補助関数
 */
static std::string buildErrorMessage(const std::string& message, ViStatus status) {
    char error_buffer[VI_FIND_BUFLEN];
    // viStatusDesc は RM セッションがなくても呼べる
    viStatusDesc(VI_NULL, status, error_buffer);

    // std::to_string は C++11 から
    return message + " (VISA Error: " + std::string(error_buffer) +
        " [Code: " + std::to_string(status) + "])";
}

VisaException::VisaException(const std::string& message, ViStatus status)
// std::runtime_error のコンストラクタを正しく初期化
    : std::runtime_error(buildErrorMessage(message, status)), m_status(status)
{
    // 本体は空
}

ViStatus VisaException::getStatus() const {
    return m_status;
}


// #########################################################################
// ###
// ### 内部実装 (VisaResourceManager)
// ### (この .cpp ファイル内でのみ定義・使用)
// ###
// #########################################################################

/**
 * @brief (内部) VISAリソースマネージャ (RM)
 *
 * シングルトンのような形で共有され、全計測器のIDNをキャッシュする責務を持つ。
 * VisaInstrument クラスによって内部的に管理される。
 */
class VisaResourceManager {
public:
    /**
     * @brief 共有インスタンスを取得するファクトリメソッド
     */
    static std::shared_ptr<VisaResourceManager> getInstance() {
        // ダブルチェックロッキング (DCL) よりも C++11 以降の
        // 静的ローカル変数のスレッドセーフな初期化を利用するのが簡単だが、
        // ここでは std::weak_ptr を使った古典的なシングルトン実装例を示す。

        std::lock_guard<std::mutex> lock(m_instance_mutex);

        // 既存のインスタンスが有効かチェック
        auto shared_instance = m_weak_instance.lock();

        if (!shared_instance) {
            // 新しく作成
            // std::make_shared は private コンストラクタを呼べないので new を使う
            shared_instance = std::shared_ptr<VisaResourceManager>(new VisaResourceManager());

            // 弱参照ポインタに保持
            m_weak_instance = shared_instance;
        }
        return shared_instance;
    }

    /**
     * @brief デストラクタ (RMセッションを閉じる)
     */
    ~VisaResourceManager() {
        if (m_rm_session != VI_NULL) {
            // リソースマネージャのセッションを閉じる
            viClose(m_rm_session);
            m_rm_session = VI_NULL;
        }
        std::cout << "[Debug] VisaResourceManager closed." << std::endl;
    }

    // コピーとムーブを禁止
    VisaResourceManager(const VisaResourceManager&) = delete;
    VisaResourceManager& operator=(const VisaResourceManager&) = delete;
    VisaResourceManager(VisaResourceManager&&) = delete;
    VisaResourceManager& operator=(VisaResourceManager&&) = delete;

    /**
     * @brief IDNキャッシュから、指定文字列を含む計測器のVISAアドレスを検索
     */
    std::string findInstrumentByIdn(const std::string& idn_substring) {
        // キャッシュ読み取り中もロック（スキャン中に呼ばれる可能性は低いが念のため）
        std::lock_guard<std::mutex> lock(m_cache_mutex);

        for (const auto& pair : m_idn_cache) {
            // pair.first = VISA Address
            // pair.second = IDN Response

            // IDN応答 (pair.second) に idn_substring が含まれるか検索
            if (pair.second.find(idn_substring) != std::string::npos) {
                return pair.first; // VISAアドレスを返す
            }
        }

        // 見つからなかった場合
        throw VisaException("Instrument with IDN containing '" + idn_substring + "' not found in cache.", VI_ERROR_RSRC_NFOUND);
    }

    /**
     * @brief 指定されたVISAアドレスで計測器セッションを開く
     * (VisaInstrument クラスから内部的に呼び出される)
     */
    ViSession openSession(const std::string& visa_address) {
        ViSession session = VI_NULL;
        // タイムアウトを適度に設定 (e.g., 5000ms)
        ViStatus status = viOpen(m_rm_session, (ViRsrc)visa_address.c_str(),
            VI_NULL, 5000, &session);

        checkStatus(status, m_rm_session, "Failed to open session for " + visa_address);

        // セッションのデフォルト設定
        // タイムアウト (e.g., 100ms)
        viSetAttribute(session, VI_ATTR_TMO_VALUE, 100);
        // 終端文字を有効化 (viReadで \n を検知するため)
        viSetAttribute(session, VI_ATTR_TERMCHAR_EN, VI_TRUE);
        viSetAttribute(session, VI_ATTR_TERMCHAR, '\n'); // 終端文字はLF (0x0A)

        return session;
    }

private:
    /**
     * @brief privateコンストラクタ (getInstance()経由でのみ生成)
     */
    VisaResourceManager() {
        std::cout << "[Debug] Initializing VisaResourceManager..." << std::endl;

        // 1. デフォルトリソースマネージャを開く
        ViStatus status = viOpenDefaultRM(&m_rm_session);
        if (status < VI_SUCCESS) {
            // RMが開けない場合は致命的
            throw VisaException("Failed to open Default Resource Manager", status);
        }

        // 2. RMが開いたら、即座に全計測器をスキャン
        try {
            scanInstruments();
        }
        catch (...) {
            // スキャン中に例外が発生しても、RMは閉じる必要がある
            viClose(m_rm_session);
            throw; // 例外を再スロー
        }
        std::cout << "[Debug] VisaResourceManager initialized. IDN cache built." << std::endl;
    }

    /**
     * @brief 接続されている全計測器のIDNをスキャンし、キャッシュする
     */
    void scanInstruments() {
        // キャッシュ操作中は排他ロック
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        m_idn_cache.clear();

        ViFindList find_list = VI_NULL;
        ViUInt32 count = 0;
        char visa_address_buffer[VI_FIND_BUFLEN];

        // "?*INSTR" は全ての計測器リソースを検索
        ViStatus status = viFindRsrc(m_rm_session, "?*INSTR", &find_list, &count, visa_address_buffer);

        if (status < VI_SUCCESS || count == 0) {
            if (find_list != VI_NULL) viClose(find_list);
            std::cerr << "Warning: No instruments found during scan." << std::endl;
            return;
        }

        std::vector<std::string> addresses;
        // 最初の1台
        addresses.push_back(std::string(visa_address_buffer));

        // 2台目以降
        for (ViUInt32 i = 1; i < count; ++i) {
            status = viFindNext(find_list, visa_address_buffer);
            if (status < VI_SUCCESS) continue;
            addresses.push_back(std::string(visa_address_buffer));
        }
        viClose(find_list); // FindListをクローズ

        std::cout << "[Debug] Found " << addresses.size() << " instruments. Scanning *IDN?..." << std::endl;

        // 見つかった各アドレスに対して *IDN? を実行
        for (const auto& address : addresses) {
            ViSession session = VI_NULL;

            // タイムアウトを短く設定 (e.g., 2000ms)
            // 接続できない（使用中の）計測器でブロックしないように
            status = viOpen(m_rm_session, (ViRsrc)address.c_str(), VI_NULL, 2000, &session);

            if (status < VI_SUCCESS) {
                // 開けない計測器はスキップ (e.g., 他のプロセスが使用中)
                std::cerr << "Warning: Failed to open " << address << " (skipping)." << std::endl;
                continue;
            }

            std::string idn_response = "[Error: No IDN response]";
            try {
                // viQueryf は Write と Read を行う
                // %t は \n 終端の文字列を読み取る
                char idn_buffer[VI_FIND_BUFLEN];
                viSetAttribute(session, VI_ATTR_TMO_VALUE, 2000); // タイムアウト設定
                status = viQueryf(session, (ViString)"*IDN?\n", (ViString)"%t", idn_buffer);

                if (status >= VI_SUCCESS) {
                    idn_response = std::string(idn_buffer);
                }
            }
            catch (...) {
                // viQueryf などで例外が発生した場合 (この設計ではVisaException)
                // スキャン中のエラーはログに残して続行
                std::cerr << "Warning: Failed to query *IDN? from " << address << std::endl;
            }

            viClose(session); // IDN取得後、セッションはすぐ閉じる

            // キャッシュに保存
            m_idn_cache[address] = idn_response;
            std::cout << "  [Debug] Cached: " << address << " -> " << idn_response << std::endl;
        }
    }

    ViSession m_rm_session = VI_NULL;

    // IDNキャッシュ (Key: VISA Address, Value: IDN Response)
    std::map<std::string, std::string> m_idn_cache;

    // キャッシュアクセス用のMutex
    std::mutex m_cache_mutex;

    // シングルトンインスタンス管理用
    static std::weak_ptr<VisaResourceManager> m_weak_instance;
    static std::mutex m_instance_mutex;
};

// --- VisaResourceManager 静的メンバー初期化 ---
std::weak_ptr<VisaResourceManager> VisaResourceManager::m_weak_instance;
std::mutex VisaResourceManager::m_instance_mutex;


// #########################################################################
// ###
// ### 公開クラス (VisaInstrument) の実装
// ###
// #########################################################################

/**
 * @brief 静的ファクトリ関数の実装
 */
std::unique_ptr<VisaInstrument> VisaInstrument::create(const std::string& resource_string) {
    // std::make_unique は C++14 ですが、private コンストラクタを呼び出すことはできません。
    // 代わりに、std::unique_ptr のコンストラクタに直接 new を渡します。
    // (C++11 と C++14 の両方で動作します)
    return std::unique_ptr<VisaInstrument>(new VisaInstrument(resource_string));
}

// IDN文字列から検索するコンストラクタ
// VISAアドレスを直接指定するコンストラクタ
VisaInstrument::VisaInstrument(const std::string& resource_string)
    : m_resource_manager(VisaResourceManager::getInstance())
{
    if (!m_resource_manager) {
        throw VisaException("Failed to get VisaResourceManager instance.", VI_ERROR_SYSTEM_ERROR);
    }

    std::string visa_address_to_open;

    if (resource_string.find("::") != std::string::npos) {
        std::cout << "[Debug] '" << resource_string << "' contains '::'. Treating as direct address." << std::endl;
        visa_address_to_open = resource_string;
    }
    else {
        std::cout << "[Debug] '" << resource_string << "' does not contain '::'. Searching IDN cache." << std::endl;
        visa_address_to_open = m_resource_manager->findInstrumentByIdn(resource_string);
        std::cout << "[Debug] Found " << resource_string << " at " << visa_address_to_open << std::endl;
    }

    m_session = m_resource_manager->openSession(visa_address_to_open);
    viClear(m_session);
}

/**
 * @brief デストラクタ (実装)
 *
 * この時点で m_resource_manager (shared_ptr) が破棄される。
 * この場所 (.cpp) は VisaResourceManager の完全な定義を知っているため、
 * shared_ptr は安全に VisaResourceManager のデストラクタを呼び出せる。
 * (最後の VisaInstrument が破棄されたタイミングで、RM のデストラクタも呼ばれる)
 */
VisaInstrument::~VisaInstrument() {
    if (m_session != VI_NULL) {
        // 計測器セッションを閉じる
        viClose(m_session);
        m_session = VI_NULL;
    }
    // このスコープを抜けるときに m_resource_manager (shared_ptr) がデクリメントされる
}

// ムーブコンストラクタ (実装)
VisaInstrument::VisaInstrument(VisaInstrument&& other) noexcept
    : m_session(other.m_session),
    m_resource_manager(std::move(other.m_resource_manager)) {
    // 元のオブジェクトのリソース所有権を放棄させる
    other.m_session = VI_NULL;
}

// ムーブ代入演算子 (実装)
VisaInstrument& VisaInstrument::operator=(VisaInstrument&& other) noexcept {
    if (this != &other) {
        // 既存のリソースを解放 (デストラクタのロジック)
        if (m_session != VI_NULL) {
            viClose(m_session);
        }

        // リソースをムーブ
        m_session = other.m_session;
        m_resource_manager = std::move(other.m_resource_manager);

        // 元のオブジェクトのリソース所有権を放棄
        other.m_session = VI_NULL;
    }
    return *this;
}

// --- write, read, query の実装 ---

void VisaInstrument::write(const std::string& command) {
    // VISAコマンドは通常 \n (LF) が必要
    std::string cmd_with_newline = command + "\n";
    ViUInt32 bytes_written = 0;

    ViStatus status = viWrite(m_session, (ViBuf)cmd_with_newline.c_str(),
        (ViUInt32)cmd_with_newline.length(), &bytes_written);

    checkStatus(status, m_session, "Failed to write command: " + command);
}

std::string VisaInstrument::read(size_t buffer_size) {
    // std::vector を使うと自動的にメモリが管理される
    std::vector<char> buffer(buffer_size);
    ViUInt32 bytes_read = 0;

    ViStatus status = viRead(m_session, (ViBuf)buffer.data(), (ViUInt32)buffer.size(), &bytes_read);

    // viRead は成功時 (VI_SUCCESS_TERM_CHAR) でも VI_SUCCESS (0) より大きい値を返す
    // checkStatus は < VI_SUCCESS (負の値) のみエラーとするため、正常に動作する
    checkStatus(status, m_session, "Failed to read data");

    // viReadで読み取った文字列から \n や \r を取り除く
    std::string result(buffer.data(), bytes_read);

    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }

    return result;
}

std::string VisaInstrument::query(const std::string& command, size_t buffer_size) {
    // 1. 書き込み
    write(command);

    // 2. 読み取り
    // (write と read の間に sleep を入れる必要は、通常 viRead のタイムアウトで
    //  カバーされるため不要だが、計測器によっては必要な場合がある)

    return read(buffer_size);
}

ViSession VisaInstrument::getSession() const {
    return m_session;
}