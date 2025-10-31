#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <Windows.h>
#include "VisaInst.h"

void error(const ViStatus status, const char *comment)
{
	if (status < VI_SUCCESS)
	{
		char buffer[VISA_NUM_STRINGS];
		viStatusDesc(gRm, status, buffer);
		printf("\a");
		printf("--- Error code: %10d ------------------\n", status);
		printf(" %s\n", buffer);
		printf(" %s\n", comment);
		printf("---------------------------------------------\n");
		//assert(status == VI_SUCCESS);
	}
}

void setRS232C(const ViSession vi)
{
	viSetAttribute(vi, VI_ATTR_ASRL_BAUD, VISA_RS232C_BAUD);
	//viSetAttribute(vi, VI_ATTR_ASRL_DATA_BITS, 8);
	//viSetAttribute(vi, VI_ATTR_ASRL_PARITY, VI_ASRL_PAR_NONE);
	//viSetAttribute(vi, VI_ATTR_ASRL_STOP_BITS, VI_ASRL_STOP_ONE);
	//viSetAttribute(vi, VI_ATTR_ASRL_FLOW_CNTRL, VI_ASRL_FLOW_DTR_DSR);
}

int findInst()
{
	ViUInt32 cntInst = 0;
	static ViUInt32 cntRes = 0;
	ViStatus status;
	ViFindList findList;
	ViSession vi = VI_NULL;

	if (cntRes != 0) return cntRes;
	if (gRm == VI_NULL)
		error(viOpenDefaultRM(&gRm), "No VISA resource found. VISA may be not installed.");
	
	cntInst = 0;
	error(viFindRsrc(gRm, "?*INSTR", &findList, &cntRes, listResouces[cntInst]), "viFindRsrc");
	printf("Resources found: %d\n", cntRes);
	for (ViUInt32 i = 0; i < cntRes; i++)
	{
		//printf("\r                                                                  \r");
		printf("%2d/%2d, %s\n", i + 1, cntRes, listResouces[cntInst]);
		status = viOpen(gRm, listResouces[cntInst], VI_NULL, VI_NULL, &vi);
		if (status < VI_SUCCESS)
			error(status, "viOpen");
		else
		{
			if (strstr(listResouces[cntInst], "ASRL") != NULL) setRS232C(vi);
			status = viQueryf(vi, "*IDN?\n", "%255t", listIdns[cntInst]);
			if (status == VI_SUCCESS)
			{
				size_t endOfString = strlen(listIdns[cntInst]);
				if (listIdns[cntInst][endOfString - 1] == '\n')
					listIdns[cntInst][endOfString - 1] = '\0';
				printf("\t%s\n", listIdns[cntInst]);
				cntInst++;
			}
			else
			{
				error(status, "'*IDN?', no responce.");
			}
			viClose(vi);
		}
		status = viFindNext(findList, listResouces[cntInst]);
		if (status < VI_SUCCESS) break;
	}
	printf("\rInstruments found: %d                                                 \n", cntInst);

	for (ViUInt32 i = 0; i < cntInst; i++)
	{
		printf("%2d. %s\n", i + 1, listResouces[i]);
		printf("    %s\n", listIdns[i]);
	}

	return cntRes;
}

VisaInst::~VisaInst()
{
	if (vi != VI_NULL)
		viClose(vi);
}

void VisaInst::error(const ViStatus status, const char *comment)
{
	if (status < VI_SUCCESS)
	{
		char buffer[VISA_NUM_STRINGS];
		viStatusDesc(gRm, status, buffer);
		printf("\a");
		printf("--- Error code: %10d ------------------\n", status);
		printf(" %s\n", buffer);
		printf(" %s %s: %s\n", model, serial, comment);
		printf("---------------------------------------------\n");
		printf("\a\nHit enter to end this program.\n"); getchar();
		exit(status);
	}
}

void VisaInst::open(const int index)
{
	ViStatus status = viOpen(gRm, listResouces[index], VI_NO_LOCK, 1000, &vi);
	if (status == VI_SUCCESS)
	{
		if (strstr(listResouces[index], "ASRL") != NULL) setRS232C(vi);
		if(VISA_ENABLE_RESET == 1) writef("*RST");
		writef("*CLS");
	}
	else
	{
		printf("--- Error -----------------------------------\n");
		printf("'%s' couldn't be connected.\n", listResouces[index]);
		printf("---------------------------------------------\n");
		printf("\a\nHit enter to end this program.\n");
		getchar();
		exit(status);
	}
}

