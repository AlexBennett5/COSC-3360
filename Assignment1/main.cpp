#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

std::mutex adr_mutex;

class Address {
	public:
	unsigned int val[4];

	Address(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4) {
		val[0] = v1;
		val[1] = v2;
		val[2] = v3;
		val[3] = v4;
	}
};

void networkOctet(std::vector<std::vector<Address> >& res, int ind) {
	std::lock_guard<std::mutex> lock(adr_mutex);
	for (int k = 0; k < 4; k++)
		res[ind][2].val[k] = res[ind][0].val[k] & res[ind][1].val[k];
}

void broadcastOctet(std::vector<std::vector<Address> >& res, int ind) {	
	std::lock_guard<std::mutex> lock(adr_mutex);
	for (int k = 0; k < 4; k++)
		res[ind][3].val[k] = res[ind][0].val[k] | (~res[ind][1].val[k] & 255);
}

void minHostOctet(std::vector<std::vector<Address> >& res, int ind) {
	std::lock_guard<std::mutex> lock(adr_mutex);
	for (int k = 0; k < 4; k++)
                res[ind][4].val[k] = res[ind][0].val[k] & res[ind][1].val[k];
	++res[ind][4].val[3];
}

void maxHostOctet(std::vector<std::vector<Address> >& res, int ind) {
	std::lock_guard<std::mutex> guard(adr_mutex);
        for (int k = 0; k < 4; k++)
		res[ind][5].val[k] = res[ind][0].val[k] | (~res[ind][1].val[k] & 255);
	--res[ind][5].val[3];
}

void addressCalculator(std::vector<std::vector<Address> >& res, int ind) {
	std::thread t1(networkOctet, std::ref(res), ind);
	std::thread t2(broadcastOctet, std::ref(res), ind);
	std::thread t3(minHostOctet, std::ref(res), ind);
	std::thread t4(maxHostOctet, std::ref(res), ind);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

std::vector<std::vector<Address> > IPcalculator(std::vector<std::vector<Address> > adr) {
	std::vector<std::thread> threads;
	int n = adr.size();
	std::vector<std::vector<Address> > res;

	for (int i = 0; i < n; i++) {
		std::vector<Address> x;
		for (int j = 0; j < 6; j++) {
			x.push_back(Address(0,0,0,0));
		}
		res.push_back(x);
	}

	for (int i = 0; i < n; i++) {
		res[i][0] = adr[i][0];
		res[i][1] = adr[i][1];
	}

	for (int i = 0; i < n; i++) {
		threads.push_back(std::thread(addressCalculator, std::ref(res), i));
	}
		
	for (int i = 0; i < n; i++) {
		threads[i].join();
	}

	return res;
}

int numberOfZeroBits(int num) {
	int count = 0;
	for (int i = 0; i < 8; i++) {
		if (!(num & 1))
			++count;
		num >>= 1;
	}
	return count;
}

int power(int base, int exp) {
	int res = 1;
	while (exp > 0) {
		res *= base;
		--exp;
	}
	return res;
}

int numberOfHosts(Address subnet) {
	int count = 0;
	for (int i = 0; i < 4; i++) 
		count += numberOfZeroBits(subnet.val[i]);
	return power(2, count) - 2;
}

void printAddress(Address adr) {
	printf("%d.%d.%d.%d\n", adr.val[0], adr.val[1], adr.val[2], adr.val[3]);
}

void printInfo(std::vector<Address> adr) {
	printf("IP Address: ");
	printAddress(adr[0]);
	printf("Subnet: ");
	printAddress(adr[1]);
	printf("Network: ");
	printAddress(adr[2]);
	printf("Broadcast: ");
	printAddress(adr[3]);
	printf("HostMin: ");
	printAddress(adr[4]);
	printf("HostMax: ");
	printAddress(adr[5]);
	printf("# Hosts: %d\n", numberOfHosts(adr[1]));
}

int main() {

	Address ip(192, 168, 1, 1);
	Address subnet(255, 255, 255, 0);
	std::vector<Address> t;
	t.push_back(ip);
	t.push_back(subnet);
	std::vector<std::vector<Address> > test;
	test.push_back(t);
	std::vector<std::vector<Address> > res = IPcalculator(test);

	printInfo(res[0]);
}

