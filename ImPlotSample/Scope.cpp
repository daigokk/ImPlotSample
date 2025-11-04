#include "Scope.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <algorithm>

template<typename T>
T clamp(T value, T low, T high) {
    return (value < low) ? low : (high < value) ? high : value;
}

Scope::Scope() {
    char address[256];
    if (CppVisa::GetAddress(model, address)) {
        initialize(address);
    }
}

Scope::Scope(const char keyword[]) {
    char address[256];
    if (CppVisa::GetAddress(keyword, address)) {
        initialize(address);
    }
}

Scope::~Scope() {
    if (vi != VI_NULL) {
        setRecordLength(1250000);
        CppVisa::Printf(vi, "COMMunicate:REMote OFF");
        CppVisa::CloseInstrument(vi);
		vi = VI_NULL;
    }
}

void Scope::initialize(const char address[]) {
    vi = CppVisa::OpenInstrument(address);
    CppVisa::Printf(vi, "COMMunicate:HEADer OFF\n");
    setRecordLength(12500);
}

char* Scope::getError() {
    return CppVisa::Queryf(vi, "STATus:ERRor?\n");
}

void Scope::setRun() {
    CppVisa::Printf(vi, "STARt\n");
}

void Scope::setStop() {
    CppVisa::Printf(vi, "STOP\n");
}

int Scope::getState() {
    int state = std::atoi(CppVisa::Queryf(vi, ":STATus:CONDition?\n"));
    return (state % 2 == 1) ? 1 : 0;
}

void Scope::setLocal() {
    CppVisa::Printf(vi, ":COMMunicate:REMote OFF\n");
}

void Scope::setRemote() {
    CppVisa::Printf(vi, ":COMMunicate:REMote ON\n");
}

void Scope::setExecute() {
    CppVisa::Printf(vi, "ASETup:EXECute\n");
}

void Scope::setChOn(int ch) {
    CppVisa::Printf(vi, "CHANnel%d:DISPlay ON\n", ch);
}

void Scope::setChOff(int ch) {
    CppVisa::Printf(vi, "CHANnel%d:DISPlay OFF\n", ch);
}

void Scope::setProbeAtte(int ch, int val) {
    CppVisa::Printf(vi, "CHANnel%d:PROBe:MODE %d\n", ch, val);
}

int Scope::getTriggerMode() {
    std::string mode = CppVisa::QueryfString(vi, ":TRIGger:MODE?\n");
    if (mode.find("AUTO") != std::string::npos) return 1;
    if (mode.find("NORM") != std::string::npos) return 2;
    if (mode.find("ALEV") != std::string::npos) return 3;
    return 0;
}

void Scope::setTriggerAuto() {
    CppVisa::Printf(vi, ":TRIGger:MODE AUTO\n");
}

void Scope::setTriggerNormal() {
    CppVisa::Printf(vi, ":TRIGger:MODE NORMal\n");
}

void Scope::setTriggerMode(int mode) {
    if (mode == 0)
        setTriggerAuto();
    else
        setTriggerNormal();
}

double Scope::getTriggerLevel() {
    return std::atof(CppVisa::Queryf(vi, "TRIGger:SIMPle:LEVel?\n"));
}

void Scope::setTriggerLevel(double val) {
    CppVisa::Printf(vi, "TRIGger:SIMPle:LEVel %f\n", val);
}

void Scope::setTriggerSource(int ch) {
    CppVisa::Printf(vi, "TRIGger:SIMPle:SOURce %d\n", ch);
}

void Scope::setTriggerSourceEXT() {
    CppVisa::Printf(vi, "TRIGger:SIMPle:SOURce EXT\n");
}

double Scope::getTimeDiv() {
    return std::atof(CppVisa::Queryf(vi, "TIMebase:TDIV?\n"));
}

void Scope::setTimeDiv(double val) {
    CppVisa::Printf(vi, "TIMebase:TDIV %f\n", val);
}

void Scope::setTimeDivIdx(int idx) {
    static const double timeRanges[] = {
        1e-8,2e-8,5e-8, 1e-7,2e-7,5e-7, 1e-6,2e-6,5e-6,
        1e-5,2e-5,5e-5, 1e-4,2e-4,5e-4, 1e-3,2e-3,5e-3,
        1e-2,2e-2,5e-2, 1e-1,2e-1,5e-1, 1e0,2e0,5e0
    };
    idx = clamp(idx, 0, 26);
    setTimeDiv(timeRanges[idx]);
}

