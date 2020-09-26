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

typedef struct Octet {
	int val[6];

} Octet;

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

void* octetTail(void *octet_void_ptr) {
	Octet *oct = (Octet *) octet_void_ptr;
	oct->val[2] = oct->val[0] & oct->val[1];
	oct->val[3] = oct->val[0] | (~oct->val[1] & 255);
	oct->val[4] = oct->val[0] & oct->val[1];
	oct->val[4]++;
	oct->val[5] = oct->val[0] | (~oct->val[1] & 255);
	oct->val[5]--;
	return NULL;
}

void* octetNormal(void *octet_void_ptr) {
	Octet *oct = (Octet *) octet_void_ptr;
	oct->val[2] = oct->val[0] & oct->val[1];
	oct->val[3] = oct->val[0] | (~oct->val[1] & 255);
	oct->val[4] = oct->val[0] & oct->val[1];
	oct->val[5] = oct->val[0] | (~oct->val[1] & 255);
	return NULL;
}

void* addressCalculator(void *adrObj_void_ptr) {

	AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
	pthread_t tid[4];
	
	Octet oct[4];

	for (int i = 0; i < 4; i++) {
		oct[i].val[0] = adrObj->ip.val[i];
		oct[i].val[1] = adrObj->subnet.val[i];
		if (i != 3) {
			if (pthread_create(&tid[i], NULL, octetNormal, &oct[i])) {
				fprintf(stderr, "Error creating thread\n");
                		return NULL;
			}
		} else {
			if (pthread_create(&tid[i], NULL, octetTail, &oct[i])) {
                                fprintf(stderr, "Error creating thread\n");
                                return NULL;
			}
		}
	}

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);
	pthread_join(tid[3], NULL);


	for (int i = 0; i < 4; i++) {
		adrObj->network.val[i] = oct[i].val[2];
		adrObj->broadcast.val[i] = oct[i].val[3];
		adrObj->minHost.val[i] = oct[i].val[4];
		adrObj->maxHost.val[i] = oct[i].val[5];
	}

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

	static AddressObj res[MAXTHREAD];
	int n = IPcalculator(res);

	printf("\n");
	for (int i = 0; i < n; i++) {
		printInfo(res[i]);
	}

}

