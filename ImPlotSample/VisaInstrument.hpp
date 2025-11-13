#pragma once

// NI-VISAのCヘッダーをインクルード
#include "visa.h" 

#include <string>
#include <memory>       // std::shared_ptr
#include <stdexcept>    // std::runtime_error

/**
 * @brief VISAエラー用のカスタム例外クラス
 */
class VisaException : public std::runtime_error {
public:
    /**
     * @param message エラーコンテキスト
     * @param status vi... 関数から返された ViStatus コード
     */
    VisaException(const std::string& message, ViStatus status);

    /**
     * @brief VISAステータスコードを取得
     */
    ViStatus getStatus() const;

private:
    ViStatus m_status;
};


// --- PIMPL イディオムのための前方宣言 ---

/**
 * @brief (内部実装) VISAリソースマネージャ
 * * このクラスの詳細は .cpp ファイル内に隠蔽されます。
 */
class VisaResourceManager;


/**
 * @brief 個別のVISA計測器ラッパークラス (RAII対応)
 *
 * ユーザーはこのクラスのみを使用します。
 * 必要なリソースマネージャは内部で自動的に管理されます。
 * * @note
 * 最初の VisaInstrument インスタンス化時に、接続されている全計測器の
 * *IDN? スキャンが自動的に実行されます。
 */
class VisaInstrument {
public:
    /**
    * @brief VisaInstrument のインスタンスを作成するファクトリ関数
    *
    * 渡された文字列 (resource_string) に "::" が含まれているか判断し、
    * IDN検索またはアドレス直指定でセッションを開きます。
    *
    * @param resource_string "34401A" (IDN) または "GPIB0::22::INSTR" (Address)
    * @return std::unique_ptr<VisaInstrument> インスタンスへのユニークポインタ
    * @throws VisaException 計測器が見つからない場合、またはオープンに失敗した場合
    */
    static std::unique_ptr<VisaInstrument> create(const std::string& resource_string);

     /**
      * @brief デストラクタ (RAII)
      *
      * 計測器セッション (viClose(m_session)) を自動的に閉じます。
      * PIMPLイディオムのため、実装は .cpp ファイル側にあります。
      */
     ~VisaInstrument();

     // コピーは禁止 (セッションハンドルはユニークであるべき)
     VisaInstrument(const VisaInstrument&) = delete;
     VisaInstrument& operator=(const VisaInstrument&) = delete;

     /**
      * @brief ムーブコンストラクタ (C++14)
      *
      * リソース（セッションハンドル）の所有権を移動させます。
      * PIMPLイディオムのため、実装は .cpp ファイル側にあります。
      */
     VisaInstrument(VisaInstrument&& other) noexcept;

     /**
      * @brief ムーブ代入演算子 (C++14)
      *
      * リソース（セッションハンドル）の所有権を移動させます。
      * PIMPLイディオムのため、実装は .cpp ファイル側にあります。
      */
     VisaInstrument& operator=(VisaInstrument&& other) noexcept;

     // --- 計測器操作API ---

     /**
      * @brief 計測器にコマンドを書き込む (自動で末尾に \n を付加)
      * @param command SCPIコマンド (e.g., "*RST", "MEAS:VOLT:DC?")
      */
     void write(const std::string& command);

     /**
      * @brief 計測器からデータを読み込む (終端文字まで)
      * @param buffer_size 読み取りバッファの最大サイズ
      * @return 読み取った文字列 (末尾の \n, \r は自動的に削除)
      */
     std::string read(size_t buffer_size = 2048);

     /**
      * @brief コマンドを書き込み、応答を読み取る (Query)
      * @param command 書き込むコマンド (e.g., "*IDN?")
      * @param buffer_size 読み取りバッファの最大サイズ
      * @return 計測器からの応答文字列 (末尾の \n, \r は自動的に削除)
      */
     std::string query(const std::string& command, size_t buffer_size = 2048);

     /**
      * @brief 生のセッションハンドルを取得 (高度な操作用)
      * @return ViSession ハンドル
      */
     ViSession getSession() const;

private:
    /**
     * @brief (privateコンストラクタ)
     * create() ファクトリ関数からのみ呼び出される
     */
    explicit VisaInstrument(const std::string& resource_string);
    ViSession m_session = VI_NULL;
    std::shared_ptr<VisaResourceManager> m_resource_manager;
};