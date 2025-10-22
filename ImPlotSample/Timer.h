#pragma once

#include <thread>
#include <chrono>
#include <windows.h>
#include <mmsystem.h>

// timeBeginPeriod, timeEndPeriod�̂��߂�winmm.lib�������N����
#pragma comment(lib, "winmm.lib")

class Timer
{
public:
	// �R���X�g���N�^�F�^�C�}�[�����������A�ŏ���Timer�I�u�W�F�N�g���������ꂽ�ۂ�
	// �V�X�e���̃^�C�}�[����\��ݒ肵�܂��B
	Timer() {
		if (getNumTimers()++ == 0) {
			timeBeginPeriod(1);
		}
		start();
	}

	// �f�X�g���N�^�F�Ō��Timer�I�u�W�F�N�g���j�����ꂽ�ۂ�
	// �V�X�e���̃^�C�}�[����\�����ɖ߂��܂��B
	~Timer() {
		if (--getNumTimers() == 0) {
			timeEndPeriod(1);
		}
	}

	// �^�C�}�[�̊J�n�n�_�����Z�b�g���܂��B
	void start() {
		QueryPerformanceCounter(&qpc_start);
		qpc_previous = qpc_start;
	}

	// �w�肵���b���������s���ꎞ��~���܂��B
	static void sleepFor(const double seconds) {
		if (seconds <= 0.0) return;
		auto duration = std::chrono::nanoseconds(static_cast<long long>(seconds * 1e9));
		std::this_thread::sleep_for(duration);
	}

	// �^�C�}�[�̊J�n���_����w�肵���b���ɂȂ�܂Ŏ��s���ꎞ��~���܂��B
	// �ҋ@�������������_�ł̎��ۂ̍��v�o�ߎ��Ԃ�b�ŕԂ��܂��B
	double sleepUntil(const double theTimeSec) {
		const auto target_ticks = static_cast<LONGLONG>(theTimeSec * getQpf().QuadPart) + qpc_start.QuadPart;

		hybrid_sleep_until(target_ticks);

		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);
		return static_cast<double>(qpc_now.QuadPart - qpc_start.QuadPart) / getQpf().QuadPart;
	}

	// ���O�̎��Ԍv���n�_����w�肵���b���������s���ꎞ��~���܂��B
	// ���肵�������̃��[�v���쐬����̂ɕ֗��ł��B
	// �ҋ@�������������_�ł̎��ۂ̍��v�o�ߎ��Ԃ�b�ŕԂ��܂��B
	double sleepFromPreviousFor(const double seconds) {
		const auto target_ticks = static_cast<LONGLONG>(seconds * getQpf().QuadPart) + qpc_previous.QuadPart;

		hybrid_sleep_until(target_ticks);

		qpc_previous.QuadPart = target_ticks;

		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);
		return static_cast<double>(qpc_now.QuadPart - qpc_start.QuadPart) / getQpf().QuadPart;
	}

	// start()���Ă΂�Ă���̍��v�o�ߎ��Ԃ�b�ŕԂ��܂��B
	double elapsedSec() const {
		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);
		return static_cast<double>(qpc_now.QuadPart - qpc_start.QuadPart) / getQpf().QuadPart;
	}

	// ���̊֐����O��Ă΂�Ă���̌o�ߎ��Ԃ�b�ŕԂ��܂��B
	// ����̌Ăяo���̂��߂ɁA�����́u�O��v�̃^�C���|�C���g���X�V���܂��B
	double elapsedFromPreviousSec() {
		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);
		double result = static_cast<double>(qpc_now.QuadPart - qpc_previous.QuadPart) / getQpf().QuadPart;
		qpc_previous = qpc_now;
		return result;
	}

private:
	// --- �����o�ϐ� ---
	LARGE_INTEGER qpc_start{};
	LARGE_INTEGER qpc_previous{};

	// --- C++14�݊��̐ÓI�����o ---

	// �p�t�H�[�}���X�J�E���^�̎��g�����擾���܂� (C++14�w�b�_�[�I�����[�Ή�)
	static const LARGE_INTEGER& getQpf() {
		static const LARGE_INTEGER qpf = [] {
			LARGE_INTEGER f;
			QueryPerformanceFrequency(&f);
			return f;
			}();
		return qpf;
	}

	// �A�N�e�B�u��Timer�C���X�^���X�̐����Ǘ����܂� (C++14�w�b�_�[�I�����[�Ή�)
	static int& getNumTimers() {
		static int numTimers = 0;
		return numTimers;
	}

	// --- �v���C�x�[�g�w���p�[�֐� ---

	// �X���[�v�ƃX�s����g�ݍ��킹���n�C�u���b�h�헪�ŁA�ڕW�̃e�B�b�N�J�E���g�ɓ��B����܂Ō����I�ɑҋ@���܂��B
	void hybrid_sleep_until(LONGLONG target_ticks) {
		LARGE_INTEGER qpc_now;
		QueryPerformanceCounter(&qpc_now);

		const LONGLONG ticks_to_wait = target_ticks - qpc_now.QuadPart;
		if (ticks_to_wait <= 0) return;

		// �X���[�v���Ԍv�Z�̂��߁A�e�B�b�N���}�C�N���b�ɕϊ�
		const long long us_to_wait = ticks_to_wait * 1'000'000 / getQpf().QuadPart;

		// �ҋ@���Ԃ������ꍇ�A�唼���X���[�v���A�c����X�s��������
		// 臒l�Ƃ���2000us (2ms) ��I��
		constexpr long long spin_threshold_us = 2000;
		if (us_to_wait > spin_threshold_us) {
			// �ڕW���Ԃ̒��O�܂ŃX���[�v
			std::this_thread::sleep_for(std::chrono::microseconds(us_to_wait - spin_threshold_us));
		}

		// �Ō�̂킸���Ȏ��Ԃ��r�W�[���[�v�i�X�s���j�őҋ@���A�����x���m�ۂ���
		while (true) {
			QueryPerformanceCounter(&qpc_now);
			if (qpc_now.QuadPart >= target_ticks) {
				break;
			}
		}
	}
};