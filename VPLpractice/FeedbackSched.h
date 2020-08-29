#ifndef FEEDBACKSCHED_H
#define FEEDBACKSCHED_H

class QueueOfProcesses;

class FeedbackScheduler {

	Process *p_arr
	QueueOfProcesses *q_arr;
	int nextQuantum;

	public:
		FeedbackScheduler();
		~FeedbackScheduler();
		FeedbackScheduler(const FeedbackScheduler&);
		const FeedbackScheduler& operator=(const FeedbackScheduler&);
		bool readFromFile(std::string);
		void start();
		void printQueuesOfProcesses();
		void printStatistics();

};

#endif

