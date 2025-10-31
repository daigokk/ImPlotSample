#include "Scope.h"
#include <iostream>
#include <string>
#include <stdlib.h>

Scope::Scope()
{
	init(model);
}

Scope::Scope(const char *serial)
{
	init(serial);
}

Scope::~Scope()
{
	//setRecordLength(1250000);
	//writef("COMMunicate:REMote OFF");
}

void Scope::init(const char *id)
{
	open(id);
	writef("COMMunicate:HEADer OFF");
	setRecordLength(12500);
}

char* Scope::getError()
{
	return queryf("STATus:ERRor?");
}

void Scope::setRun()
{
	writef("STARt");
}

void Scope::setStop()
{
	writef("STOP");
}

int Scope::getState()
{
	if (atoi(queryf(":STATus:CONDition?")) % 2 == 1)
		return 1;	//RUN
	else
		return 0;	//STOP
}

void Scope::setLocal()
{
	writef(":COMMunicate:REMote OFF");
}

void Scope::setRemote()
{
	writef(":COMMunicate:REMote ON");
}

void Scope::setExecute()
{
	writef("ASETup:EXECute");
}

void Scope::setChOn(int ch)
{
	writef("CHANnel%d:DISPlay ON", ch);
}

void Scope::setChOff(int ch)
{
	writef("CHANnel%d:DISPlay OFF", ch);
}

void Scope::setProbeAtte(int ch, int val)
{
	writef("CHANnel%d:PROBe:MODE %d", ch, val);
}

int Scope::getTriggerMode()
{
	char *ret;
	ret = queryf(":TRIGger:MODE?");
	if (strstr(ret, "AUTO") != NULL)
		return 1;
	else if (strstr(ret, "NORM") != NULL)
		return 2;
	else if (strstr(ret, "ALEV") != NULL)
		return 3;
	else
		return 0;
}

void Scope::setTriggerAuto()
{
	writef(":TRIGger:MODE AUTO");
}

void Scope::setTriggerNormal()
{
	writef(":TRIGger:MODE NORMal");
}

void Scope::setTriggerMode(int auto0normal1)
{
	if (auto0normal1 == 0)
		setTriggerAuto();
	else
		setTriggerNormal();
}

double Scope::getTriggerLevel()
{
	return atof(queryf("TRIGger:SIMPle:LEVel?"));
}

void Scope::setTriggerLevel(double val)
{
	writef("TRIGger:SIMPle:LEVel %f", val);
}

void Scope::setTriggerSource(int ch)
{
	writef("TRIGger:SIMPle:SOURce %d", ch);
}

void Scope::setTriggerSourceEXT()
{
	writef("TRIGger:SIMPle:SOURce EXT");
}

double Scope::getTimeDiv()
{
	return atof(queryf("TIMebase:TDIV?"));
}

void Scope::setTimeDiv(double val)
{
	writef("TIMebase:TDIV %f", val);
}

void Scope::setTimeDivIdx(int idx)
{
	static double t_ranges[] = {
		1e-8,2e-8,5e-8,
		1e-7,2e-7,5e-7,
		1e-6,2e-6,5e-6,
		1e-5,2e-5,5e-5,
		1e-4,2e-4,5e-4,
		1e-3,2e-3,5e-3,
		1e-2,2e-2,5e-2,
		1e-1,2e-1,5e-1,
		1e0,2e0,5e0
	};
	if (idx < 0) idx = 0;
	if (idx > 26) idx = 26;
	setTimeDiv(t_ranges[idx]);
}

double Scope::getYposition(int ch)
{
	return atof(queryf("CHANnel%d:POSition?"));
}

void Scope::setYposition(int ch, double val)
{
	writef("CHANnel%d:POSition %f", ch, val);
}

double Scope::getVDiv(int ch)
{
	return atof(queryf("CHANnel%d:VDIV?", ch));
}

void Scope::setVDiv(int ch, double val)
{
	writef("CHAN%d:VDIV %f", ch, val);
}

void Scope::setVDivIdx(int ch, int idx)
{
	static double ranges[] = {
		1e-1,2e-1,5e-1,
		1e0,2e0,5e0,
		1e1,2e1,5e1,
	};
	if (idx < 0) idx = 0;
	if (idx > 9) idx = 9;
	setVDiv(ch, ranges[idx]);
}

