#ifndef QUEUE_H
#define QUEUE_H

class Process {

	int PID;
	int serviceTime;
	int remainTime;
	int turnaroundTime;

	public:
		Process(int PID, int serviceTime) {
			this->PID = PID;
			this->serviceTime = serviceTime;
			this->remainTime = serviceTime;
			turnaroundTime = 0;
		}

		void setTurnaroundTime(int time) {
			turnaroundTime = time;
		}
};

class QueueOfProcesses {

	Process *arr;
	int front;
	int rear;
	int capacity;
	int size;
	int quantum;

public:
	QueueOfProcesses(int capacity, int quantum);
	~QueueOfProcesses();

	void enqueue(Process p);
	void dequeue();
	Process peek();
	int size();
	bool isFull();

};

#endif
