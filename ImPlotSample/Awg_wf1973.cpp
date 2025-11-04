#include "Awg_wf1973.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Awg_wf1973::Awg_wf1973()
{
	init(model);
}

Awg_wf1973::Awg_wf1973(const char *serial)
{
	init(serial);
}

Awg_wf1973::~Awg_wf1973()
{
//	writef("");
}

void Awg_wf1973::init(const char address[])
{
	vi = CppVisa::OpenInstrument(address);
	setVmin();
	setLoad50();
	setSin();
}

char* Awg_wf1973::getError()
{
	return CppVisa::Queryf(vi, "SYSTem:ERRor?\n");
}

void Awg_wf1973::setLoad50()
{
	CppVisa::Printf(vi, "OUTP1:LOAD 50OHM\n");
}

void Awg_wf1973::setLoadHi()
{
	CppVisa::Printf(vi, "OUTP:LOAD INFinity\n");
}
void Awg_wf1973::setOn()
{
	CppVisa::Printf(vi, "OUTP:STAT ON\n");
}

void Awg_wf1973::setOff()
{
	CppVisa::Printf(vi, "OUTP:STAT OFF\n");
}

int Awg_wf1973::getMode()
{
	char *ret;
	ret = CppVisa::Queryf(vi, "MODE?\n");
	if (strcmp(ret, "NORMal") == 0)
		return 0;
	else if (strcmp(ret, "BURSt") == 0)
		return 1;
	else
		return 3;
}

void Awg_wf1973::setModeCont()
{
	CppVisa::Printf(vi, "MODE NORMal\n");
}

void Awg_wf1973::setModeBrst()
{
	CppVisa::Printf(vi, "MODE BURSt\n");
	CppVisa::Printf(vi, "BM:TYPE BURst\n");
}

void Awg_wf1973::setBrstCycles(double val)
{
	CppVisa::Printf(vi, "BM:MARK  %f\n", val);
}

double Awg_wf1973::getBrstCycles()
{
	return atof(CppVisa::Queryf(vi, "BM:MARK?\n"));
}

void Awg_wf1973::setBrstSpaces(double val)
{
	CppVisa::Printf(vi, "BM:SPACe  %f\n", val);
}

double Awg_wf1973::getBrstSpaces()
{
	return atof(CppVisa::Queryf(vi, "BM:SPACe?\n"));
}

void Awg_wf1973::setModeTrig()
{
	CppVisa::Printf(vi, "BM:TYPE TRIGger\n");
}

void Awg_wf1973::setSin()
{
	CppVisa::Printf(vi, "FUNC:SHAP SIN\n");
}

void Awg_wf1973::setSqu()
{
	CppVisa::Printf(vi, "FUNC:SHAP SQU\n");
}

void Awg_wf1973::setTri()
{
	CppVisa::Printf(vi, "FUNC:SHAP RAMP\n");
}

void Awg_wf1973::setVmin()
{
	CppVisa::Printf(vi, "VOLT MIN\n");
}

double Awg_wf1973::getFreq()
{
	return atof(CppVisa::Queryf(vi, "FREQ?\n"));
}

void Awg_wf1973::setFreq(double val)
{
	CppVisa::Printf(vi, "FREQ %f\n", val);
}

double Awg_wf1973::getVpp()
{
	CppVisa::Printf(vi, "VOLT:UNIT VPP\n");
	return atof(CppVisa::Queryf(vi, "VOLT?\n"));
}

void Awg_wf1973::setVpp(double val)
{
	CppVisa::Printf(vi, "VOLT:UNIT VPP\n");
	CppVisa::Queryf(vi, "VOLT %fVPP\n", val);
}

double Awg_wf1973::getVrms()
{
	char *ret;
	double v;
	CppVisa::Printf(vi, "VOLT:UNIT VRMS\n");
	v = atof(CppVisa::Queryf(vi, "VOLT?\n"));

	ret = CppVisa::Queryf(vi, "FUNC:SHAP?\n");
	if (strcmp("SIN", ret))
	{
		printf("'VRMS' cannot be used with the %s mode.\n", ret);
		//printf("Vpp: %.3f\n", v);
	}
	else
	{
		//printf("Vrms: %.3f\n", v);
	}
	
	return v;
}

void Awg_wf1973::setVrms(double val)
{
	char *ret;
	ret = CppVisa::Queryf(vi, "FUNC:SHAP?\n");
	if (strcmp("SIN", ret))
	{
		printf("'VRMS' cannot be used with the %s mode.\n", ret);
	}
	CppVisa::Printf(vi, "VOLT:UNIT VRMS\n");
	CppVisa::Printf(vi, "VOLT %fVRMS\n", val);
}

double Awg_wf1973::getOffset()
{
	return atof(CppVisa::Queryf(vi, "VOLT:OFFSet?\n"));
}

void Awg_wf1973::setOffset(double val)
{
	CppVisa::Printf(vi, "VOLT:OFFSet %f\n", val);
}

void Awg_wf1973::setDuty(double parcent)
{
	CppVisa::Printf(vi, "PULS:DCYCle:CENTer %f\n", parcent);
}
