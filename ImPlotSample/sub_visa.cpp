#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>
#include "sub_visa.h"
#define VISA_NUM_STRING 1024

ViSession rm = VI_NULL;
int enableOPC = 1;
int enableViewCommands = 1;
char terminator[3] = { "\r\n" };

ViSession vi_get_handle(char* visa_resource_name)
{
	ViStatus status;
	char ret[VISA_NUM_STRING];
	if (rm == VI_NULL)
	{
		status = viOpenDefaultRM(&rm);	//1. VISAのハンドルを取得
		vi_checkError(status, __FILE__, __LINE__);
	}
	ViSession vi;
	status = viOpen(rm, visa_resource_name, VI_NULL, VI_NULL, &vi); //2. 計測器のハンドルを取得
	vi_checkError(status, __FILE__, __LINE__);
	return vi;
}

void vi_close_handle(ViSession vi)
{
	viClose(vi);	// 計測器のハンドルを開放
}

void vi_close()
{
	viClose(rm);	// VISAのハンドルを開放
}

void vi_checkError(ViStatus status, const char* filename, int line)
{
	ViSession rm;
	viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS)
	{
		char buffer[VISA_NUM_STRING];
		viStatusDesc(rm, status, buffer);
		printf("\a");
		printf("--- VISA error code: %10d, File: %s, Line: %3d ---\n", status, filename, line);
		printf(" %s\n", buffer);
		printf("--- 小坂のコメント----------------------------------------------\n");
		switch (status)
		{
		case -1073807343: //viOpen error
			printf(" 計測器が見つからないという意味のエラーです。\n");
			printf(" VISAリソース名を間違えているとこのエラーが出ます。NI MAXの画面上で表示されるリソース名をコピーして使いましょう。\n");
			printf(" まさか、計測器の電源が入っていない、ということはありませんか？\n"); 
			printf(" もしくは、計測器と自分が使っているPCにUSBケーブルで接続されているか確認してみましょう。\n");
			printf(" オシロスコープ(DLM2022)の場合、USBケーブルをLANのコネクタにさしてしまう人がよくいます。\n");
			break;
		case -1073807346: // 
			printf(" 計測器のハンドルを取得せずに通信しようとすると出るエラーです。viOpenを実行していないか、失敗しています。\n");
			printf(" viOpenで指定するVISAリソース名を確認してください。NI MAXの画面上で表示されるリソース名をコピーして使いましょう。\n");
			break;
		case -1073807339: //Timeout
			printf(" viScanfを実行した際に計測器から答えが帰って来なかった時に出るエラーです。\n");
			printf(" 直前にviPrintfで送った問い合わせコマンド(最後に?がついている)を間違えているか、送っていない可能性があります。\n");
			printf(" 問い合わせコマンドを送っていない場合は、viScanfの行を削除してください。\n");
			printf(" 問い合わせコマンドを送る必要がある場合は、直前のviPrintfの行を確認してください。\n");
			break;
		default:
			printf(" 何かがおかしい、何かを間違えています。\n");
			printf(" まず、どの行で出ているエラーなのかを確認してください。\n");
			printf(" ソースコードを10回読んでもわからなければ、小坂を呼んでください。原因を一緒に考えましょう。\n");
		}
		printf("----------------------------------------------------------------\n");
	}
	viClose(rm);
}

char* vi_get_idn(ViSession vi)
{
	static char idn[VISA_NUM_STRING];
	vi_checkError(viQueryf(vi, "%s%s", "%256t", "*IDN?", terminator, idn), __FILE__, __LINE__);
	return idn;
}

int fg_get_error(ViSession vi)
{
	char status[VISA_NUM_STRING];
	vi_checkError(viQueryf(vi, "%s%s", "%256t", ":SYSTem:ERRor?", terminator, status), __FILE__, __LINE__);
	if (atoi(status) != 0)
	{
		printf("--- Instrument Error code: %s --------------\n", status);
		printf(" ファンクションジェネレータ(WF1973)の取扱説明書(pp. 196-198)を参照して原因を究明してください。\n");
		printf("----------------------------------------------------------------\n");
	}
	return atoi(status);
}

