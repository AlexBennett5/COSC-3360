#include "QueueOfProcesses.h"

QueueOfProcesses::QueueOfProcesses(int capacity, int quantum) {

	arr = new Process[capacity];
	this->capacity = capacity;
	this->quantum = quantum;
	size = 0;
	front = 0;
	rear = -1;

}
        
QueueOfProcess::~QueueOfProcesses() {

	delete[] arr;
}

void QueueOfProcess::enqueue(Process p) {

	rear = (rear + 1) % capacity;
	arr[rear] = p;
	size++;
}
        
void QueueOfProcess::dequeue() {

	front = (front + 1) % capacity;
	size--;
}
        
Process QueueOfProcess::peek() {

	return arr[front];
}
        
int QueueOfProcess::size() {

	return size;
}
        
bool QueueOfProcess::isFull() {

	return (size == capacity);
}

