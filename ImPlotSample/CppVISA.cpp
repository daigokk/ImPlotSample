#include "CppVisa.h"

#pragma comment(lib, "./VISA/visa64.lib")

#include <cstdarg>   // va_list �Ȃ�
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
        printf("--- ����̃R�����g----------------------------------------------\n");
        switch (status)
        {
        case -1073807343: //viOpen error
            printf(" �v���킪������Ȃ��Ƃ����Ӗ��̃G���[�ł��B\n");
            printf(" VISA���\�[�X�����ԈႦ�Ă���Ƃ��̃G���[���o�܂��BNI MAX�̉�ʏ�ŕ\������郊�\�[�X��(USB0::????????::INSTR)���R�s�[���Ďg���܂��傤�B\n");
            printf(" �܂����A�v����̓d���������Ă��Ȃ��A�Ƃ������Ƃ͂���܂��񂩁H\n");
            printf(" �������́A�v����Ǝ������g���Ă���PC��USB�P�[�u���Őڑ�����Ă��邩�m�F���Ă݂܂��傤�B\n");
            printf(" �I�V���X�R�[�v(DLM2022)�̏ꍇ�AUSB�P�[�u����LAN�̃R�l�N�^�ɂ����Ă��܂��l���悭���܂��B\n");
            break;
        case -1073807346: // 
            printf(" �v����̃n���h�����擾�����ɒʐM���悤�Ƃ���Əo��G���[�ł��BviOpen�����s���Ă��Ȃ����A���s���Ă��܂��B\n");
            printf(" viOpen�Ŏw�肷��VISA���\�[�X��(USB0::????????::INSTR)���m�F���Ă��������BNI MAX�̉�ʏ�ŕ\������郊�\�[�X�����R�s�[���Ďg���܂��傤�B\n");
            break;
        case -1073807339: //Timeout
            printf(" viScanf�����s�����ۂɌv���킩�瓚�����A���ė��Ȃ��������ɏo��G���[�ł��B\n");
            printf(" ���O��viPrintf�ő������₢���킹�R�}���h(�Ō��?�����Ă���)���ԈႦ�Ă��邩�A���s�R�[�h(\\n)�𑗂��Ă��Ȃ��\��������܂��B\n");
            printf(" �₢���킹�R�}���h�𑗂��Ă��Ȃ��ꍇ�́AviScanf�̍s���폜���Ă��������B\n");
            printf(" �₢���킹�R�}���h�𑗂�K�v������ꍇ�́A���O��viPrintf�̍s���m�F���Ă��������B\n");
            break;
        default:
            printf(" ���������������A�������ԈႦ�Ă��܂��B\n");
            printf(" �܂��A�ǂ̍s�ŏo�Ă���G���[�Ȃ̂����m�F���Ă��������B\n");
            printf(" �\�[�X�R�[�h��10��ǂ�ł��킩��Ȃ���΁A������Ă�ł��������B�������ꏏ�ɍl���܂��傤�B\n");
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
        std::cerr << "[Error] VISA���\�[�X�}�l�[�W���[������������Ă��܂���BCppVisa::OpenRM�����s���Ă��������B\n";
    }
    ViSession instrument = VI_NULL;
    ViStatus status = viOpen(resourceManager, address, VI_NULL, VI_NULL, &instrument);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return VI_NULL;
    }
    // Timeout�����̐ݒ�i�~���b�P�ʁj
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
    std::cout << cmd; // �f�o�b�O�p�o��
    ViStatus status = viPrintf(instrument, cmd);

    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
    return true;
}

// cviScanf�ɂ�����format�ŗp���镶�����%t���g���B%s���g���Ƌ󔒂ŋ�؂�ꂽ�ŏ��̒P�ꂵ���ǂݎ��Ȃ��B
bool CppVisa::Scanf(const ViSession instrument, const char* filename, int line, const char* format, void* output) {
    ViStatus status = viScanf(instrument, format, output);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
    std::cout << (char*)output; // �f�o�b�O�p�o��
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
        std::cerr << "�v����̃I�[�v���Ɏ��s���܂����B" << std::endl;
        return;
    }
    status = viQueryf(instrument, "%s", "%255t", "*IDN?\n", ret);
    if (status < VI_SUCCESS) {
        std::cerr << "�v����̖₢���킹�Ɏ��s���܂����B" << std::endl;
        return;
    }
    status = viClose(instrument);
    if (status < VI_SUCCESS) {
        std::cerr << "�v����̃N���[�Y�Ɏ��s���܂����B" << std::endl;
        return;
    }
}

void CppVisa::FindRsrc() {
    // �ڑ�����Ă���v����������i��: GPIB, USB, TCPIP�Ȃǁj
    ViStatus status;
    ViFindList findList = 0;
    ViUInt32 numInstrs;
    ViChar instrDesc[256], ret[256] = { 0 };
    status = viFindRsrc(resourceManager, "?*INSTR", &findList, &numInstrs, instrDesc);
    if (status < VI_SUCCESS) {
        std::cerr << "�v����̌����Ɏ��s���܂����B" << std::endl;
        return;
    }
    std::cout << "���������v����̐�: " << numInstrs << std::endl;
    if (numInstrs == 0) {
        std::cout << "�v���킪������܂���ł����B" << std::endl;
        return;
    }
    // �ŏ��̌v������擾
    GetIdn(instrDesc, ret);
    std::cout << "1: " << instrDesc << ", " << ret << std::endl;

    // �c��̌v������擾
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
