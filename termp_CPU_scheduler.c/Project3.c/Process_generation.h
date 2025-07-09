#ifndef PROCESS_GENERATION_H
#define PROCESS_GENERATION_H

#define max_arrival_time 10
#define max_burst_time 10
#define max_pri 4
#define max_io 3
#define max_ioduration 5

typedef struct {
    int start_time;
    int duration;
} IO_operation;

typedef struct {
    int pid; //프로세스를 특정한다
    int arrival_time;//프로세스가 발생하는 시점(ready queue에 삽입되는 시점)
    int burst_time;//cpu 작업이 필요한 시간의 합
    int remain_time;//남은 cpu 작업 시간
    int io_duration;//총 io 작업 시간(wait queue에서 대기하는 시간의 합)
    char priority;//우선순위, 작을 수록 좋다
    int finish_time;//프로세스의 작업이 종료된 시점

    IO_operation io[max_io];
    int io_count;
} Process;

void PrintProcessInfo(Process p[], int n, char ret_code);
void AssignIO(Process* p);
void AssignIOtoAll(Process p[], int n);
void RandomProcess(Process p[], int n);
char KeyboardInput(Process p[], int n);
char ProcessGenerator(Process** p, int* n);

#endif 