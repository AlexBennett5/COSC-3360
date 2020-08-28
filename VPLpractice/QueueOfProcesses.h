#ifndef QUEUE_H
#define QUEUE_H

class Process {

	int PID;
	int serviceTime;
	int arrivalTime;

	public:
		Process(int PID, int serviceTime, int arrivalTime) {
			this->PID = PID;
			this->serviceTime = serviceTime;
			this->arrivalTime = arrivalTime;
		}
};

class QueueOfProcesses {

	Process *arr;
	int front;
	int rear;
	int capacity;
	int size;
	
public:
	QueueOfProcesses(int capacity);
	~QueueOfProcesses();

	void enqueue(Process p);
	void dequeue();
	Process peek();
	int size();
	bool isFull();

};

#endif
