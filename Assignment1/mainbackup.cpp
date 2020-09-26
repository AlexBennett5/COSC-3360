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

	Address() {
		val[0] = 0;
		val[1] = 0;
		val[2] = 0;
		val[3] = 0;
	}


	void operator=(const Address &A) {
		val[0] = A.val[0];
		val[1] = A.val[1];
		val[2] = A.val[2];
		val[3] = A.val[3];
	}
};

Address networkAddress(Address ip, Address subnet) {	
	Address network(0,0,0,0);
	for (int i = 0; i < 4; i++)
		network.val[i] = ip.val[i] & subnet.val[i];
	return network;
}

Address broadcastAddress(Address ip, Address subnet) {
	Address broadcast(0,0,0,0);
	for (int i = 0; i < 4; i++)
		broadcast.val[i] = ip.val[i] | (~subnet.val[i] & 255);
	return broadcast;
}

class AddressObj {
	public:
		Address ip;
		Address subnet;
		Address network;
		Address broadcast;
		Address minHost;
		Address maxHost;
		int hosts;

	AddressObj() {
		hosts = 0;
	}

	AddressObj(const AddressObj &A) {
		ip = A.ip;
		subnet = A.subnet;
		network = A.network;
		broadcast = A.broadcast;
		minHost = A.minHost;
		maxHost = A.maxHost;
		hosts = A.hosts;
	}	
};

void networkOctet(std::vector<AddressObj>& res, int ind) {
        std::lock_guard<std::mutex> lock(adr_mutex);
        res[ind].network = networkAddress(res[ind].ip, res[ind].subnet);
}

void broadcastOctet(std::vector<AddressObj>& res, int ind) {
        std::lock_guard<std::mutex> lock(adr_mutex);
        res[ind].broadcast = broadcastAddress(res[ind].ip, res[ind].subnet);
}

void minHostOctet(std::vector<AddressObj>& res, int ind) {
        std::lock_guard<std::mutex> lock(adr_mutex);
        res[ind].minHost = networkAddress(res[ind].ip, res[ind].subnet);
        ++res[ind].minHost.val[3];
}

void maxHostOctet(std::vector<AddressObj>& res, int ind) {
        std::lock_guard<std::mutex> guard(adr_mutex);
        res[ind].maxHost = broadcastAddress(res[ind].ip, res[ind].subnet);
        --res[ind].maxHost.val[3];
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

void addressCalculator(std::vector<AddressObj>& res, int ind) {
        std::thread t1(networkOctet, std::ref(res), ind);
        std::thread t2(broadcastOctet, std::ref(res), ind);
        std::thread t3(minHostOctet, std::ref(res), ind);
        std::thread t4(maxHostOctet, std::ref(res), ind);
        t1.join();
        t2.join();
        t3.join();
        t4.join();

        adr_mutex.lock();
        res[ind].hosts = numberOfHosts(res[ind].subnet);
        adr_mutex.unlock();
}

void tokenize(std::string const &str, const char split, std::vector<std::string> &tokens) {
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(split, end)) != std::string::npos) {
		end = str.find(split, start);
		tokens.push_back(str.substr(start, end - start));
	}
}

Address vectToAddress(std::vector<std::string> vect) {
	int v1 = std::stoi(vect[0]);
	int v2 = std::stoi(vect[1]);
	int v3 = std::stoi(vect[2]);
	int v4 = std::stoi(vect[3]);
	return Address(v1, v2, v3, v4);
}

std::vector<AddressObj> IPcalculator() {
	std::vector<std::thread> threads;
	std::vector<AddressObj> res;
	std::string line;
	int i = 0;
	
	while (std::getline(std::cin, line)) {
		std::vector<std::string> adr;
		std::vector<std::string> ip;
		std::vector<std::string> subnet;

		tokenize(line, ' ', adr);
		tokenize(adr[0], '.', ip);
		tokenize(adr[1], '.', subnet);

		AddressObj newObj;	
		newObj.ip = vectToAddress(ip);
		newObj.subnet = vectToAddress(subnet);

		res.push_back(newObj);
		threads.push_back(std::thread(addressCalculator, std::ref(res), i));
		++i;
	}
		
	for (int k = 0; k < i; k++) {
		threads[k].join();
	}

	return res;
}

void printAddress(Address adr) {
	printf("%d.%d.%d.%d\n", adr.val[0], adr.val[1], adr.val[2], adr.val[3]);
}

void printInfo(AddressObj adr) {
	printf("IP Address: ");
	printAddress(adr.ip);
	printf("Subnet: ");
	printAddress(adr.subnet);
	printf("Network: ");
	printAddress(adr.network);
	printf("Broadcast: ");
	printAddress(adr.broadcast);
	printf("HostMin: ");
	printAddress(adr.minHost);
	printf("HostMax: ");
	printAddress(adr.maxHost);
	printf("# Hosts: %d\n\n", adr.hosts);
}

int main() {

	std::vector<AddressObj> res = IPcalculator();
	
	for (int i = 0; i < res.size(); i++) {
		printInfo(res[i]);
	}
}

