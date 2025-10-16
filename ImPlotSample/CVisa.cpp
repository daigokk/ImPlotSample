#include <cstdarg>   // va_list など
#include <iostream>
#include "CVisa.h"

ViSession CVisa::rm;

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

void vi_FindRsrc(const ViSession rm) {
    // 接続されている計測器を検索（例: GPIB, USB, TCPIPなど）
	ViStatus status;
	ViFindList findList;
    ViUInt32 numInstrs;
	ViChar instrDesc[256];
    status = viFindRsrc(rm, "?*INSTR", &findList, &numInstrs, instrDesc);
    if (status < VI_SUCCESS) {
        std::cerr << "計測器の検索に失敗しました。" << std::endl;
        return;
    }

    std::cout << "見つかった計測器の数: " << numInstrs << std::endl;
    std::cout << "1: " << instrDesc << std::endl;

    // 残りの計測器を取得
    for (ViUInt32 i = 1; i < numInstrs; ++i) {
        status = viFindNext(findList, instrDesc);
        if (status < VI_SUCCESS) break;
        std::cout << i + 1 << ": " << instrDesc << std::endl;
    }

    viClose(findList);
}

bool CVisa::OpenRM(const char filename[], const int line) {
    ViStatus status = viOpenDefaultRM(&rm);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
		return false;
    }
	return true;
}

ViSession CVisa::OpenInstrument(const char address[], const char filename[], const int line) {
    ViSession instr = VI_NULL;
    ViStatus status = viOpen(rm, address, VI_NULL, VI_NULL, &instr);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return VI_NULL;
    }

    status = viSetAttribute(instr, VI_ATTR_TMO_VALUE, 2000);
    if (status < VI_SUCCESS) {
        std::cerr << "[Error] viSetAttribute failed: " << status << "\n";
        return VI_NULL;
    }

    return instr;
}

void CVisa::CloseRM() {
    if (rm) {
        viClose(rm);
        rm = VI_NULL;
    }
}

void CVisa::CloseInstrument(ViSession instr) {
    if (instr) {
        viClose(instr);
    }
}

bool CVisa::cviPrintf(const ViSession instr, const char filename[], const int line, const char* format, ...) {
    va_list args;
    va_start(args, format);
    ViStatus status = viPrintf(instr, format, args);
    va_end(args);

    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
    return true;
}

// cviScanfにおいてformatで用いる文字列は%tを使う。%sを使うと空白で区切られた最初の単語しか読み取れない。
bool CVisa::cviScanf(const ViSession instr, const char filename[], int line, const char* format, void* output) {
    ViStatus status = viScanf(instr, format, output);
    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return false;
    }
    return true;
}