void VisaInst::open(const char *id)
{
	int cntRes = findInst();
	int index, flag = 0;

	for (int i = 0; i < cntRes; i++)
	{
		if (strcmp(id, "") !=0 && strstr(listIdns[i], id) != NULL)
		{
			flag++;
			index = i;
		}
	}
	if (flag == 0)
	{
		printf("--- Error -----------------------------------\n");
		printf("'%s' was not found.\n",id);
		printf("---------------------------------------------\n");
		printf("\a\nHit enter to end this program.\n");
		getchar();
		exit(EXIT_FAILURE);
	}
	else if (flag == 1)
	{
		char buffer[VISA_NUM_STRINGS];

		strncpy(buffer, listIdns[index], sizeof(buffer) - 1);
		strtok(buffer, ",");
		strncpy(VisaInst::model, strtok(NULL, ","), sizeof(VisaInst::model) - 1);
		if (VisaInst::model[0] == ' ')
			strncpy(VisaInst::model, &(VisaInst::model[1]), sizeof(VisaInst::model) - 1);
		strncpy(VisaInst::serial, strtok(NULL, ","), sizeof(VisaInst::serial) - 1);
		if (VisaInst::serial[0] == ' ')
			strncpy(VisaInst::serial, &(VisaInst::serial[1]), sizeof(VisaInst::serial) - 1);
		open(index);
	}
	else
	{
		printf("--- Error -----------------------------------\n");
		printf("There are %d instruments which have the same name (%s).\n", flag, VisaInst::model);
		printf("This should be called with the unique serial.\n");
		printf("---------------------------------------------\n");
		printf("\a\nHit enter to end this program.\n");
		getchar();
		exit(EXIT_FAILURE);
	}
}

void VisaInst::opc()
{
	ViStatus status;
	char ret[VISA_NUM_STRINGS] = {"0"};
	if (enableOPC == 0)
	{
		Sleep(100);
		return;
	}
	do
	{
		status = viQueryf(vi, "*OPC?\n", "%255t", ret);
	} while (strcmp(ret, "1\n") != 0);
	//status = viFlush(vi, VI_READ_BUF | VI_WRITE_BUF);
}

void VisaInst::writef(const char *format, ...)
{
	char cmd[VISA_NUM_STRINGS];
	va_list ap;
	va_start(ap, format);
	vsnprintf(cmd, VISA_NUM_STRINGS - 1, format, ap);
	va_end(ap);
	if(enableViewCommands == 1) printf("%s %s: '%s' is running...", model, serial, cmd);
	//opc();
	error(viPrintf(vi, "%s\n", cmd), cmd);
	opc();
	if (enableViewCommands == 1) printf("\r%s %s: '%s'              \n", model, serial, cmd);
}

void VisaInst::writefWithoutOPC(const char *format, ...)
{
	char cmd[VISA_NUM_STRINGS];
	va_list ap;
	va_start(ap, format);
	vsnprintf(cmd, VISA_NUM_STRINGS - 1, format, ap);
	va_end(ap);
	if (enableViewCommands == 1) printf("%s %s: '%s' is running...", model, serial, cmd);
	//opc();
	error(viPrintf(vi, "%s\n", cmd), cmd);
	//opc();
	if (enableViewCommands == 1) printf("\r%s %s: '%s'              \n", model, serial, cmd);
}

char* VisaInst::queryf(const char *format, ...)
{
	static char ret[VISA_NUM_STRINGS];
	char cmd[VISA_NUM_STRINGS];
	va_list ap;
	va_start(ap, format);
	vsnprintf(cmd, VISA_NUM_STRINGS - 1, format, ap);
	va_end(ap);
	opc();
	if (enableViewCommands == 1) printf("%s %s: '%s' is running...", model, serial, cmd);
	error(viQueryf(vi, "%s\n", "%255t", cmd, ret), cmd);
	size_t endOfString = strlen(ret);
	if (ret[endOfString - 1] == '\n')
		ret[endOfString - 1] = '\0';
	opc();
	if (enableViewCommands == 1) printf("\r%s %s: '%s', %s               \n", model, serial, cmd, ret);
	return ret;
}

char VisaInst::viGetchar()
{
	char c;
	error(viScanf(vi, "%c", &c), "Get a character.");
	return c;
}

char* VisaInst::idn()
{
	return queryf("*IDN?");
}

void VisaInst::saveCSV(const int numStrings, const int length, const char *lines, const char *filename)
{
	std::ofstream ofs(filename, std::ios::out);

	if (ofs.fail() != 0)
	{
		printf("--- File IO error --------------------------\n");
		printf("'%s' couldn't be opened.\n", filename);
		printf("---------------------------------------------\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < length; i++)
	{
		ofs << &(lines[i * numStrings]) << std::endl;
	}
}
