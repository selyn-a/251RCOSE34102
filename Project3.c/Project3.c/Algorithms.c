#define _CRT_SECURE_NO_WARNINGS
#include "Process_generation.h"
#include "Queue.h"
#include "Evaluation.h"
#include "Algorithms.h"
#include <stdlib.h>



int time = 0;
int log_index = 0;
TimeLog log[max_log];

int CompareArrival(const void* a, const void* b) {
	Process* p1 = (Process*)a;
	Process* p2 = (Process*)b;
	
	return p1->arrival_time - p2->arrival_time;
}

int ComparePriority(const void* a, const void* b) {
	Process* p1 = (Process*)a;
	Process* p2 = (Process*)b;

	return p1->priority - p2->priority;
}

int CompareBurst(const void* a, const void* b) {
	Process* p1 = (Process*)a;
	Process* p2 = (Process*)b;
	return p1->remain_time - p2->remain_time;
}

void PushReadyQueue(Process* p, Queue* rq, int n, int (*compare)(const void*, const void*)) {
	for (int i = 0; i < n; i++) {
		if (p[i].arrival_time == time) {
			if (compare ==NULL ) Enqueue(rq, &p[i]);
			else SortedEnqueue(rq, &p[i], compare);
		}
	}
	return ;
}

void PushReadyQueueM(Process* p, Queue rq[], int n, CompareFunc compare[]) {
	for (int i = 0; i < n; i++) {
		if (p[i].arrival_time == time) {
			int level = p[i].priority;
			if (compare[level] == NULL) Enqueue(&rq[level], &p[i]);
			else SortedEnqueue(&rq[level], &p[i], compare[level]);
		}
	}
	return;
}

int IO_interrupt(Process* p) {
	if (!p->io_count) {
		return 0;
	}

	int time = p->burst_time - p->remain_time;

	for (int i = 0; i < p->io_count; i++) {
		if (p->io[i].start_time == time) {
			return 1;
		}
	}

	return 0;
}

void CheckIOCompletion(Queue* wq, Queue* rq, int (*compare)(const void*, const void*)) {
	int count = wq->size;

	for (int i = 0; i < count; i++) {
		Process* p = Dequeue(wq);
		p->io[0].duration--;

		if (p->io[0].duration <= 0) {
			for (int j = 0; j < p->io_count; j++) {
				p->io[j] = p->io[j + 1];
			}
			p->io_count--;
			if (compare == NULL) Enqueue(rq, p);
			else SortedEnqueue(rq, p, compare);
		}
		else Enqueue(wq, p);
	}
}

void CheckIOCompletionM(Queue* wq, Queue rq[], CompareFunc compare[]) {
	int count = wq->size;

	for (int i = 0; i < count; i++) {
		Process* p = Dequeue(wq);
		p->io[0].duration--;

		if (p->io[0].duration <= 0) {
			int level = p->priority;
			for (int j = 0; j < p->io_count; j++) {
				p->io[j] = p->io[j + 1];
			}
			p->io_count--;
			if (compare[level] == NULL) Enqueue(&rq[level], p);
			else SortedEnqueue(&rq[level], p, compare[level]);
		}
		else Enqueue(wq, p);
	}
}
Process* ContextSwitch(Queue* rq,Queue* wq, Process* p, int code, int (*compare)(const void*, const void*)) {

	if (code == COMPLETE) {
		p->finish_time = time;
		p = NULL;
		log[log_index-1].duration = time - log[log_index-1].start_time;
	}
	else if (code == IO) {
		p->io_duration += p->io[0].duration;
		Enqueue(wq, p);
		p = NULL;
		log[log_index-1].duration = time - log[log_index-1].start_time;
	}
	else if (code == PREEMPTION) {
		SortedEnqueue(rq, p, compare);
		p = NULL;
		log[log_index - 1].duration = time - log[log_index - 1].start_time;
	}
	else if (code == TIME_EXPIRED) {
		Enqueue(rq, p);
		p = NULL;
		log[log_index - 1].duration = time - log[log_index - 1].start_time;
	}

	if (!IsEmpty(rq)) p = Dequeue(rq);

	if (p != NULL) {
		log[log_index].start_time = time;
		log[log_index].pid = p->pid;
		log_index++;
	}

	if (log_index > 1000){
		printf("로그 overflow ");
	}
	
	return p;
}

