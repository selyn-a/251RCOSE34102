#include <stdio.h>
#include "Process_generation.h"
#define queue_capacity 100

typedef struct {
	Process* arr[queue_capacity];
	int front;
	int rear;
	int size;
} Queue;


void InitQueue(Queue* q);
void InitMultiQueue(Queue** q);
char IsEmpty(Queue* q);
char IsEmptyM(Queue* q);
char IsFull(Queue* q);
char Enqueue(Queue* q, Process* p);
Process* Dequeue(Queue* q);
void SortedEnqueue(Queue* q, Process* p, int (*compare)(const void*, const void*));
void Pre_Scheduler(Process p[], int n, int (*compare)(const void*, const void*));