char* CVisa::cviQueryf(const ViSession instr, const char filename[], const int line, const char* format, ...) {
    static char ret[256];
    va_list args;

    va_start(args, format);
    ViStatus status = viPrintf(instr, format, args);
    va_end(args);

    if (status < VI_SUCCESS) {
        vi_checkError(status, filename, line);
        return nullptr;
    }

    status = viScanf(instr, "%255t", ret);
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

int scope_get_RecordLength(ViSession vi)
{
	char ret[256];
	vi_checkError(viQueryf(vi, "%s", "%255t", ":ACQuire:RLENgth?\n", ret), __FILE__, __LINE__);
	return atoi(ret);
}

void scope_set_ViewCh(ViSession vi, int ch)
{
	vi_checkError(viPrintf(vi, "CHANnel%d:DISPlay ON\n"), __FILE__, __LINE__);
}

int scope_get_State(ViSession vi)
{
	char ret[256];
	vi_checkError(viQueryf(vi, "%s", "%255t", ":STATus:CONDition?\n", ret), __FILE__, __LINE__);
	if (atoi(ret) % 2 == 1)
		return 1;	//RUN
	else
		return 0;	//STOP
}

void scope_set_Stop(ViSession vi)
{
	vi_checkError(viPrintf(vi, "STOP\n"), __FILE__, __LINE__);
}

void scope_set_Run(ViSession vi)
{
	vi_checkError(viPrintf(vi, "STARt\n"), __FILE__, __LINE__);
}

void scope_set_timediv(ViSession vi, double sec)
{
	vi_checkError(viPrintf(vi, "TIMebase:TDIV %f\n", sec), __FILE__, __LINE__);
}

double scope_get_timediv(ViSession vi)
{
	char ret[256];
	vi_checkError(viQueryf(vi, "%s", "%255t", "TIMebase:TDIV?\n", ret), __FILE__, __LINE__);
	return atof(ret);
}

void scope_get_Waveforms(ViSession vi, int ch, double voltages[])
{
	vi_checkError(viPrintf(vi, "COMMunicate:HEADer OFF\n"), __FILE__, __LINE__);
	double position, range, offset;
	int length, count;
	char c;
	char ret[256];

	length = scope_get_RecordLength(vi);
	scope_set_ViewCh(vi, ch);

	vi_checkError(viPrintf(vi, "WAVeform:STARt 0\n"), __FILE__, __LINE__);
	vi_checkError(viPrintf(vi, "WAVeform:END %d\n", length - 1), __FILE__, __LINE__);
	vi_checkError(viPrintf(vi, "WAVeform:TRACe %d\n", ch), __FILE__, __LINE__);
	vi_checkError(viPrintf(vi, "WAVeform:FORMat RBYTe\n"), __FILE__, __LINE__);

	
	vi_checkError(viQueryf(vi, "%s", "%255t", "WAVeform:POSition ?\n", ret), __FILE__, __LINE__);
	position = atof(ret);
	vi_checkError(viQueryf(vi, "%s", "%255t", "WAVeform:RANGe?\n", ret), __FILE__, __LINE__);
	range = atof(ret);
	vi_checkError(viQueryf(vi, "%s", "%255t", "WAVeform:OFFSet?\n", ret), __FILE__, __LINE__);
	offset = atof(ret);
	vi_checkError(viPrintf(vi, "WAVeform:SEND?\n"), __FILE__, __LINE__);

	c = visa_viGetchar(vi);
	if (c != '#') exit(-1);
	c = visa_viGetchar(vi);
	if (c < '0' && '9' < c) exit(-1);
	count = c - '0';
	for (int i = 0; i < count; i++)
	{
		c = visa_viGetchar(vi);
		if (c < '0' && '9' < c) exit(-1);
	}
	for (int i = 0; i < length; i++)
	{
		c = visa_viGetchar(vi);
		voltages[i] = range * ((unsigned char)c - position) / 25 + offset;
	}
}

int scope_save_Waveformsf(ViSession vi, const char* format, ...)
{
	double* v1, * v2;
	int length;
	double dt;
	char* lines;

	char filename[256];
	va_list ap;
	va_start(ap, format);
	vsnprintf(filename, 256 - 1, format, ap);
	va_end(ap);

	length = scope_get_RecordLength(vi);
	dt = scope_get_timediv(vi) * 10 / length;
	v1 = (double*)malloc(sizeof(double) * length);
	v2 = (double*)malloc(sizeof(double) * length);

	int flag = scope_get_State(vi);
	scope_set_Stop(vi);
	scope_get_Waveforms(vi, 1, v1);
	scope_get_Waveforms(vi, 2, v2);
	if (flag == 1)
		scope_set_Run(vi);

	FILE* fp;
	fopen_s(&fp, filename, "w");
	if (fp == NULL)
	{
		printf("Failed. '%s' couldn't be opened.\n", filename);
		return -1;
	}
	for (int i = 0; i < length; i++)
	{
		fprintf(fp, "%e, %e, %e\n", (i + 1) * dt, v1[i], v2[i]);
	}
	fclose(fp);
	printf("Succeeded. '%s' could be saved.\n", filename);
	free(v1); v1 = NULL;
	free(v2); v2 = NULL;
	return 0;
}
