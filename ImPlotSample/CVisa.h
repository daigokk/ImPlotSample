#pragma once

#pragma comment(lib, "./VISA/visa64.lib")
#include <visa.h>

void vi_checkError(const ViStatus status, const char filename[], const int line);
void vi_FindRsrc(const ViSession rm);
int scope_save_Waveformsf(ViSession vi, const char* format, ...);

class CVisa {
private:
	static ViSession rm;
	
public:
	static bool OpenRM(const char filename[], const int line);
	static void CloseRM();
	static ViSession OpenInstrument(const char address[], const char filename[], const int line);
	static void CloseInstrument(ViSession instr);
	static bool cviPrintf(const ViSession instr, const char filename[], const int line, const char* format, ...);
	// cviScanfにおいてformatで用いる文字列は%tを使う。%sを使うと空白で区切られた最初の単語しか読み取れない。
	static bool cviScanf(const ViSession instr, const char filename[], const int line, const char* format, void* output);
	static char* cviQueryf(const ViSession vi, const char filename[], const int line, const char* format, ...);
};
