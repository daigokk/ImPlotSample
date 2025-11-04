#pragma once

#include "CppVisa.h"


class Scope
{
private:
	ViSession vi = VI_NULL;
public:
	Scope();
	Scope(const char keyword[]);
	~Scope();

	const char* model = "710105";

	void initialize(const char address[]);
	char* getError();
	void setRun();
	void setStop();
	int getState();
	void setLocal();
	void setRemote();
	void setExecute();
	void setChOn(int ch);
	void setChOff(int ch);
	void setProbeAtte(int ch, int val);
	int getTriggerMode();
	void setTriggerAuto();
	void setTriggerNormal();
	void setTriggerMode(int auto0normal1);
	double getTriggerLevel();
	void setTriggerLevel(double val);
	void setTriggerSource(int ch);
	void setTriggerSourceEXT();
	double getTimeDiv();
	void setTimeDiv(double val);
	void setTimeDivIdx(int idx);
	double getYposition(int ch);
	void setYposition(int ch, double val);
	double getVDiv(int ch);
	void setVDiv(int ch, double val);
	void setVDivIdx(int ch, int idx);
	void setViewXY();
	void setViewYT();
	void setViewCh(int ch);
	int getRecordLength();
	void setRecordLength(const int val);
	void getWaveforms(int ch, std::vector<double>& voltages);
	void getWaveforms(std::vector<double>& voltages1, std::vector<double>& voltages2);
	void saveWaveforms(int ch, const char* filename = "data.csv");
	void saveWaveforms(const char* filename = "data.csv");
};