#pragma once

#include <visa.h>


class CppVisa {
public:
    static ViSession resourceManager;
    // ���\�[�X�}�l�[�W���̏�����
    static bool OpenRM(const char* filename, const int line);

    // ���\�[�X�}�l�[�W���̏I������
    static void CloseRM();

    // �v����̃I�[�v��
    static ViSession OpenInstrument(const char address[], const char* filename, const int line);

    // �v����̃N���[�Y
    static void CloseInstrument(ViSession instrument);

	// �v����փR�}���h���M�i�ψ����j�K�����s�R�[�h(\n)���܂߂邱�ƁB
    static bool Printf(const ViSession instrument, const char* filename, const int line, const char* format, ...);

    // �v���킩��̉�����M
    // ���ӁFformat�ɂ� %t ���g�p�B%s �͋󔒋�؂�̍ŏ��̒P�ꂵ���擾�ł��Ȃ��B
    static bool Scanf(const ViSession instrument, const char* filename, const int line, const char* format, void* output);

    // �N�G�����M�Ɖ����擾�i�ψ����j
    static char* Queryf(const ViSession instrument, const char* filename, const int line, const char* format, ...);

    static void GetIdn(const ViChar* instrDesc, char* ret);

	static void FindRsrc();
};
