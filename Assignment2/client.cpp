#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>

#define MAXTHREAD 100

typedef struct Address {
	unsigned int val[4];

} Address;

typedef struct AddressObj {
	Address ip;
	Address subnet;
	Address network;
	Address broadcast;
	Address minHost;
	Address maxHost;
	int hosts;

} AddressObj;

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

Address vectToAddress(std::vector<std::string> vect) {
	Address ret;
	ret.val[0] = std::stoi(vect[0]);
	ret.val[1] = std::stoi(vect[1]);
	ret.val[2] = std::stoi(vect[2]);
	ret.val[3] = std::stoi(vect[3]);
	return ret;
}

void parseLine(std::string const &str, const char split, std::vector<std::string> &tokens) {
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(split, end)) != std::string::npos) {
		end = str.find(split, start);
		tokens.push_back(str.substr(start, end - start));
	}
}

void* sendToServer(void *adrObj_void_ptr) {
	AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
}

int ipCalculator(AddressObj res[]) {
	pthread_t tid[MAXTHREAD];
	std::string line;
	int n = 0;

	while (std::cin >> line) {
		std::vector<std::string> ip;
		std::vector<std::string> subnet;

		parseLine(line, '.', ip);
		std::cin >> line;
		parseLine(line, '.', subnet);	

		res[i].ip = vectToAddress(ip);
		res[i].subnet = vectToAddress(subnet);
	
		if (pthread_create(&tid[n], NULL, /* Function */, &res[n])) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}

		++n;
	
	}

	for (int k = 0; k < n; k++)
		pthread_join(tid[k], NULL);

	return n;
}

int main(int argc, char *argv[]) {

	static AddressObj res[MAXTHREAD];
	int n = ipCalculator(res);

	printf("\n");
	for (int i = 0; i < n; i++) {
		printInfo(res[i]);
	}

}
