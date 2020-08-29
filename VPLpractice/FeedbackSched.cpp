#include <cstdio>
#include "FeedbackSched.h"
#include "QueueOfProcesses.h"

#define MAXSIZE 30

FeedbackScheduler::FeedbackScheduler() {

	q_arr = new QueueOfProcesses[MAXSIZE];
	p_arr = new Process[MAXSIZE];
       	nextQuantum = 1;	
}
                
FeedbackScheduler::~FeedbackScheduler() {
	delete[] arr;
}
                
FeedbackScheduler(const FeedbackScheduler &f) {

}
                
const FeedbackScheduler& operator=(const FeedbackScheduler &f) {

}
                
bool FeedbackScheduler::readFromFile(std::string str) {

	std::ifstream infile(str);

	int index = 0;
	int pid, servicetime
	while (infile >> pid >> servicetime) {

		Process p = new Process(pid, servicetime);
		p_arr[index++] = p;
	}

}
                
void start() {

	printf("Initial ready queue:\n");



}
                
void printQueuesOfProcesses() {

}
                
void printStatistics() {

}