void fg_set_freq(ViSession vi, double freq)
{
	vi_checkError(viPrintf(vi, "freq %f\n", freq), __FILE__, __LINE__);
	fg_get_error(vi);
}

double fg_get_freq(ViSession vi)
{
	char ret[VISA_NUM_STRING];
	vi_checkError(viPrintf(vi, "freq?\n"), __FILE__, __LINE__);
	vi_checkError(viScanf(vi, "%255t", ret), __FILE__, __LINE__);
	fg_get_error(vi);
	return atof(ret);
}

int os_get_error(ViSession vi)
{
	char status[VISA_NUM_STRING];
	vi_checkError(viQueryf(vi, "%s%s", "%256t", "STATus:ERRor?", terminator, status), __FILE__, __LINE__);
	if (atoi(status) != 0)
	{
		printf("--- Instrument Error code: %s --------------\n", status);
		printf(" オシロスコープ(DLM2000)の取扱説明書(付録2エラーメッセージ)を参照して原因を究明してください。\n");
		printf("----------------------------------------------------------------\n");
	}
	return atoi(status);
}

void os_set_timediv(ViSession vi, double sec)
{
	vi_checkError(viPrintf(vi, "TIMebase:TDIV %f\n", sec), __FILE__, __LINE__);
	char status[VISA_NUM_STRING]; 
	os_get_error(vi);
}

double os_get_timediv(ViSession vi)
{
	char ret[VISA_NUM_STRING];
	vi_checkError(viPrintf(vi, "TIMebase:TDIV?\n"), __FILE__, __LINE__);
	vi_checkError(viScanf(vi, "%255t", ret), __FILE__, __LINE__);
	char status[VISA_NUM_STRING]; 
	os_get_error(vi);
	return atof(ret);
}

void error(const ViStatus status, const char* comment)
{
	ViSession rm;
	viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS)
	{
		char buffer[VISA_NUM_STRING];
		viStatusDesc(rm, status, buffer);
		printf("\a");
		printf("--- Error code: %10d ------------------\n", status);
		printf(" %s\n", buffer);
		printf(" %s\n", comment);
		printf("---------------------------------------------\n");
		//assert(status == VI_SUCCESS);
	}
	viClose(rm);
}

void visa_opc(ViSession vi)
{
	ViStatus status;
	char ret[VISA_NUM_STRING] = { "0" };
	if (enableOPC == 0)
	{
		Sleep(100);
		return;
	}
	do
	{
		//status = viPrintf(vi, "*OPC?\n");
		//status = viScanf(vi, "%256t", ret);
		status = viQueryf(vi, "*OPC?%s", "%256t", terminator, ret);
	} while (strcmp(ret, "1\n") != 0);
	char cmd[VISA_NUM_STRING];
	sprintf(cmd, "*CLS%s", terminator);
	status = viPrintf(vi, cmd);
	//status = viFlush(vi, VI_READ_BUF | VI_WRITE_BUF);
}

char visa_viGetchar(ViSession vi)
{
	char c;
	error(viScanf(vi, "%c", &c), "Get a character.");
	return c;
}

void visa_writef(ViSession vi, const char* format, ...)
{
	char cmd[VISA_NUM_STRING];
	char cmd2[VISA_NUM_STRING];
	va_list ap;
	va_start(ap, format);
	vsnprintf(cmd, VISA_NUM_STRING - 1, format, ap);
	va_end(ap);
	if (enableViewCommands == 1) printf("'%s' is running...", cmd);
	sprintf(cmd2, "%s%s", cmd, terminator);
	error(viPrintf(vi, cmd2), cmd);
	visa_opc(vi);
	if (enableViewCommands == 1) printf("\r'%s'\t\t\t\n", cmd);
}

