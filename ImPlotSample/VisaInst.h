#pragma once
#include "visa.h"

#define VISA_FILENAME_CSV "waveforms.csv"
#define VISA_NUM_STRINGS 256
#define VISA_MAX_INSTRUMENTS 10
#define VISA_ENABLE_RESET 1
#define VISA_RS232C_BAUD 9600

static ViSession gRm = VI_NULL;
static char listResouces[VISA_MAX_INSTRUMENTS][VISA_NUM_STRINGS];
static char listIdns[VISA_MAX_INSTRUMENTS][VISA_NUM_STRINGS];

class VisaInst
{
private:
	ViSession vi = VI_NULL;
	char model[VISA_NUM_STRINGS] = { "" };
	char serial[VISA_NUM_STRINGS] = { "" };
	void open(const int index);
	void opc();
public:
	int enableOPC = 1;
	int enableViewCommands = 1;
	~VisaInst();
	void error(const ViStatus status, const char *comment);
	void open(const char *id);
	void writef(const char *format, ...);
	void writefWithoutOPC(const char *format, ...);
	char* queryf(const char *format, ...);
	char viGetchar();
	char* idn();
	static void saveCSV(const int numStrings, const int length, const char *lines, const char *filename);
};