Process* ContextSwitchM(Queue rq[], Queue* wq, Process* p, int code) {

	if (code == COMPLETE) {
		p->finish_time = time;
		p = NULL;
		log[log_index - 1].duration = time - log[log_index - 1].start_time;
	}
	else if (code == IO) {
		Enqueue(wq, p);
		p = NULL;
		log[log_index - 1].duration = time - log[log_index - 1].start_time;
	}
	else if (code == TIME_EXPIRED) {
		Enqueue(&rq[p->priority], p);
		p = NULL;
		log[log_index - 1].duration = time - log[log_index - 1].start_time;
	}
	for (int i = 0; i < max_pri; i++) {
		if (!IsEmpty(&rq[i])) p = Dequeue(&rq[i]);
		if (p != NULL) {
			log[log_index].start_time = time;
			log[log_index].pid = p->pid;
			log_index++;
			break;
		}
	}


	if (log_index > 1000) {
		printf("로그 overflow ");
	}

	return p;
}


void PrintGanttChart() {
	printf("\nGantt Chart:\n");
	int log_len = 0;
	int prev_end = log[0].start_time;;

	// 1. 상단 바
	while (log_len < log_index) {
		printf(" ");
		for (int i = log_len; i < log_len+8; i++) {
			if (i >= log_index) break;
			// idle 공간 있는지 확인
			if (i != 0 && log[i - 1].start_time + log[i - 1].duration != log[i].start_time) {
				printf("------ ");
			}
			printf("------ ");
		}
		printf("\n|");

		// 2. 프로세스 라벨
		for (int i = log_len; i < log_len+8; i++) {
			if (i >= log_index) break;
			if (i != 0 && log[i - 1].start_time + log[i - 1].duration != log[i].start_time) {
				printf("  ID  |");
			}
			printf("  P%d  |", log[i].pid);
		}

		// 3. 하단 바
		printf("\n ");
		for (int i = log_len; i < log_len+8; i++) {
			if (i >= log_index) break;
			// idle 공간 있는지 확인
			if (i != 0 && log[i - 1].start_time + log[i - 1].duration != log[i].start_time) {
				printf("------ ");
			}
			printf("------ ");
		}

		// 4. 시간 축
		printf("\n");
		printf("%-2d", prev_end);

		for (int i = log_len; i < log_len+8; i++) {
			if (i >= log_index) break;
			// idle 공간 있는지 확인
			if (i != 0 && prev_end != log[i].start_time) {
				if (log[i].start_time < 10) printf(" "); //숫자 간격 맞추기
				printf("     %d", log[i].start_time);
			}
			prev_end = log[i].start_time + log[i].duration;
			if (i != 0 && log[i].start_time < 10) printf(" "); //숫자 간격 맞추기
			printf("     %d", prev_end);
		}

		log_len += 8;
		printf("\n\n");
	}
}


void PrintQueue(Queue* q) {
	printf("[Queue %d] size = %d | front = %d | rear = %d\n",time ,q->size, q->front, q->rear);
	for (int i = 0, idx = q->front; i < q->size; i++, idx = (idx + 1) % queue_capacity) {
		printf("  [%d] PID: %d\n", idx, q->arr[idx] ? q->arr[idx]->pid : -1);
	}
}

void NonPre_Scheduler(Process p[], int n, int (*compare)(const void*, const void*)) {
	int completed = 0;
	Process* current = NULL;

	Queue rq;
	InitQueue(&rq);
	Queue wq;
	InitQueue(&wq);

	qsort(p, n, sizeof(Process), CompareArrival);
	time = 0;
	log_index = 0;

	while (completed < n) {
		PushReadyQueue(p, &rq, n, compare);
		//PrintQueue(&rq);

		if (current == NULL)
			current = ContextSwitch(&rq, &wq, current, START, NULL);

		else {
			if (current->remain_time == 0) {
				completed++;
				current = ContextSwitch(&rq, &wq, current, COMPLETE, NULL);
			}
			else if (IO_interrupt(current)) {
				current = ContextSwitch(&rq, &wq, current, IO, NULL);
			}
		}

		if (current != NULL) current->remain_time--;

		CheckIOCompletion(&wq, &rq, compare);
		time++;
	}
	log[log_index - 1].duration = time - 1 - log[log_index - 1].start_time;
}

char CheckPreemption(Process* current, Queue rq[], int(*compare)(const void*, const void*)) {
	Process* rq1 = NULL;
	if (!IsEmpty(rq)) {
		rq1 = rq->arr[rq->front];
		if (compare(current, rq1) > 0)
			return 1;
	}

	return 0;

}

