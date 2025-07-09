#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "Process_generation.h"
#define overlap 999999

///// ���μ��� ����/////////
void PrintProcessInfo(Process p[], int n, char ret_code) {
    for (int i = 0; i < n; i++) {
        printf("==== Process %d ====\n", i+1);
        printf("PID          : %d\n", p[i].pid);
        printf("Arrival Time : %d\n", p[i].arrival_time);
        printf("Burst Time   : %d\n", p[i].burst_time);
        if (ret_code == 2) printf("priority    : %hhd\n", p[i].priority);
        printf("I/O Count    : %d\n", p[i].io_count);

        if (p[i].io_count > 0) {
            printf("I/O Operations:\n");
            for (int j = 0; j < p[i].io_count; j++) {
                // ��ȿ�� start_time�� ���
                if (p[i].io[j].start_time != overlap) {
                    printf("  [I/O %d] Start: %d, Duration: %d\n",
                        j, p[i].io[j].start_time, p[i].io[j].duration);
                }
                else {
                    printf("  [I/O %d] Skipped (invalidated)\n", j);
                }
            }
        }
        else {
            printf("No I/O operations.\n");
        }
        printf("\n");
    }
}

void AssignIO(Process* p) {
    if (p->burst_time <= 1) {
        p->io_count = 0;
        return;
    }

    int num;
    num = rand() % (max_io + 1);
    if (num > p->burst_time - 1) num = p->burst_time - 1;
    p->io_count = num;

    for (int i = 0; i < num; i++) {
        p->io[i].start_time = rand() % (p->burst_time-1)+1; //���μ����� ���۰� ������ io�� �߻�x
        p->io[i].duration = rand() % (max_ioduration)+1;
    }

    if (num == 1) return;

    for (int i = 0; i < num;i++){
        int min_index = i;
        for (int j = i + 1; j < num; j++) {
            int f = p->io[i].start_time - p->io[j].start_time;
            if (f == 0 && p->io[i].start_time != overlap) {
                p->io[j].start_time = overlap;
                p->io_count--;
            }
            else if (f > 0) {
                min_index = j;
            }
        }
        if (i != min_index) {
            IO_operation temp = p->io[i];
            p->io[i] = p->io[min_index];
            p->io[min_index] = temp;
        }
    }
}


void AssignIOtoAll(Process p[], int n) {
    for (int i = 0; i < n; i++) {
        if (p[i].burst_time >= 3) {
            AssignIO(&p[i]);
        }
        else p[i].io_count = 0;
    }
}

void RandomProcess(Process p[], int n) {

    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        p[i].arrival_time = rand() % max_arrival_time;
        p[i].burst_time = (rand() % (max_burst_time - 1)) + 1;
        p[i].remain_time = p[i].burst_time;
        p[i].priority = (rand() % max_pri);//pri�� ������ 0~3
        p[i].finish_time = 0;
        p[i].io_duration = 0;
    }

    AssignIOtoAll(p, n);
}
char KeyboardInput(Process p[], int n) {
    char pri_f = 'y', ret_code;
    char random_f = 'y';
    int at, bt;
    printf("priority�� �����Ͻ� �ǰ���? (y/n)\n");
    scanf(" %c", &pri_f);
    while (getchar() != '\n');


    if (pri_f != 'y' && pri_f != 'n') {
        printf("�߸��� �Է��Դϴ�.");
        return 0;
    }


    else if (pri_f == 'y') {
        ret_code = 2;
        printf("arrival time, burst time, priority�� ����� �����Ͽ� ������� �Է����ּ���.\n");
        printf("priority�� 0~4���̷� �������ּ���.\n\n");
        printf("arrival_time burst_time priority\n");

        for (int i = 0; i < n; i++) {
            char pri_char;
            printf("Process %d: ", i + 1);
            scanf("%d %d %c", &at, &bt, &pri_char);
            getchar();

            p[i].pid = i + 1;
            p[i].arrival_time = at;
            p[i].burst_time = bt;
            p[i].remain_time = bt;
            p[i].priority = pri_char;
            p[i].finish_time = 0;
            p[i].io_count = 0;
            p[i].io_duration = 0;
        }
    }

    else {
        ret_code = 1;
        printf("arrival_time, burst_time�� ����� �����Ͽ� ������� �Է����ּ���.\n");
        for (int i = 0; i < n;i++) {
            printf("Process %d: ", i + 1);
            scanf("%d %d", &at, &bt);
            p[i].pid = i + 1;
            p[i].arrival_time = at;
            p[i].burst_time = bt;
            p[i].remain_time = bt;
            p[i].finish_time = 0;
            p[i].io_count = 0;
            p[i].io_duration = 0;
        }
    }

    printf("�Է��Ͻ� ���μ����鿡�� �������� io operation�� �߻���ų���? (y/n)\n");
    scanf(" %c", &random_f);

    if (random_f == 'y') AssignIOtoAll(p, n);
    return ret_code;
}

char ProcessGenerator(Process** p, int* n) {
    char key_f, ret_code = 2;
    printf("���μ��� �� ���� �����ұ��?\n");
    scanf("%d", n);
    *p = (Process*)malloc(sizeof(Process) * (*n));
    if (*p == NULL) {
        printf("�޸� �Ҵ翡 �����Ͽ����ϴ�.\n");
        return 0;
    }
    printf("���μ����� �������� �����Ͻðڽ��ϱ�? (y/n)\n");
    scanf(" %c", &key_f);

    if (key_f != 'y' && key_f != 'n') {
        printf("�߸��� �Է��Դϴ�.\n");
        return 0;
    }
    else if (key_f == 'y') ret_code = KeyboardInput(*p, *n);
    else RandomProcess(*p, *n);

    PrintProcessInfo(*p, *n, ret_code);
    printf("---------------------------------\n");
    printf("���μ��� ������ �Ϸ�Ǿ����ϴ�.\n\n\n");
    return ret_code;

}