void Scope::setViewXY()
{
	double range1, range2;

	range1 = getVDiv(1);
	range2 = getVDiv(2);
	if (range1 < range2)
		setVDiv(1, range2);
	else
		setVDiv(2, range1);
	writef("XY1:DISPLAY ON");
}

void Scope::setViewYT()
{
	return writef("XY1:DISPLAY OFF");
}

void Scope::setViewCh(int ch)
{
	writef("CHANnel%d:DISPlay ON", ch);
}

int Scope::getRecordLength()
{
	return atoi(queryf(":ACQuire:RLENgth?"));
}

void Scope::setRecordLength(const int val)
{
	writef("ACQuire:RLENgth %d", val);
}

void Scope::getWaveforms(int ch, double *voltages)
{
	double position, range, offset;
	int length, count;
	char c;

	length = this->getRecordLength();
	this->setViewCh(ch);

	int flag = this->getState();
	this->setStop();
	this->writef("WAVeform:STARt 0");
	this->writef("WAVeform:END %d", length - 1);
	this->writef("WAVeform:TRACe %d", ch);
	this->writef("WAVeform:FORMat RBYTe");
	position = atof(this->queryf("WAVeform:POSition?"));
	range = atof(this->queryf("WAVeform:RANGe?"));
	offset = atof(this->queryf("WAVeform:OFFSet?"));
	this->writefWithoutOPC("WAVeform:SEND?");

	c = this->viGetchar();
	if (c != '#') exit(EXIT_FAILURE);
	c = this->viGetchar();
	if (c < '0' && '9' < c) exit(EXIT_FAILURE);
	count = c - '0';
	for (int i = 0; i < count; i++)
	{
		c = this->viGetchar();
		if (c < '0' && '9' < c) exit(EXIT_FAILURE);
	}
	for (int i = 0; i < length; i++)
	{
		c = this->viGetchar();
		voltages[i] = range * ((unsigned char)c - position) / 25 + offset;
	}
	if (flag == 1)
		setRun();
}

void Scope::getWaveforms(double *voltages1, double *voltages2)
{
	int flag = this->getState();
	this->setStop();
	this->getWaveforms(1, voltages1);
	this->getWaveforms(2, voltages2);
	if (flag == 1)
		setRun();
}

void Scope::saveWaveforms(int ch, const char *filename)
{
	int length;
	double *v, dt;
	char *lines;

	length = getRecordLength();
	dt = getTimeDiv() * 10 / length;
	v = (double*)malloc(sizeof(double) * length);
	lines = (char*)malloc(sizeof(char) * VISA_NUM_STRINGS * length);

	getWaveforms(ch, v);

	for (int i = 0; i < length; i++)
	{
		snprintf(&(lines[i * VISA_NUM_STRINGS]), VISA_NUM_STRINGS, "%e, %e", (i + 1)*dt, v[i]);
	}
	saveCSV(VISA_NUM_STRINGS, length, lines, filename);
	free(v);
	free(lines);
}

void Scope::saveWaveforms(int ch)
{
	saveWaveforms(ch, VISA_FILENAME_CSV);
}

void Scope::saveWaveforms(const char *filename)
{
	double *v1, *v2;
	int length;
	double dt;
	char *lines;

	length = getRecordLength();
	dt = getTimeDiv() * 10 / length;
	v1 = (double*)malloc(sizeof(double) * length);
	v2 = (double*)malloc(sizeof(double) * length);
	lines = (char*)malloc(sizeof(char) * VISA_NUM_STRINGS * length);

	getWaveforms(v1, v2);

	for (int i = 0; i < length; i++)
	{
		snprintf(&(lines[i * VISA_NUM_STRINGS]), VISA_NUM_STRINGS, "%e, %e, %e", (i + 1)*dt, v1[i], v2[i]);
	}
	saveCSV(VISA_NUM_STRINGS, length, lines, filename);
	free(v1); v1 = NULL;
	free(v2); v2 = NULL;
	free(lines); lines = NULL;
}

void Scope::saveWaveforms()
{
	saveWaveforms(VISA_FILENAME_CSV);
}
