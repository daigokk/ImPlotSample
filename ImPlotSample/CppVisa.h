#pragma once

#pragma comment(lib, "./VISA/visa64.lib")
#include <visa.h>
#include <cstdarg>   // va_list �Ȃ�
#include <iostream>

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

void vi_getIdn(const ViSession resourceManager, const ViChar* instrDesc, char* ret) {
    ViSession instrument;
    ViStatus status;
    status = viOpen(resourceManager, instrDesc, VI_NULL, VI_NULL, &instrument);
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

void vi_FindRsrc(const ViSession resourceManager) {
    // �ڑ�����Ă���v����������i��: GPIB, USB, TCPIP�Ȃǁj
    ViStatus status;
    ViFindList findList;
    ViUInt32 numInstrs;
    ViChar instrDesc[256], ret[256];
    status = viFindRsrc(resourceManager, "?*INSTR", &findList, &numInstrs, instrDesc);
    if (status < VI_SUCCESS) {
        std::cerr << "�v����̌����Ɏ��s���܂����B" << std::endl;
        return;
    }
    std::cout << "���������v����̐�: " << numInstrs << std::endl;
    if(numInstrs == 0) {
        std::cout << "�v���킪������܂���ł����B" << std::endl;
        return;
	}
	// �ŏ��̌v������擾
    vi_getIdn(resourceManager, instrDesc, ret);
    std::cout << "1: " << instrDesc << ", " << ret << std::endl;

    // �c��̌v������擾
    for (ViUInt32 i = 1; i < numInstrs; ++i) {
        status = viFindNext(findList, instrDesc);
        if (status < VI_SUCCESS) break;
        vi_getIdn(resourceManager, instrDesc, ret);
        std::cout << i + 1 << ": " << instrDesc << ", " << ret << std::endl;
    }

    viClose(findList);
}


class CppVisa {
public:
    static ViSession resourceManager;
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

ViSession CppVisa::resourceManager;

bool CppVisa::OpenRM(const char* filename, const int line) {
    ViStatus status = viOpenDefaultRM(&resourceManager);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
    return true;
}

ViSession CppVisa::OpenInstrument(const char address[], const char* filename, const int line) {
    ViSession instrument = VI_NULL;
    ViStatus status = viOpen(resourceManager, address, VI_NULL, VI_NULL, &instrument);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return VI_NULL;
    }
    // Timeout�����̐ݒ�i�~���b�P�ʁj
    status = viSetAttribute(instrument, VI_ATTR_TMO_VALUE, 2000);
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

bool CppVisa::cviPrintf(const ViSession instrument, const char* filename, const int line, const char* format, ...) {
	char cmd[256];
    va_list args;
    va_start(args, format);
	vsprintf(cmd, format, args);
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
bool CppVisa::cviScanf(const ViSession instrument, const char* filename, int line, const char* format, void* output) {
    ViStatus status = viScanf(instrument, format, output);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
	std::cout << (char*)output; // �f�o�b�O�p�o��
    return true;
}

char* CppVisa::cviQueryf(const ViSession instrument, const char* filename, const int line, const char* format, ...) {
    char ret[256];
    va_list args;

    va_start(args, format);
    vsprintf(ret, format, args);
    va_end(args);
    ViStatus status = cviPrintf(instrument,filename, line,  ret);

    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return nullptr;
    }

    status = cviScanf(instrument, filename, line, "%255t", ret);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return nullptr;
    }

    return ret;
}


char visa_viGetchar(ViSession vi)
{
    char c;
    vi_checkError(viScanf(vi, "%c", &c), __FILE__, __LINE__);
    return c;
}

// DLM2022��p�֐��Q
// ���R�[�h�����擾����
int scope_get_RecordLength(ViSession vi)
{
    char ret[256];
    vi_checkError(viQueryf(vi, "%s", "%255t", ":ACQuire:RLENgth?\n", ret), __FILE__, __LINE__);
    return atoi(ret);
}

// �`�����l��ch��\������
void scope_set_ViewCh(ViSession vi, int ch)
{
    vi_checkError(viPrintf(vi, "CHANnel%d:DISPlay ON\n"), __FILE__, __LINE__);
}

// �I�V���X�R�[�v�̓����Ԃ��擾����B1:RUN, 0:STOP
int scope_get_State(ViSession vi)
{
    char ret[256];
    vi_checkError(viQueryf(vi, "%s", "%255t", ":STATus:CONDition?\n", ret), __FILE__, __LINE__);
    if (atoi(ret) % 2 == 1)
        return 1;	//RUN
    else
        return 0;	//STOP
}

// �I�V���X�R�[�v���~����
void scope_set_Stop(ViSession vi)
{
    vi_checkError(viPrintf(vi, "STOP\n"), __FILE__, __LINE__);
}

// �I�V���X�R�[�v�𓮍삳����
void scope_set_Run(ViSession vi)
{
    vi_checkError(viPrintf(vi, "STARt\n"), __FILE__, __LINE__);
}

// �`�����l��ch�̐����ڐ����ݒ�E�擾����
void scope_set_Voltdir(ViSession vi, int ch, double volt)
{
    vi_checkError(viPrintf(vi, "CHANnel%d:VDIV %f\n", ch, volt), __FILE__, __LINE__);
}

double scope_get_Voltdir(ViSession vi, int ch)
{
    char ret[256];
    vi_checkError(viQueryf(vi, "%s", "%255t", "CHANnel%d:VDIV?\n", ch, ret), __FILE__, __LINE__);
    return atof(ret);
}

// ���Ԏ��̎��Ԗڐ����ݒ�E�擾����
void scope_set_timediv(ViSession vi, double sec)
{
    vi_checkError(viPrintf(vi, "TIMebase:TDIV %f\n", sec), __FILE__, __LINE__);
}

// ���Ԏ��̎��Ԗڐ�����擾����
double scope_get_timediv(ViSession vi)
{
    char ret[256];
    vi_checkError(viQueryf(vi, "%s", "%255t", "TIMebase:TDIV?\n", ret), __FILE__, __LINE__);
    return atof(ret);
}

// ���Ԏ��f�[�^��times�z��Ɏ擾����
void scope_get_times(const ViSession vi, double times[]) {
    int length = scope_get_RecordLength(vi);
    double dt = scope_get_timediv(vi) * 10 / length;
    for (int i = 0; i < length; ++i) {
        times[i] = (i + 1) * dt;
    }
}

// �`�����l��ch�̔g�`�f�[�^��voltages�z��Ɏ擾����
void scope_get_Waveform(const ViSession vi, int ch, double voltages[]) {
    vi_checkError(viPrintf(vi, "COMMunicate:HEADer OFF\n"), __FILE__, __LINE__);

    int length = scope_get_RecordLength(vi);
    scope_set_ViewCh(vi, ch);

    vi_checkError(viPrintf(vi, "WAVeform:STARt 0\n"), __FILE__, __LINE__);
    vi_checkError(viPrintf(vi, "WAVeform:END %d\n", length - 1), __FILE__, __LINE__);
    vi_checkError(viPrintf(vi, "WAVeform:TRACe %d\n", ch), __FILE__, __LINE__);
    vi_checkError(viPrintf(vi, "WAVeform:FORMat RBYTe\n"), __FILE__, __LINE__);

    double position = atof(CppVisa::cviQueryf(vi, __FILE__, __LINE__, "WAVeform:POSition?\n"));
    double range = atof(CppVisa::cviQueryf(vi, __FILE__, __LINE__, "WAVeform:RANGe?\n"));
    double offset = atof(CppVisa::cviQueryf(vi, __FILE__, __LINE__, "WAVeform:OFFSet?\n"));

    vi_checkError(viPrintf(vi, "WAVeform:SEND?\n"), __FILE__, __LINE__);

    char c = visa_viGetchar(vi);
    if (c != '#') exit(-1);

    int count = visa_viGetchar(vi) - '0';
    for (int i = 0; i < count; ++i) {
        visa_viGetchar(vi); // skip header digits
    }

    for (int i = 0; i < length; ++i) {
        c = visa_viGetchar(vi);
        voltages[i] = range * ((unsigned char)c - position) / 25.0 + offset;
    }
}

// �g�`�f�[�^���t�@�C���ɕۑ�����
int scope_save_Waveformsf(const ViSession vi, const char* format, ...) {
    char filename[256];
    va_list ap;
    va_start(ap, format);
    vsnprintf(filename, sizeof(filename) - 1, format, ap);
    va_end(ap);

    int length = scope_get_RecordLength(vi);
    double dt = scope_get_timediv(vi) * 10 / length;

    double* v1 = new double[length];
    double* v2 = new double[length];

    int wasRunning = scope_get_State(vi);
    scope_set_Stop(vi);
    scope_get_Waveform(vi, 1, v1);
    scope_get_Waveform(vi, 2, v2);
    if (wasRunning) scope_set_Run(vi);

    FILE* fp;
    fopen_s(&fp, filename, "w");
    if (!fp) {
        std::cerr << "Failed to open file: " << filename << "\n";
        delete[] v1;
        delete[] v2;
        return -1;
    }

    for (int i = 0; i < length; ++i) {
        fprintf(fp, "%e, %e, %e\n", (i + 1) * dt, v1[i], v2[i]);
    }

    fclose(fp);
    std::cout << "Saved waveform to: " << filename << "\n";

    delete[] v1;
    delete[] v2;
    return 0;
}
