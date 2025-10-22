#pragma once

#include <thread>
#include <chrono>
#include <windows.h>
#include <mmsystem.h>

// timeBeginPeriod, timeEndPeriodのためにwinmm.libをリンクする
#pragma comment(lib, "winmm.lib")

class Timer
{
public:
	// コンストラクタ：タイマーを初期化し、最初のTimerオブジェクトが生成された際に
	// システムのタイマー分解能を設定します。
	Timer() {
		if (getNumTimers()++ == 0) {
			timeBeginPeriod(1);
		}
		start();
	}

	// デストラクタ：最後のTimerオブジェクトが破棄された際に
	// システムのタイマー分解能を元に戻します。
	~Timer() {
		if (--getNumTimers() == 0) {
			timeEndPeriod(1);
		}
	}

	// タイマーの開始地点をリセットします。
	void start() {
		QueryPerformanceCounter(&qpc_start);
		qpc_previous = qpc_start;
	}

	// 指定した秒数だけ実行を一時停止します。
	static void sleepFor(const double seconds) {
		if (seconds <= 0.0) return;
		auto duration = std::chrono::nanoseconds(static_cast<long long>(seconds * 1e9));
		std::this_thread::sleep_for(duration);
	}

	// タイマーの開始時点から指定した秒数になるまで実行を一時停止します。
	// 待機が完了した時点での実際の合計経過時間を秒で返します。
	double sleepUntil(const double theTimeSec) {
		const auto target_ticks = static_cast<LONGLONG>(theTimeSec * getQpf().QuadPart) + qpc_start.QuadPart;

		hybrid_sleep_until(target_ticks);

		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);
		return static_cast<double>(qpc_now.QuadPart - qpc_start.QuadPart) / getQpf().QuadPart;
	}

	// 直前の時間計測地点から指定した秒数だけ実行を一時停止します。
	// 安定した周期のループを作成するのに便利です。
	// 待機が完了した時点での実際の合計経過時間を秒で返します。
	double sleepFromPreviousFor(const double seconds) {
		const auto target_ticks = static_cast<LONGLONG>(seconds * getQpf().QuadPart) + qpc_previous.QuadPart;

		hybrid_sleep_until(target_ticks);

		qpc_previous.QuadPart = target_ticks;

		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);
		return static_cast<double>(qpc_now.QuadPart - qpc_start.QuadPart) / getQpf().QuadPart;
	}

	// start()が呼ばれてからの合計経過時間を秒で返します。
	double elapsedSec() const {
		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);
		return static_cast<double>(qpc_now.QuadPart - qpc_start.QuadPart) / getQpf().QuadPart;
	}

	// この関数が前回呼ばれてからの経過時間を秒で返します。
	// 次回の呼び出しのために、内部の「前回」のタイムポイントを更新します。
	double elapsedFromPreviousSec() {
		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);
		double result = static_cast<double>(qpc_now.QuadPart - qpc_previous.QuadPart) / getQpf().QuadPart;
		qpc_previous = qpc_now;
		return result;
	}

private:
	// --- メンバ変数 ---
	LARGE_INTEGER qpc_start{};
	LARGE_INTEGER qpc_previous{};

	// --- C++14互換の静的メンバ ---

	// パフォーマンスカウンタの周波数を取得します (C++14ヘッダーオンリー対応)
	static const LARGE_INTEGER& getQpf() {
		static const LARGE_INTEGER qpf = [] {
			LARGE_INTEGER f;
			QueryPerformanceFrequency(&f);
			return f;
			}();
		return qpf;
	}

	// アクティブなTimerインスタンスの数を管理します (C++14ヘッダーオンリー対応)
	static int& getNumTimers() {
		static int numTimers = 0;
		return numTimers;
	}

	// --- プライベートヘルパー関数 ---

	// スリープとスピンを組み合わせたハイブリッド戦略で、目標のティックカウントに到達するまで効率的に待機します。
	void hybrid_sleep_until(LONGLONG target_ticks) {
		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);

		const LONGLONG ticks_to_wait = target_ticks - qpc_now.QuadPart;
		if (ticks_to_wait <= 0) return;

		// スリープ期間計算のため、ティックをマイクロ秒に変換
		const long long us_to_wait = ticks_to_wait * 1'000'000 / getQpf().QuadPart;

		// 待機時間が長い場合、大半をスリープし、残りをスピンさせる
		// 閾値として2000us (2ms) を選択
		constexpr long long spin_threshold_us = 2000;
		if (us_to_wait > spin_threshold_us) {
			// 目標時間の直前までスリープ
			std::this_thread::sleep_for(std::chrono::microseconds(us_to_wait - spin_threshold_us));
		}

		// 最後のわずかな時間をビジーループ（スピン）で待機し、高精度を確保する
		while (true) {
			QueryPerformanceCounter(&qpc_now);
			if (qpc_now.QuadPart >= target_ticks) {
				break;
			}
		}
	}
};