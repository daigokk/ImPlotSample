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

void Awg_wf1973::init(const char *id)
{
	enableOPC = 0;
	open(id);
	setVmin();
	setLoad50();
	setSin();
}

char* Awg_wf1973::getError()
{
	return queryf("SYSTem:ERRor?");
}

void Awg_wf1973::setLoad50()
{
	writef("OUTP1:LOAD 50OHM");
}

void Awg_wf1973::setLoadHi()
{
	writef("OUTP:LOAD INFinity");
}
void Awg_wf1973::setOn()
{
	writef("OUTP:STAT ON");
}

void Awg_wf1973::setOff()
{
	writef("OUTP:STAT OFF");
}

int Awg_wf1973::getMode()
{
	char *ret;
	ret = queryf("MODE?");
	if (strcmp(ret, "NORMal") == 0)
		return 0;
	else if (strcmp(ret, "BURSt") == 0)
		return 1;
	else
		return 3;
}

void Awg_wf1973::setModeCont()
{
	writef("MODE NORMal");
}

void Awg_wf1973::setModeBrst()
{
	writef("MODE BURSt");
	writef("BM:TYPE BURst");
}

void Awg_wf1973::setBrstCycles(double val)
{
	writef("BM:MARK  %f", val);
}

double Awg_wf1973::getBrstCycles()
{
	return atof(queryf("BM:MARK?"));
}

void Awg_wf1973::setBrstSpaces(double val)
{
	writef("BM:SPACe  %f", val);
}

double Awg_wf1973::getBrstSpaces()
{
	return atof(queryf("BM:SPACe?"));
}

void Awg_wf1973::setModeTrig()
{
	writef("BM:TYPE TRIGger");
}

void Awg_wf1973::setSin()
{
	writef("FUNC:SHAP SIN");
}

void Awg_wf1973::setSqu()
{
	writef("FUNC:SHAP SQU");
}

void Awg_wf1973::setTri()
{
	writef("FUNC:SHAP RAMP");
}

void Awg_wf1973::setVmin()
{
	writef("VOLT MIN");
}

double Awg_wf1973::getFreq()
{
	return atof(queryf("FREQ?"));
}

void Awg_wf1973::setFreq(double val)
{
	writef("FREQ %f", val);
}

double Awg_wf1973::getVpp()
{
	writef("VOLT:UNIT VPP");
	return atof(queryf("VOLT?"));
}

void Awg_wf1973::setVpp(double val)
{
	writef("VOLT:UNIT VPP");
	writef("VOLT %fVPP", val);
}

double Awg_wf1973::getVrms()
{
	char *ret;
	double v;
	writef("VOLT:UNIT VRMS");
	v = atof(queryf("VOLT?"));

	ret = queryf("FUNC:SHAP?");
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
	ret = queryf("FUNC:SHAP?");
	if (strcmp("SIN", ret))
	{
		printf("'VRMS' cannot be used with the %s mode.\n", ret);
	}
	writef("VOLT:UNIT VRMS");
	writef("VOLT %fVRMS", val);
}

double Awg_wf1973::getOffset()
{
	return atof(queryf("VOLT:OFFSet?"));
}

void Awg_wf1973::setOffset(double val)
{
	writef("VOLT:OFFSet %f", val);
}

void Awg_wf1973::setDuty(double parcent)
{
	writef("PULS:DCYCle:CENTer %f", parcent);
}