double Scope::getYposition(int ch) {
    return std::atof(CppVisa::Queryf(vi, "CHANnel%d:POSition?\n", ch));
}

void Scope::setYposition(int ch, double val) {
    CppVisa::Printf(vi, "CHANnel%d:POSition %f\n", ch, val);
}

double Scope::getVDiv(int ch) {
    return std::atof(CppVisa::Queryf(vi, "CHANnel%d:VDIV?\n", ch));
}

void Scope::setVDiv(int ch, double val) {
    CppVisa::Printf(vi, "CHANnel%d:VDIV %f\n", ch, val);
}

void Scope::setVDivIdx(int ch, int idx) {
    static const double ranges[] = {
        1e-1,2e-1,5e-1, 1e0,2e0,5e0, 1e1,2e1,5e1
    };
    idx = clamp(idx, 0, 8);
    setVDiv(ch, ranges[idx]);
}

void Scope::setViewXY() {
    double range1 = getVDiv(1);
    double range2 = getVDiv(2);
    if (range1 < range2)
        setVDiv(1, range2);
    else
        setVDiv(2, range1);
    CppVisa::Printf(vi, "XY1:DISPLAY ON\n");
}

void Scope::setViewYT() {
    CppVisa::Printf(vi, "XY1:DISPLAY OFF\n");
}

void Scope::setViewCh(int ch) {
    CppVisa::Printf(vi, "CHANnel%d:DISPlay ON\n", ch);
}

int Scope::getRecordLength() {
    return std::atoi(CppVisa::Queryf(vi, ":ACQuire:RLENgth?\n"));
}

void Scope::setRecordLength(int val) {
    CppVisa::Printf(vi, "ACQuire:RLENgth %d\n", val);
}

void Scope::getWaveforms(int ch, std::vector<double>& voltages) {
    int length = getRecordLength();
    voltages.resize(length);

    bool wasRunning = getState();
    setStop();

    CppVisa::Printf(vi, "WAVeform:STARt 0\n");
    CppVisa::Printf(vi, "WAVeform:END %d\n", length - 1);
    CppVisa::Printf(vi, "WAVeform:TRACe %d\n", ch);
    CppVisa::Printf(vi, "WAVeform:FORMat RBYTe\n");

    double position = std::atof(CppVisa::Queryf(vi, "WAVeform:POSition?\n"));
    double range = std::atof(CppVisa::Queryf(vi, "WAVeform:RANGe?\n"));
    double offset = std::atof(CppVisa::Queryf(vi, "WAVeform:OFFSet?\n"));

    CppVisa::Printf(vi, "WAVeform:SEND?\n");

    char c = CppVisa::Getchar(vi);
    if (c != '#') std::exit(EXIT_FAILURE);

    c = CppVisa::Getchar(vi);
    if (!std::isdigit(c)) std::exit(EXIT_FAILURE);

    int count = c - '0';
    for (int i = 0; i < count; ++i) {
        c = CppVisa::Getchar(vi);
        if (!std::isdigit(c)) std::exit(EXIT_FAILURE);
    }

    for (int i = 0; i < length; ++i) {
        c = CppVisa::Getchar(vi);
        voltages[i] = range * (static_cast<unsigned char>(c) - position) / 25.0 + offset;
    }

    if (wasRunning) setRun();
}

void Scope::getWaveforms(std::vector<double>& voltages1, std::vector<double>& voltages2) {
    bool wasRunning = getState();
    setStop();
    getWaveforms(1, voltages1);
    getWaveforms(2, voltages2);
    if (wasRunning) setRun();
}

void Scope::saveWaveforms(int ch, const char* filename) {
    int length = getRecordLength();
    double dt = getTimeDiv() * 10 / length;

    std::vector<double> voltages;
    getWaveforms(ch, voltages);

    std::ofstream file(filename);
    if (!file) {
        std::cerr << "[Error] ファイルを開けませんでした: " << filename << std::endl;
        return;
    }

    for (int i = 0; i < length; ++i) {
        file << (i + 1) * dt << ", " << voltages[i] << "\n";
    }
}

void Scope::saveWaveforms(const char* filename) {
    int length = getRecordLength();
    double dt = getTimeDiv() * 10 / length;

    std::vector<double> v1, v2;
    getWaveforms(v1, v2);

    std::ofstream file(filename);
    if (!file) {
        std::cerr << "[Error] ファイルを開けませんでした: " << filename << std::endl;
        return;
    }

    for (int i = 0; i < length; ++i) {
        file << (i + 1) * dt << ", " << v1[i] << ", " << v2[i] << "\n";
    }
}