void visa_writefWithoutOPC(ViSession vi, const char* format, ...)
{
	char cmd[VISA_NUM_STRING];
	char cmd2[VISA_NUM_STRING];
	va_list ap;
	va_start(ap, format);
	vsnprintf(cmd, VISA_NUM_STRING - 1, format, ap);
	va_end(ap);
	if (enableViewCommands == 1) printf("'%s' is running...", cmd);
	//opc();
	sprintf(cmd2, "%s%s", cmd, terminator);
	error(viPrintf(vi, cmd2), cmd);
	//opc();
	if (enableViewCommands == 1) printf("\r'%s'\t\t\t\n", cmd);
}

char* visa_queryf(ViSession vi, const char* format, ...)
{
	static char ret[VISA_NUM_STRING];
	char cmd[VISA_NUM_STRING];
	va_list ap;
	va_start(ap, format);
	vsnprintf(cmd, VISA_NUM_STRING - 1, format, ap);
	va_end(ap);
	visa_opc(vi);
	if (enableViewCommands == 1) printf("'%s' is running...", cmd);
	error(viQueryf(vi, "%s%s", "%256t", cmd, terminator, ret), cmd);
	size_t endOfString = strlen(ret);
	if (ret[endOfString - 1] == '\n') ret[endOfString - 1] = '\0';
	if (ret[endOfString - 2] == '\r') ret[endOfString - 2] = '\0';
	visa_opc(vi);
	if (enableViewCommands == 1) printf("\r'%s', %s\t\t\t\n", cmd, ret);
	return ret;
}

int os_get_RecordLength(ViSession vi)
{
	return atoi(visa_queryf(vi, ":ACQuire:RLENgth?"));
}

void os_set_ViewCh(ViSession vi, int ch)
{
	visa_writef(vi, "CHANnel%d:DISPlay ON", ch);
}

int os_get_State(ViSession vi)
{
	if (atoi(visa_queryf(vi, ":STATus:CONDition?")) % 2 == 1)
		return 1;	//RUN
	else
		return 0;	//STOP
}

void os_set_Stop(ViSession vi)
{
	visa_writef(vi, "STOP");
}

void os_set_Run(ViSession vi)
{
	visa_writef(vi, "STARt");
}

void os_get_Waveforms(ViSession vi, int ch, double* voltages)
{
	visa_writef(vi, "COMMunicate:HEADer OFF");
	double position, range, offset;
	int length, count;
	char c;

	length = os_get_RecordLength(vi);
	os_set_ViewCh(vi, ch);

	
	visa_writef(vi, "WAVeform:STARt 0");
	visa_writef(vi, "WAVeform:END %d", length - 1);
	visa_writef(vi, "WAVeform:TRACe %d", ch);
	visa_writef(vi, "WAVeform:FORMat RBYTe");
	position = atof(visa_queryf(vi, "WAVeform:POSition?"));
	range = atof(visa_queryf(vi, "WAVeform:RANGe?"));
	offset = atof(visa_queryf(vi, "WAVeform:OFFSet?"));
	visa_writefWithoutOPC(vi, "WAVeform:SEND?");

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

int os_save_Waveformsf(ViSession vi, const char* format, ...)
{
	double* v1, * v2;
	int length;
	double dt;
	char* lines;
	
	char filename[VISA_NUM_STRING];
	va_list ap;
	va_start(ap, format);
	vsnprintf(filename, VISA_NUM_STRING - 1, format, ap);
	va_end(ap);

	length = os_get_RecordLength(vi);
	dt = os_get_timediv(vi) * 10 / length;
	v1 = (double*)malloc(sizeof(double) * length);
	v2 = (double*)malloc(sizeof(double) * length);

	int flag = os_get_State(vi);
	os_set_Stop(vi);
	os_get_Waveforms(vi, 1, v1);
	os_get_Waveforms(vi, 2, v2);
	if (flag == 1)
		os_set_Run(vi);

	FILE* fp;
	fp = fopen(filename, "w");
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
