#pragma once

#pragma comment(lib, "./VISA/visa64.lib")
#include <visa.h>

// �G���[�`�F�b�N�p���[�e�B���e�B�֐�
void vi_checkError(ViStatus status, const char* filename, int line);

// VISA���\�[�X����
void vi_FindRsrc(const ViSession resourceManager);

// �g�`�ۑ��i�ψ����j
int scope_save_Waveformsf(const ViSession instrument, const char* format, ...);

class CVisa {
private:
    static ViSession resourceManager;

public:
    // ���\�[�X�}�l�[�W���̏�����
    static bool OpenRM(const char* filename, const int line);

    // ���\�[�X�}�l�[�W���̏I������
    static void CloseRM();

    // �v����̃I�[�v��
    static ViSession OpenInstrument(const char* address, const char* filename, const int line);

    // �v����̃N���[�Y
    static void CloseInstrument(ViSession instrument);

	// �v����փR�}���h���M�i�ψ����j�K�����s�R�[�h(\n)���܂߂邱�ƁB
    static bool cviPrintf(const ViSession instrument, const char* filename, const int line, const char* format, ...);

    // �v���킩��̉�����M
    // ���ӁFformat�ɂ� %t ���g�p�B%s �͋󔒋�؂�̍ŏ��̒P�ꂵ���擾�ł��Ȃ��B
    static bool cviScanf(const ViSession instrument, const char* filename, const int line, const char* format, void* output);

    // �N�G�����M�Ɖ����擾�i�ψ����j
    static char* cviQueryf(const ViSession instrument, const char* filename, const int line, const char* format, ...);
};