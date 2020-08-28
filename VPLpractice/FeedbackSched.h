#ifndef FEEDBACKSCHED_H
#define FEEDBACKSCHED_H

class FeedbackScheduler {

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

