#pragma once
#include "visa.h"

ViSession vi_get_handle(char* visa_resource_name);
void vi_close_handle(ViSession vi);
void vi_close();
void vi_checkError(ViStatus status, const char* filename, int line);
char* vi_get_idn(ViSession vi);

int fg_get_error(ViSession vi);
void fg_set_freq(ViSession vi, double freq);
double fg_get_freq(ViSession vi);
int os_get_error(ViSession vi);
void os_set_timediv(ViSession vi, double sec);
double os_get_timediv(ViSession vi);
int os_save_Waveformsf(ViSession vi, const char* format, ...);