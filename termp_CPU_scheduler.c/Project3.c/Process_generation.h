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
    int pid; //���μ����� Ư���Ѵ�
    int arrival_time;//���μ����� �߻��ϴ� ����(ready queue�� ���ԵǴ� ����)
    int burst_time;//cpu �۾��� �ʿ��� �ð��� ��
    int remain_time;//���� cpu �۾� �ð�
    int io_duration;//�� io �۾� �ð�(wait queue���� ����ϴ� �ð��� ��)
    char priority;//�켱����, ���� ���� ����
    int finish_time;//���μ����� �۾��� ����� ����

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