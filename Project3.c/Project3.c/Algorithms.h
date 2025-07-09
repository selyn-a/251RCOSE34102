#pragma once
#include "Process_generation.h"
#define START 0
#define COMPLETE 1
#define IO 2
#define PREEMPTION 3
#define TIME_EXPIRED 4

typedef void (*Scheduler)(Process[], int);
typedef int (*CompareFunc)(const void*, const void*);

typedef struct {
	char preemption;
	CompareFunc compare;
	int time_quantum;
} Alg;

void FCFS(Process[], int);
void SJF_np(Process[], int);
void SJF_p(Process[], int);
void Priority_np(Process[], int);
void Priority_p(Process[], int);
void RoundRobin(Process[], int);
void Priority_p_aging(Process p[], int n);
void MultiLevel(Process p[], int n);
void DoAll(Process p[], int n);
void CopyProcessArray(Process* dest, Process* src, int n);

void NonPre_Scheduler(Process p[], int n, int (*compare)(const void*, const void*));
void Pre_Scheduler(Process p[], int n, int (*compare)(const void*, const void*));


int CompareArrival(const void* , const void* );
int ComparePriority(const void* a, const void* b);
int CompareBurst(const void* a, const void* b);

void PushReadyQueueM(Process* p, Queue* rq, int n, CompareFunc compare[]);
void PushReadyQueue(Process* p, Queue* rq, int n, int (*compare)(const void*, const void*));
int IO_interrupt(Process* p);
void CheckIOCompletion(Queue* wq, Queue* rq);
void CheckIOCompletionM(Queue* wq, Queue* rq, CompareFunc compare[]);
char CheckPreemption(Process* current, Queue rq[], int(*compare)(const void*, const void*));

Process* ContextSwitch(Queue* rq, Queue* wq, Process* p, int code, int (*compare)(const void*, const void*));
Process* ContextSwitchM(Queue* rq, Queue* wq, Process* p, int code);