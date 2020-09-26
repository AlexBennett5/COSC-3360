#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>

#define MAXTHREAD 100

typedef struct Address {
	unsigned int val[4];

} Address;

Address networkAddress(Address ip, Address subnet) {	
	Address network;
	for (int i = 0; i < 4; i++)
		network.val[i] = ip.val[i] & subnet.val[i];
	return network;
}

Address broadcastAddress(Address ip, Address subnet) {
	Address broadcast;
	for (int i = 0; i < 4; i++)
		broadcast.val[i] = ip.val[i] | (~subnet.val[i] & 255);
	return broadcast;
}

typedef struct AddressObj {
	Address ip;
	Address subnet;
	Address network;
	Address broadcast;
	Address minHost;
	Address maxHost;
	int hosts;
		
} AddressObj;

void* networkOctet(void *adrObj_void_ptr) {
	
	AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
	adrObj->network = networkAddress(adrObj->ip, adrObj->subnet);
	return NULL;
}

void* broadcastOctet(void *adrObj_void_ptr) {
	
	AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
	adrObj->broadcast = broadcastAddress(adrObj->ip, adrObj->subnet);
	return NULL;
}

void* minHostOctet(void *adrObj_void_ptr) {
	
	AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
	adrObj->minHost = networkAddress(adrObj->ip, adrObj->subnet);
	adrObj->minHost.val[3]++;
	return NULL;
}

void* maxHostOctet(void *adrObj_void_ptr) {
	
	AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
	adrObj->maxHost = broadcastAddress(adrObj->ip, adrObj->subnet);
	adrObj->maxHost.val[3]--;
	return NULL;
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

void* addressCalculator(void *adrObj_void_ptr) {

	AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
	pthread_t tid[4];
	
	if (pthread_create(&tid[0], NULL, networkOctet, adrObj)) {
        	fprintf(stderr, "Error creating thread\n");
                return NULL;
        }
	
	if (pthread_create(&tid[1], NULL, broadcastOctet, adrObj)) {
        	fprintf(stderr, "Error creating thread\n");
                return NULL;
        }

	if (pthread_create(&tid[2], NULL, minHostOctet, adrObj)) {
        	fprintf(stderr, "Error creating thread\n");
                return NULL;
        }
	
	if (pthread_create(&tid[3], NULL, maxHostOctet, adrObj)) {
        	fprintf(stderr, "Error creating thread\n");
                return NULL;
        }

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);
	pthread_join(tid[3], NULL);

	adrObj->hosts = numberOfHosts(adrObj->subnet);

	return NULL;
}

void parseLine(std::string const &str, const char split, std::vector<std::string> &tokens) {
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(split, end)) != std::string::npos) {
		end = str.find(split, start);
		tokens.push_back(str.substr(start, end - start));
	}
}

Address vectToAddress(std::vector<std::string> vect) {
	Address ret;
	ret.val[0] = std::stoi(vect[0]);
	ret.val[1] = std::stoi(vect[1]);
	ret.val[2] = std::stoi(vect[2]);
	ret.val[3] = std::stoi(vect[3]);
	return ret;
}

int IPcalculator(AddressObj res[]) {
	pthread_t tid[MAXTHREAD];
	std::string line;
	int i = 0;
	
	while (std::cin >> line) {
		std::vector<std::string> adr;
		std::vector<std::string> ip;
		std::vector<std::string> subnet;

		parseLine(line, '.', ip);
		std::cin >> line;
		parseLine(line, '.', subnet);	

		res[i].ip = vectToAddress(ip);
		res[i].subnet = vectToAddress(subnet);

		if (pthread_create(&tid[i], NULL, addressCalculator, &res[i])) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		++i;
	}
		
	for (int k = 0; k < i; k++) {
		pthread_join(tid[k], NULL);
	}

	return i;
}

void printAddress(Address adr) {
	printf("%d.%d.%d.%d\n", adr.val[0], adr.val[1], adr.val[2], adr.val[3]);
}

void printInfo(AddressObj adr) {
	printf("\nIP Address: ");
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
	printf("# Hosts: %d\n", adr.hosts);
}

int main() {

	static AddressObj res[MAXTHREAD];
	int n = IPcalculator(res);

	for (int i = 0; i < n; i++) {
		printInfo(res[i]);
	}
}

