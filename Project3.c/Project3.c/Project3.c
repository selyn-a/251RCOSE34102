#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Process_generation.h"
#include "Queue.h"
#include "Algorithms.h"
#include "Evaluation.h"

extern int log_index;
extern TimeLog log[];


/////// 알고리즘 선택///////
Scheduler AlgorithmSelection() {
    char alg = 0;

    printf("어떤 알고리즘으로 CPU sheduling 할까요?\n\n");
    printf("1. FCFS   2. Non-preemptive SJF   3. Preemptive SJF\n");
    printf("4. Non-preemptive priority   5. Preemptive priority   6. Round Robin\n");
    printf("7. Preemptive priority with aging   8. multi-level   9. Do all and compare\n\n");
    printf("실행할 알고리즘의 번호: ");
    scanf("%hhd", &alg);

    switch (alg) {
        case 1: return FCFS;
        case 2: return SJF_np;
        case 3: return SJF_p;
        case 4: return Priority_np;
        case 5: return Priority_p;
        case 6: return RoundRobin;
        case 7: return Priority_p_aging;
        case 8: return MultiLevel;
        case 9: return DoAll;
        default:
            printf("잘못된 입력입니다. FCFS로 기본 설정합니다");
            return FCFS;
    }
}
void PrintLog() {
    printf("\n\n-----Log Print--------\n\n");
    for (int i = 0; i < log_index; i++) {
        printf("pid:%d start:%d duration:%d\n", log[i].pid, log[i].start_time, log[i].duration);
    }
    
}

int main() {
    int n;
    char ret_code = 0;
    Process* p = NULL;
    srand(time(NULL));
    
    ret_code=ProcessGenerator(&p, &n);
    if (!ret_code) return 0;

    Scheduler sel_alg = AlgorithmSelection();
    sel_alg(p, n);
    if (sel_alg != DoAll) {
        PrintLog();
        PrintGanttChart();
        PrintTimeChart(p, n);
    }

    free(p);
    return 0;
}