#pragma once
#define max_log 1000

typedef struct {
	int start_time;
	int pid;
	int duration;
} TimeLog;

void PrintTimeChart(Process* p, int n);