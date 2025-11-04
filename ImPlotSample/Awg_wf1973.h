#pragma once

#include "CppVisa.h"


class Awg_wf1973
{
private:
	ViSession vi = VI_NULL;
public:
	Awg_wf1973(const char *serial);
	Awg_wf1973();
	~Awg_wf1973();

	const char *model = "WF1973";

	void init(const char address[]);
	char* getError();
	void setLoad50();
	void setLoadHi();
	void setOn();
	void setOff();
	int getMode();
	void setModeCont();
	void setModeBrst();
	void setBrstCycles(double val);
	double getBrstCycles();
	void setBrstSpaces(double val);
	double getBrstSpaces();
	void setModeTrig();
	void setSin();
	void setSqu();
	void setTri();
	void setVmin();
	double getFreq();
	void setFreq(double val);
	double getVpp();
	void setVpp(double val);
	double getVrms();
	void setVrms(double val);
	double getOffset();
	void setOffset(double val);
	void setDuty(double parcent);
};