void Pre_Scheduler(Process p[], int n, int (*compare)(const void*, const void*)) {
	int completed = 0;
	Process* current = NULL;

	Queue rq;
	InitQueue(&rq);
	Queue wq;
	InitQueue(&wq);

	qsort(p, n, sizeof(Process), CompareArrival);
	time = 0;
	log_index = 0;

	while (completed < n) {
		PushReadyQueue(p, &rq, n, compare);
		CheckIOCompletion(&wq, &rq, compare);
		//PrintQueue(&rq);

		if (current == NULL)
			current = ContextSwitch(&rq, &wq, current, START, NULL);

		else {
			if (IO_interrupt(current)) {
				current = ContextSwitch(&rq, &wq, current, IO, NULL);
			}
			else if (CheckPreemption(current, &rq, compare)) {
				//printf("\nPREEMPTION!!\n!");
				current = ContextSwitch(&rq, &wq, current, PREEMPTION, compare);
			}
			else if (current->remain_time == 0) {
				completed++;
				current = ContextSwitch(&rq, &wq, current, COMPLETE, NULL);
			}
		}

		if (current != NULL) current->remain_time--;

		time++;
	}

	log[log_index - 1].duration = time - 1 - log[log_index - 1].start_time;
}

void Aging(Queue* q, float rate) {

	int count = q->size;
	int index = q->front;

	for (int i = 0; i < count; i++) {
		if (q->arr[index] != NULL) {
			q->arr[index]->priority -= rate;
		}
		index = (index + 1) % queue_capacity;
	}
}

void FCFS(Process p[], int n) {
	NonPre_Scheduler(p, n, NULL);
}
void SJF_np(Process p[], int n) {
	NonPre_Scheduler(p, n, CompareBurst);
}
void SJF_p(Process p[], int n) {
	Pre_Scheduler(p, n, CompareBurst);
}
void Priority_np(Process p[], int n) {
	NonPre_Scheduler(p, n, ComparePriority);
}
void Priority_p(Process p[], int n) {
	Pre_Scheduler(p, n, ComparePriority);
}
void Priority_p_aging(Process p[], int n) {
	int completed = 0;
	float rate = 0.25;
	Process* current = NULL;

	printf("\naging rate를 설정해주세요. (기본:0.25)\n");
	scanf("%f", &rate);

	Queue rq;
	InitQueue(&rq);
	Queue wq;
	InitQueue(&wq);

	qsort(p, n, sizeof(Process), CompareArrival);
	time = 0;
	log_index = 0;

	while (completed < n) {
		PushReadyQueue(p, &rq, n, ComparePriority);
		//PrintQueue(&rq);

		if (current == NULL)
			current = ContextSwitch(&rq, &wq, current, START, NULL);

		else {
			if (IO_interrupt(current)) {
				current = ContextSwitch(&rq, &wq, current, IO, NULL);
			}
			else if (CheckPreemption(current, &rq, ComparePriority)) {
				//printf("\nPREEMPTION!!\n!");
				current = ContextSwitch(&rq, &wq, current, PREEMPTION, ComparePriority);
			}
			else if (current->remain_time == 0) {
				completed++;
				current = ContextSwitch(&rq, &wq, current, COMPLETE, NULL);
			}
		}

		if (current != NULL) current->remain_time--;

		Aging(&rq, rate);
		CheckIOCompletion(&wq, &rq, ComparePriority);
		time++;
	}

	log[log_index - 1].duration = time - 1 - log[log_index - 1].start_time;
}

void RoundRobin(Process p[], int n) {
	int time_quantum=3;

	printf("\ntime quantum은 몇으로 할까요?\n");
	getchar();
	scanf("%d", &time_quantum);


	int completed = 0;
	int quantum_left;
	Process* current = NULL;

	Queue rq;
	InitQueue(&rq);
	Queue wq;
	InitQueue(&wq);

	qsort(p, n, sizeof(Process), CompareArrival);
	time = 0;
	log_index = 0;

	while (completed < n) {
		int new = 1;
		PushReadyQueue(p, &rq, n, NULL);
		//PrintQueue(&rq);

		if (current == NULL) {
			current = ContextSwitch(&rq, &wq, current, START, NULL);
		}

		else {
			if (current->remain_time == 0) {
				completed++;
				current = ContextSwitch(&rq, &wq, current, COMPLETE, NULL);
			}
			else if (IO_interrupt(current)) {
				current = ContextSwitch(&rq, &wq, current, IO, NULL);
			}
			else if (quantum_left <= 0) {
				if (!IsEmpty(&rq)) current = ContextSwitch(&rq, &wq, current, TIME_EXPIRED, NULL);
				else new = 0;
			}
			else new = 0;
		}

		if (current != NULL) {
			current->remain_time--;
			if (new) quantum_left = time_quantum;
			quantum_left--;
		}

		CheckIOCompletion(&wq, &rq, NULL);
		time++;
	}
	log[log_index - 1].duration = time - 1 - log[log_index - 1].start_time;
}

