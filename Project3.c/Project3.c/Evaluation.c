#include "Process_generation.h"
#include <stdio.h>


void PrintTimeChart(Process* p, int n) {
	float average_wait = 0, average_TA = 0;
	printf("\n\n|  <pid>  |  <arrival>  |  <finish>  |  <TA>  |  <wait>  |\n");

	for (int i = 0; i < n; i++) {
		int a = p[i].arrival_time;
		int b = p[i].burst_time;
		int c = p[i].finish_time;
		int d = p[i].io_duration;
		printf("|    %d    |      %d      |", p[i].pid, a);
		
		if (p[i].finish_time < 10) printf(" ");
		printf("     %d     |", c);

		if (c-a < 10) printf(" ");
		printf("   %d   |", c - a);

		if (c-a-b < 10) printf(" ");
		printf("    %d    |\n", c - a - b-d);

		average_TA = average_TA + c - a;
		average_wait = average_wait+ c - a - b - d;
	}
	average_TA /= n;
	average_wait /= n;
	printf("\naverage turnaround: %f      average wait:%f", average_TA, average_wait);
}