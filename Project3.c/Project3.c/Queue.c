#include "Process_generation.h"
#include "Queue.h"
#include <stdio.h>
extern int time;

void InitQueue(Queue* q) {
	q->front = 0;
	q->rear = 0;
	q->size = 0;
}
void InitMultiQueue(Queue* q) {
	for (int i = 0; i < max_pri; i++) {
		q[i].front = 0;
		q[i].rear = 0;
		q[i].size = 0;
	}
}

char IsEmpty(Queue* q) {
	return q->size == 0;
}

char IsEmptyM(Queue* q) {
	for (int i = 0; i < max_pri; i++) {
		if (q[i].size != 0) return 0;
	}
	return 1;
}

char IsFull(Queue* q) {
	return q->size == queue_capacity;
}

char Enqueue(Queue* q, Process* p) {
	if (IsFull(q)) {
		printf("Queue is full.\n");
		return 0;
	}
	
	q->arr[q->rear] = p;
	q->rear = (q->rear + 1) % queue_capacity;
	q->size++;
	return 1;
}

Process* Dequeue(Queue* q) {
	if (IsEmpty(q))	return NULL;

	Process* p= q->arr[q->front];
	q->front = (q->front + 1) % queue_capacity;
	q->size--;
	return p;
}

void SortedEnqueue(Queue* q, Process* p, int (*compare)(const void*, const void*)) {
	if (q->size >= queue_capacity) {
		printf("Queue is full!\n");
		return;
	}

	int insert_pos = q->rear;
	int index = (q->front + q->size - 1) % queue_capacity;

	while (q->size > 0 && index != (q->front - 1 + queue_capacity) % queue_capacity) {
		if (compare(q->arr[index], p) <= 0) break;

		q->arr[(index + 1) % queue_capacity] = q->arr[index];  // 뒤로 밀기
		index = (index - 1 + queue_capacity) % queue_capacity;
	}
	q->arr[(index + 1) % queue_capacity] = p;
	q->rear = (q->rear + 1) % queue_capacity;
	q->size++;
}