void MultiLevel(Process p[], int n) {
	int completed = 0;
	int time_quantum[max_pri];
	int max_quantum = 0;
	Process* current = NULL;
	CompareFunc compare[max_pri];


	printf("\n각 레벨의 queue에 알고리즘을 지정할 수 있습니다.\n");

	for (int i = 0; i < max_pri; i++) {
		int choice = 0;
		printf("level %d: 1. FCFS  2. SJF  3. RR\n", i);
		while (1) {
			if (scanf("%d", &choice) != 1) {
				while (getchar() != '\n'); // 입력 버퍼 비우기
				printf("잘못된 입력입니다. 다시 선택하세요: ");
				continue;
			}
			switch (choice) {
			case 1:
				compare[i] = CompareArrival;
				break;
			case 2:
				compare[i] = CompareBurst;
				break;
			case 3:
				printf("\n time quantum은 몇으로 할까요?: ");
				while (getchar() != '\n');
				scanf("%d", &time_quantum[i]);
				compare[i] = NULL;  // RR은 비교 함수가 필요 없을 수도 있음
				break;
			default:
				printf("유효한 번호가 아닙니다. 다시 선택하세요: ");
				continue;
			}
			break;
		}

	}

	Queue rq[max_pri];
	Queue wq;
	int quantum_left= 9999999;
	InitMultiQueue(rq);
	InitQueue(&wq);

	qsort(p, n, sizeof(Process), CompareArrival);

	while (completed < n) {
		int new = 1;
		PushReadyQueueM(p, rq, n, compare);
		//PrintQueue(&rq);

		if (current == NULL)
			current = ContextSwitchM(rq, &wq, current, START, NULL);

		else {
			if (IO_interrupt(current)) {
				current = ContextSwitchM(rq, &wq, current, IO, NULL);
			}
			else if (current->remain_time == 0) {
				completed++;
				current = ContextSwitchM(rq, &wq, current, COMPLETE, NULL);
			}
			else if (quantum_left <= 0) {
				if (!IsEmptyM(rq)) {
					current = ContextSwitchM(rq, &wq, current, TIME_EXPIRED, NULL);
					quantum_left = 9999999;
				}
				else new = 0;
			}
			else new = 0;
		}

		if (current != NULL) {
			current->remain_time--;
			if (new && compare[current->priority] == NULL) {
				quantum_left = time_quantum[current->priority];
			}
			if (quantum_left != 9999999) {
				quantum_left--;
			}
		}

		CheckIOCompletionM(&wq, rq, compare);
		time++;
		if (time > 10000) {
			printf("FAIL!!!");
			break;
		}
	}

	log[log_index - 1].duration = time - 1 - log[log_index - 1].start_time;
}
void CopyProcessArray(Process** dest, Process* src, int n) {
	if (src == NULL || n <= 0) {
		fprintf(stderr, "Invalid source or size\n");
		exit(EXIT_FAILURE);
	}

	if (*dest != NULL) free(*dest);
	*dest = NULL;

	*dest = (Process*)malloc(n * sizeof(Process));
	if (*dest == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < n; i++) {
		(*dest)[i] = src[i];
	}
}
void DoAll(Process p[], int n) {
	Process* copy=NULL;

	// ? 복사 먼저 수행해야 함!
	CopyProcessArray(&copy, p, n);

	printf("--------------------FCFS-------------------\n");
	NonPre_Scheduler(copy, n, NULL);
	PrintTimeChart(copy, n);
	

	CopyProcessArray(&copy, p, n);  // 매번 초기화
	printf("\n\n--------------------NPSJF-------------------\n");
	NonPre_Scheduler(copy, n, CompareBurst);
	PrintTimeChart(copy, n);

	CopyProcessArray(&copy, p, n);
	printf("\n\n--------------------PSJF-------------------\n");
	Pre_Scheduler(copy, n, CompareBurst);
	PrintTimeChart(copy, n);

	CopyProcessArray(&copy, p, n);
	printf("\n\n--------------------NPPri-------------------\n");
	NonPre_Scheduler(copy, n, ComparePriority);
	PrintTimeChart(copy, n);

	CopyProcessArray(&copy, p, n);
	printf("\n\n--------------------PPri-------------------\n");
	Pre_Scheduler(copy, n, ComparePriority);
	PrintTimeChart(copy, n);

	CopyProcessArray(&copy, p, n);
	printf("\n\n--------------------RR-------------------\n");
	RoundRobin(copy, n);
	PrintTimeChart(copy, n);

	CopyProcessArray(&copy, p, n);
	printf("\n\n--------------------PPaging-------------------\n");
	Priority_p_aging(copy, n);
	PrintTimeChart(copy, n);



	//free(copy);  // ?? 마지막에 메모리 해제
}